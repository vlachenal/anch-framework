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
#include "network/udpSocket.hpp"

#include <sys/socket.h>
#include <netinet/in.h>


using anch::network::Socket;
using anch::network::SocketType;
using anch::network::UdpSocket;
using anch::network::IOException;


// Constructors +
UdpSocket::UdpSocket():
  Socket(SocketType::UDP) {
  // Nothing to do
}

UdpSocket::UdpSocket(const std::string& ipAddress, uint16_t port):
  Socket(ipAddress, port, SocketType::UDP) {
  // Nothing to do
}
// Constructors -

// Destructors +
UdpSocket::~UdpSocket() noexcept {
  // Nothing to do
}
// Destructors -


// Methods +
void
UdpSocket::listen() {
  // Nothing to do
}

void
UdpSocket::accept(Socket&) {
  // Nothing to do
}

void
UdpSocket::connect() {
  // Nothing to do
}

void
UdpSocket::send(const std::string& message) {
  ssize_t res = ::sendto(_sock,
			 message.data(),
			 message.size() + 1,
			 0,
			 _address->ai_addr,
			 sizeof(*(_address->ai_addr)));
  if(res == SOCKET_ERROR) {
    throw IOException("Error on sendto()");
  }
}

void
UdpSocket::send(const std::string& message, const sockaddr_storage& peerAddr) {
  ssize_t res = ::sendto(_sock,
			 message.data(),
			 message.size() + 1,
			 0,
			 (SOCKADDR*)&peerAddr,
			 sizeof(sockaddr_storage));
  if(res == SOCKET_ERROR) {
    throw IOException("Error on sendto()");
  }
}

void
UdpSocket::receive(std::string& message) {
  // Receive message +
  char buffer[1024];
  ssize_t res = 0;
  sockaddr_storage peerAddr;
  socklen_t peerAddrLen = sizeof(sockaddr_storage);
  if((res = ::recvfrom(_sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&peerAddr, &peerAddrLen)) > 0) {
    message += buffer;
  }
  if(res < 0) {
    throw IOException("Error on recvfrom()");
  }
  // '\0' character is automatically append by std::string
  // Receive message -

  // Notify everybody that a message has been received +
  notifyObservers(SocketEvent(message, peerAddr));
  // Notify everybody that a message has been received -
}
// Methods -


// Accessors +
int
UdpSocket::getDomain() const {
  return AF_INET;
}

int
UdpSocket::getType() const {
  return SOCK_DGRAM;
}
// Accessors -
