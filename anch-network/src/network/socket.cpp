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
#include "network/socket.hpp"


using std::string;

using anch::network::Socket;
using anch::network::SocketType;


// Constructors +
/**
 * {@link Socket} constructor
 *
 * @param destIpAddr Destination IP address
 * @param port Destination port
 */
Socket::Socket(const string& destIpAddr, uint16_t port, SocketType type):
  _destIpAddr(destIpAddr), _port(port), _type(type) {
  // Nothing to do
}
// Constructors -

// Destructor +
/**
 * {@link Socket} destructor
 */
Socket::~Socket() {
  // Nothing to do
}
// Destructor -
