/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "network/socket.hpp"

#include <cerrno>
#include <utility>

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/time.h>
#include <unistd.h>

using anch::network::Address;
using anch::network::Direction;
using anch::network::IOException;
using anch::network::Socket;
using anch::network::SocketType;

namespace {

#ifdef MSG_NOSIGNAL
  /*! Send flags: do not raise SIGPIPE */
  constexpr int SEND_FLAGS = MSG_NOSIGNAL;
#else
  /*! Send flags (SIGPIPE is disabled with SO_NOSIGPIPE) */
  constexpr int SEND_FLAGS = 0;
#endif

  /*!
   * Set close-on-exec flag and disable SIGPIPE when \c MSG_NOSIGNAL is not available
   *
   * \param fd the file descriptor
   */
  void
  initDescriptor(int fd) {
    ::fcntl(fd, F_SETFD, FD_CLOEXEC);
#if !defined(MSG_NOSIGNAL) && defined(SO_NOSIGPIPE)
    int on = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));
#endif
  }

  /*!
   * Connect socket (wait for connection when \c connect is interrupted)
   *
   * \param fd the file descriptor
   * \param address the address to connect to
   *
   * \return \c 0 on success, the \c errno value otherwise
   */
  int
  connectTo(int fd, const Address& address) {
    if(::connect(fd, address.data(), address.size()) == 0) {
      return 0;
    }
    if(errno != EINTR) {
      return errno;
    }
    // Connection goes on asynchronously: wait for it +
    pollfd pfd{fd, POLLOUT, 0};
    int res = 0;
    do {
      res = ::poll(&pfd, 1, -1);
    } while(res < 0 && errno == EINTR);
    if(res < 0) {
      return errno;
    }
    int error = 0;
    socklen_t length = sizeof(error);
    if(::getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &length) != 0) {
      return errno;
    }
    return error;
    // Connection goes on asynchronously: wait for it -
  }

  /*!
   * Set timeout socket option
   *
   * \param fd the file descriptor
   * \param option \c SO_RCVTIMEO or \c SO_SNDTIMEO
   * \param timeout the timeout
   */
  void
  setTimeout(int fd, int option, std::chrono::milliseconds timeout) {
    timeval value{};
    value.tv_sec = static_cast<time_t>(timeout.count() / 1000);
    value.tv_usec = static_cast<suseconds_t>((timeout.count() % 1000) * 1000);
    if(::setsockopt(fd, SOL_SOCKET, option, &value, sizeof(value)) != 0) {
      throw IOException("Error on setsockopt()");
    }
  }

}

// Constructors +
Socket::Socket() noexcept: _fd(-1) {
  // Nothing to do
}

Socket::Socket(int fd) noexcept: _fd(fd) {
  // Nothing to do
}

Socket::Socket(int family, SocketType type): _fd(::socket(family, static_cast<int>(type), 0)) {
  if(_fd < 0) {
    throw IOException("Error on socket()");
  }
  initDescriptor(_fd);
}

Socket::Socket(Socket&& other) noexcept: _fd(std::exchange(other._fd, -1)) {
  // Nothing to do
}
// Constructors -

// Destructor +
Socket::~Socket() noexcept {
  close();
}
// Destructor -

// Operators +
Socket&
Socket::operator=(Socket&& other) noexcept {
  if(this != &other) {
    close();
    _fd = std::exchange(other._fd, -1);
  }
  return *this;
}
// Operators -

// Methods +
Socket
Socket::connect(const std::string& host, uint16_t port, SocketType type) {
  int error = 0;
  for(const Address& address : Address::resolve(host, port, type)) {
    Socket sock(address.getFamily(), type);
    error = connectTo(sock._fd, address);
    if(error == 0) {
      return sock;
    }
  }
  throw IOException("Error on connect() to " + host + ':' + std::to_string(port), error);
}

