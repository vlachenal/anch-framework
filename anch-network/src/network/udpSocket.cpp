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


using std::string;

using anch::network::Socket;
using anch::network::SocketType;
using anch::network::UdpSocket;
using anch::network::IOException;


// Constructors +
/**
 * {@link UdpSocket} default constructor
 */
UdpSocket::UdpSocket():
  Socket(SocketType::UDP) {
  // Nothing to do
}

/**
 * {@link UdpSocket} constructor
 *
 * @param ipAddress The IP address
 * @param port The port number
 */
UdpSocket::UdpSocket(const string& ipAddress, uint16_t port) throw(IOException):
  Socket(ipAddress, port, SocketType::UDP) {
  // Nothing to do
}
// Constructors -

// Destructors +
/**
 * {@link UdpSocket} destructor
 */
UdpSocket::~UdpSocket() throw() {
  // Nothing to do
}
// Destructors -


// Methods +
/**
 * Listen on socket
 *
 * @throw anch::network::IOException Error while listening on the socket
 */
void
UdpSocket::listen() throw(IOException) {
  // Nothing to do
}

/**
 * Accept client connection.<br>
 * This method do nothing since UDP is not connected
 *
 * @param socket The socket which describes client connection
 *
 * @throw anch::network::IOException Never on UDP socket
 */
void
UdpSocket::accept(Socket& socket) throw(IOException) {
  // Nothing to do
}

/**
 * Connect to remote socket.<br>
 * This method do nothing since UDP is not connected.
 *
 * @throw anch::network::IOException Never on UDP socket
 */
void
UdpSocket::connect() throw(IOException) {
  // Nothing to do
}

/**
 * Send a message on socket
 *
 * @param message The message to send
 *
 * @throw anch::network::IOException Network error while sending message
 */
void
UdpSocket::send(const string& message) throw(IOException) {
  int res = ::sendto(_sock,
		     message.data(),
		     message.size() + 1,
		     0,
		     _address->ai_addr,
		     sizeof(*(_address->ai_addr)));
  if(res == SOCKET_ERROR) {
    throw IOException("Error on sendto()");
  }
}

/**
 * Send a message on socket
 *
 * @param message The message to send
 *
 * @throw anch::network::IOException Network error while sending message
 */
void
UdpSocket::send(const string& message, const sockaddr_storage& peerAddr)
  throw(IOException) {
  int res = ::sendto(_sock,
		     message.data(),
		     message.size() + 1,
		     0,
		     (SOCKADDR*)&peerAddr,
		     sizeof(sockaddr_storage));
  if(res == SOCKET_ERROR) {
    throw IOException("Error on sendto()");
  }
}

/**
 * Receive a message on socket
 *
 * @param message The string where to write the message
 *
 * @throw anch::network::IOException Network error while receiving message
 */
void
UdpSocket::receive(string& message) throw(IOException) {
  // Receive message +
  char buffer[1024];
  int res = 0;
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
/**
 * Get the socket domain
 *
 * @return The POSIX socket domain
 */
int
UdpSocket::getDomain() const {
  return AF_INET;
}

/**
 * Get the socket service type
 *
 * @return The POSIX socket service type
 */
int
UdpSocket::getType() const {
  return SOCK_DGRAM;
}
// Accessors -