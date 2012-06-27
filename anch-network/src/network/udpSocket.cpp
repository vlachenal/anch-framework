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
#include "network/udpSocket.hpp"

#include <sys/socket.h>
#include <netinet/in.h>

using std::string;

using anch::network::Socket;
using anch::network::SocketType;
using anch::network::UdpSocket;

// Constructors +
/**
 * {@link UdpSocket} constructor
 *
 * @param ipAddress The IP address
 * @param port The port number
 */
UdpSocket::UdpSocket(const string& ipAddress, uint16_t port):
  Socket(ipAddress, port, SocketType::UDP) {
  // Nothing to do
}
// Constructors -

// Destructors +
/**
 * {@link UdpSocket} destructor
 */
UdpSocket::~UdpSocket() {
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
