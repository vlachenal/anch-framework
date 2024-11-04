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
#include "network/tcpSocket.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


using anch::network::Socket;
using anch::network::SocketType;
using anch::network::TcpSocket;
using anch::network::IOException;


const int BUFFER_SIZE = 1024;


// Constructors +
TcpSocket::TcpSocket():
  Socket(SocketType::TCP) {
  // Nothing to do
}

TcpSocket::TcpSocket(const std::string& ipAddress, uint16_t port):
  Socket(ipAddress, port, SocketType::TCP) {
  // Nothing to do
}
// Constructors -

// Destructors +
TcpSocket::~TcpSocket() noexcept {
  // Nothing to do
}
// Destructors -


// Methods +
void
TcpSocket::send(const std::string& message) {
  ssize_t res = ::send(_sock, message.data(), message.size() + 1, 0);
  if(res == SOCKET_ERROR) {
    throw IOException("Error on send()");
  }

  // Notify everybody that a message has been received +
  notifyObservers(SocketEvent(message));
  // Notify everybody that a message has been received -
}

void
TcpSocket::receive(std::string& message) {
  // Receive message +
  char buffer[BUFFER_SIZE];
  std::memset(&buffer, 0, BUFFER_SIZE);
  // '\0' character is automatically append by std::string
  while(read(buffer, BUFFER_SIZE) > 0) {
    message += buffer;
    std::memset(&buffer, 0, BUFFER_SIZE);
  }
  // Receive message -

  // Notify everybody that a message has been received +
  notifyObservers(SocketEvent(message));
  // Notify everybody that a message has been received -
}

std::size_t
TcpSocket::write(const char* buffer, std::size_t size) {
  ssize_t res = ::send(_sock, buffer, size, 0);
  if(res == SOCKET_ERROR) {
    throw IOException("Error on send()");
  }

  // Notify everybody that a message has been received +
  notifyObservers(SocketEvent(buffer));
  // Notify everybody that a message has been received -
  return static_cast<std::size_t>(res);
}

std::size_t
TcpSocket::read(char* buffer, std::size_t size) {
  ssize_t res = ::recv(_sock, buffer, size, 0);
  if(res == SOCKET_ERROR) {
    throw IOException("Error on recv()");
  }

  // Notify everybody that a message has been received +
  notifyObservers(SocketEvent(buffer));
  // Notify everybody that a message has been received -
  return static_cast<std::size_t>(res);
}
// Methods -


// Accessors +
int
TcpSocket::getDomain() const {
  return AF_INET;
}

int
TcpSocket::getType() const {
  return SOCK_STREAM;
}
// Accessors -
