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
#include "network/socketEvent.hpp"

using std::string;

using anch::network::SocketEvent;


// Constructors +
/*!
 * \ref SocketEvent constructor
 *
 * \param message The message received on socket
 */
SocketEvent::SocketEvent(const string& message): _message(message) {
  // Nothing to do
}

/*!
 * \ref SocketEvent constructor
 *
 * \param message The message received on socket
 * \param address The address where the message has been sent
 */
SocketEvent::SocketEvent(const std::string& message,
			 const sockaddr_storage& address):
  _message(message),
  _address(address) {
  // Nothing to do
}
// Constructors -

// Destructor +
/*!
 * \ref SocketEvent destructor
 */
SocketEvent::~SocketEvent() {
  // Nothing to do
}
// Destructor -