Socket
Socket::bind(const std::string& host, uint16_t port, SocketType type) {
  int error = 0;
  for(const Address& address : Address::resolve(host, port, type, true)) {
    Socket sock(address.getFamily(), type);
    int on = 1;
    if(::setsockopt(sock._fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
      throw IOException("Error on setsockopt()");
    }
    if(address.getFamily() == AF_INET6 && host.empty()) { // any address: accept IPv4 too
      int off = 0;
      ::setsockopt(sock._fd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
    }
    if(::bind(sock._fd, address.data(), address.size()) == 0) {
      return sock;
    }
    error = errno;
  }
  throw IOException("Error on bind() to " + host + ':' + std::to_string(port), error);
}

void
Socket::listen(int backlog) {
  if(::listen(_fd, backlog) != 0) {
    throw IOException("Error on listen()");
  }
}

Socket
Socket::accept(Address* peer) {
  Address address;
  socklen_t length = sizeof(address._storage);
  int fd = -1;
  do {
    fd = ::accept(_fd, reinterpret_cast<sockaddr*>(&address._storage), &length);
  } while(fd < 0 && errno == EINTR);
  if(fd < 0) {
    throw IOException("Error on accept()");
  }
  initDescriptor(fd);
  if(peer != NULL) {
    address._length = length;
    *peer = address;
  }
  return Socket(fd);
}

std::size_t
Socket::send(const char* data, std::size_t size) {
  ssize_t res = -1;
  do {
    res = ::send(_fd, data, size, SEND_FLAGS);
  } while(res < 0 && errno == EINTR);
  if(res < 0) {
    throw IOException("Error on send()");
  }
  return static_cast<std::size_t>(res);
}

void
Socket::sendAll(const char* data, std::size_t size) {
  while(size > 0) {
    std::size_t sent = send(data, size);
    data += sent;
    size -= sent;
  }
}

std::size_t
Socket::receive(char* data, std::size_t size) {
  ssize_t res = -1;
  do {
    res = ::recv(_fd, data, size, 0);
  } while(res < 0 && errno == EINTR);
  if(res < 0) {
    throw IOException("Error on recv()");
  }
  return static_cast<std::size_t>(res);
}

std::size_t
Socket::sendTo(const char* data, std::size_t size, const Address& to) {
  ssize_t res = -1;
  do {
    res = ::sendto(_fd, data, size, SEND_FLAGS, to.data(), to.size());
  } while(res < 0 && errno == EINTR);
  if(res < 0) {
    throw IOException("Error on sendto() to " + to.toString());
  }
  return static_cast<std::size_t>(res);
}

std::size_t
Socket::receiveFrom(char* data, std::size_t size, Address& from) {
  iovec iov{data, size};
  msghdr msg{};
  msg.msg_name = &from._storage;
  msg.msg_namelen = sizeof(from._storage);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  ssize_t res = -1;
  do {
    res = ::recvmsg(_fd, &msg, 0);
  } while(res < 0 && errno == EINTR);
  if(res < 0) {
    throw IOException("Error on recvmsg()");
  }
  from._length = msg.msg_namelen;
  if((msg.msg_flags & MSG_TRUNC) != 0) {
    throw IOException("Datagram is larger than " + std::to_string(size) + " bytes", EMSGSIZE);
  }
  return static_cast<std::size_t>(res);
}

void
Socket::shutdown(Direction how) {
  if(::shutdown(_fd, static_cast<int>(how)) != 0) {
    throw IOException("Error on shutdown()");
  }
}

void
Socket::close() noexcept {
  if(_fd >= 0) {
    ::close(_fd);
    _fd = -1;
  }
}

void
Socket::setReceiveTimeout(std::chrono::milliseconds timeout) {
  setTimeout(_fd, SO_RCVTIMEO, timeout);
}

void
Socket::setSendTimeout(std::chrono::milliseconds timeout) {
  setTimeout(_fd, SO_SNDTIMEO, timeout);
}
// Methods -

// Accessors +
Address
Socket::getLocalAddress() const {
  Address address;
  socklen_t length = sizeof(address._storage);
  if(::getsockname(_fd, reinterpret_cast<sockaddr*>(&address._storage), &length) != 0) {
    throw IOException("Error on getsockname()");
  }
  address._length = length;
  return address;
}

Address
Socket::getPeerAddress() const {
  Address address;
  socklen_t length = sizeof(address._storage);
  if(::getpeername(_fd, reinterpret_cast<sockaddr*>(&address._storage), &length) != 0) {
    throw IOException("Error on getpeername()");
  }
  address._length = length;
  return address;
}

int
Socket::getDescriptor() const noexcept {
  return _fd;
}

bool
Socket::isOpen() const noexcept {
  return _fd >= 0;
}
// Accessors -
