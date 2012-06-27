/*
    This file is part of ANCH Framework.

    ANCH Framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ANCH Framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "network/tcpSocket.hpp"

#include <sys/socket.h>
#include <netinet/in.h>

using std::string;

using anch::network::Socket;
using anch::network::SocketType;
using anch::network::TcpSocket;

// Constructors +
/**
 * {@link TcpSocket} constructor
 *
 * @param ipAddress The IP address
 * @param port The port number
 */
TcpSocket::TcpSocket(const string& ipAddress, uint16_t port):
  Socket(ipAddress, port, SocketType::TCP) {
  // Nothing to do
}
// Constructors -

// Destructors +
/**
 * {@link TcpSocket} destructor
 */
TcpSocket::~TcpSocket() {
  // Nothing to do
}
// Destructors -


// Accessors +
/**
 * Get the socket domain
 *
 * @return The POSIX socket domain
 */
int
TcpSocket::getDomain() const {
  return AF_INET;
}

/**
 * Get the socket service type
 *
 * @return The POSIX socket service type
 */
int
TcpSocket::getType() const {
  return SOCK_STREAM;
}
// Accessors -
