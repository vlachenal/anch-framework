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

// Microsoft Windows operating systems defintions +
#ifdef ANCH_WINDOWS
#include <winsock32.h>
// Microsoft Windows operating systems defintions -

// POSIX operating systems definitions +
#elif defined ANCH_POSIX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define closesocket(s) ::close(s)
#ifndef SO_REUSEPORT
#define SO_REUSEPORT SO_REUSEADDR
#endif
#endif
// POSIX operating systems definitions -

#include <sstream>


using std::string;
using std::ostringstream;

using anch::events::Observable;
using anch::network::Socket;
using anch::network::Direction;
using anch::network::SocketEvent;
using anch::network::SocketType;
using anch::network::IOException;


// Constructors +
/**
 * {@link Socket} constructor.
 *
 * @param type The socket type
 */
Socket::Socket(anch::network::SocketType type):
  Observable<SocketEvent>(),
  _ipAddress(""),
  _port(-1),
  _type(type),
  _sock(INVALID_SOCKET),
  _backlog(5) {
  // Nothing to do
}

/**
 * {@link Socket} constructor.
 * Backlog is set to 5 by default. You can change it using the setter before call listen method.
 *
 * @param ipAddress IP address
 * @param port Destination port
 * @param type The socket type
 *
 * @throw anch::network::IOException Error while creating the socket
 */
Socket::Socket(const string& ipAddress, uint16_t port, SocketType type)
  throw(IOException):
  Observable<SocketEvent>(),
  _ipAddress(ipAddress),
  _port(port),
  _type(type),
  _backlog(5) {
  // Open the socket according to socket type +
  _sock = INVALID_SOCKET;
  int sockType = -1;
  switch(type) {
  case SocketType::TCP:
    sockType = SOCK_STREAM;
    break;
  case SocketType::UDP:
    sockType = SOCK_DGRAM;
    break;
#ifdef ANCH_POSIX
  case SocketType::POSIX:
    sockType = SOCK_STREAM;
    break;
#endif
  default:
    // Nothing to do
    break;
  }
  // Open the socket according to socket type -

  // Retrieve address informations +
  addrinfo hints;
  addrinfo* result;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = sockType;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  ostringstream strPortOss;
  strPortOss << _port;
  int res = getaddrinfo(_ipAddress.empty() ? NULL : _ipAddress.data(),
			strPortOss.str().data(), &hints, &result);
  if(res != 0) {
    throw IOException("Error on getaddrinfo()", res);
  }

  // Check addresses by opening socket and bind
  for(_address = result ; _address != NULL ; _address = _address->ai_next) {
    _sock = ::socket(_address->ai_family,
		     _address->ai_socktype,
		     _address->ai_protocol);
    if(_sock != INVALID_SOCKET) {
      break;
    }
  }

  freeaddrinfo(result);
  if(_address == NULL) {
    throw IOException("No working address");
  }
  // Retrieve address informations -
}
// Constructors -

// Destructor +
/**
 * {@link Socket} destructor
 */
Socket::~Socket() throw() {
  // Nothing to do
}
// Destructor -


// Methods +
/**
 * Bind socket
 *
 * @throw anch::network::IOException Error while binding the socket
 */
void
Socket::bind() throw(IOException) {
  int reusePort = true;
  int res = setsockopt(_sock, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));
  if(res == SOCKET_ERROR) {
    throw IOException("Error on setsockopt()");
  }
  res = ::bind(_sock, _address->ai_addr, _address->ai_addrlen);
  if(res == SOCKET_ERROR) {
    throw IOException("Error on bind()");
  }
}

/**
 * Connect to remote socket
 *
 * @throw anch::network::IOException Error while connectin the client socket to the server socket
 */
void
Socket::connect() throw(IOException) {
  int res = ::connect(_sock, _address->ai_addr, _address->ai_addrlen);
  if(res != 0) {
    throw IOException("Error on connect()");
  }
}

/**
 * Listen on socket
 *
 * @throw anch::network::IOException Error while listening on the socket
 */
void
Socket::listen() throw(IOException) {
  int res = ::listen(_sock, _backlog);
  if(res == SOCKET_ERROR) {
    throw IOException("Error on listen()");
  }
}

/**
 * Accept client connection
 *
 * @throw anch::network::IOException Error while accepting client connection
 */
void
Socket::accept(Socket& socket) throw(IOException) {
  SOCKADDR_IN csin;
  socklen_t length = sizeof(csin);
  SOCKET sock = ::accept(_sock, (SOCKADDR*)&csin, &length);
  if(sock == INVALID_SOCKET) {
    throw IOException("Error on accept()");
  }
  socket._sock = sock;
}

/**
 * Shutdown data flow between client and server.<br>
 * This method has to be called by server.
 *
 * @param how Direction of the data flow which has to be closed
 *
 * @throw anch::network::IOException Network error while shutting down data transfer
 */
void
Socket::shutdown(Direction how) throw(IOException) {
  int res = ::shutdown(_sock, static_cast<int>(how));
  if(res != 0) {
    throw IOException("Error on shutdown()");
  }
}

/**
 * Close the socket
 */
void
Socket::close() throw() {
  // Shutdown the socket to release every data on the socket +
  try {
    shutdown(Direction::BOTH);
  } catch(const IOException& e) {
    // Nothing to do
  }
  // Shutdown the socket to release every data on the socket -
  closesocket(_sock);
}
// Methods -
