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


using std::string;

using anch::network::Socket;
using anch::network::SocketType;
using anch::network::TcpSocket;
using anch::network::IOException;


const int BUFFER_SIZE = 1024;


// Constructors +
/*!
 * \ref TcpSocket default constructor
 */
TcpSocket::TcpSocket():
  Socket(SocketType::TCP) {
  // Nothing to do
}

/*!
 * \ref TcpSocket constructor
 *
 * \param ipAddress The IP address
 * \param port The port number
 *
 * \throw IOException any error
 */
TcpSocket::TcpSocket(const string& ipAddress, uint16_t port) throw(IOException):
  Socket(ipAddress, port, SocketType::TCP) {
  // Nothing to do
}
// Constructors -

// Destructors +
/*!
 * \ref TcpSocket destructor
 */
TcpSocket::~TcpSocket() noexcept {
  // Nothing to do
}
// Destructors -


// Methods +
/*!
 * Send a message on socket
 *
 * \param message The message to send
 *
 * \throw anch::network::IOException Network error while sending message
 */
void
TcpSocket::send(const string& message) throw(IOException) {
  int res = ::send(_sock, message.data(), message.size() + 1, 0);
  if(res == SOCKET_ERROR) {
    throw IOException("Error on send()");
  }
}

/*!
 * Receive a message on socket
 *
 * \param message The string where to write the message
 *
 * \throw anch::network::IOException Network error while receiving message
 */
void
TcpSocket::receive(string& message) throw(IOException) {
  // Receive message +
  char buffer[BUFFER_SIZE];
  memset(&buffer, 0, BUFFER_SIZE);
  int res = 0;
  while((res = ::recv(_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
    message += buffer;
    memset(&buffer, 0, BUFFER_SIZE);
  }
  if(res == SOCKET_ERROR) {
    throw IOException("Error on recv()");
  }
  // '\0' character is automatically append by std::string
  // Receive message -

  // Notify everybody that a message has been received +
  notifyObservers(SocketEvent(message));
  // Notify everybody that a message has been received -
}
// Methods -


// Accessors +
/*!
 * Get the socket domain
 *
 * \return The POSIX socket domain
 */
int
TcpSocket::getDomain() const {
  return AF_INET;
}

/*!
 * Get the socket service type
 *
 * \return The POSIX socket service type
 */
int
TcpSocket::getType() const {
  return SOCK_STREAM;
}
// Accessors -
