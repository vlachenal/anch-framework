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
#ifndef _ANCH_NETWORK_SOCKET_EVENT_H_
#define _ANCH_NETWORK_SOCKET_EVENT_H_

#include <iostream>

namespace anch {
  namespace network {

    /**
     * Socket event
     *
     * @author Vincent Lachenal
     */
    class SocketEvent {

    private:
      // Attributes +
      /** The message received on socket */
      std::string _message;
      // Attributes -

    public:
      // Constructors +
      /**
       * {@link SocketEvent} constructor
       *
       * @param message The message received on socket
       */
      SocketEvent(const std::string& message);
      // Constructors -

      // Destructor +
      /**
       * {@link SocketEvent} destructor
       */
      virtual ~SocketEvent();
      // Destructor -

    public:
      // Accessors +
      /**
       * Get the event message
       *
       * @return The message
       */
      inline const std::string& getMessage() const {
	return _message;
      };
      // Accessors -

    };

  }
}

#endif // _ANCH_NETWORK_SOCKET_EVENT_H_
