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
#ifndef _ANCH_NETWORK_SOCKET_H_
#define _ANCH_NETWORK_SOCKET_H_

#include <iostream>

namespace anch {
  namespace network {

    /**
     * Socket type enumerations
     *
     * @author Vincent Lachenal
     */
    enum SocketType {
      /** Unknown socket */
      UNKNOWN = 0,

      /** TCP socket */
      TCP,

      /** UDP socket */
      UDP,

      /** POSIX socket (not available for Windows TM operating system) */
      POSIX,
    };

  }
}

namespace anch {
  namespace network {

    /**
     * Network socket management class
     *
     * @author Vincent Lachenal
     */
    class Socket {

    private:
      // Attributes +
      /** Destination IP address */
      std::string _destIpAddr;

      /** Destination port */
      uint16_t _port;

      /** The socket type */
      SocketType _type;
      // Attributes -

    public:
      // Constructors +
      /**
       * {@link Socket} constructor
       *
       * @param destIpAddr Destination IP address
       * @param port Destination port
       * @param type The socket type
       */
      Socket(const std::string& destIpAddr,
	     uint16_t port,
	     anch::network::SocketType type = anch::network::SocketType.UNKNOWN);
      // Constructors -

      // Destructor +
      /**
       * {@link Socket} destructor
       */
      virtual ~Socket();
      // Destructor -

    public:
      // Methods +
      void bind();

      void connect();

      void listen();

      void send();

      void receive(std::string message);

      void close();
      // Methods -

    public:
      // Accessors +
      /**
       * Destination IP address getter
       *
       * @return The destination IP address
       */
      inline const std::string& getDestIpAddr() const {
	return _destIpAddr;
      }

      /**
       * Destination IP address setter
       *
       * @param destIpAddr The destination IP address to set
       */
      inline void setDestIpAddr(const std::string& destIpAddr) {
	_destIpAddr = destIpAddr;
      }
      /**
       * Destination port getter
       *
       * @return The destination port
       */
      inline unint16_t getPort() const {
	return _port;
      }

      /**
       * Destination port setter
       *
       * @param port The destination port to set
       */
      inline void setDestIpAddr(unint16_t port) {
	_port = port;
      }

      /**
       * Socket type getter
       *
       * @return The socket type
       */
      inline anch::network::SocketType getType() const {
	return _type;
      }

      /**
       * Socket type setter
       *
       * @param type The socket type to set
       */
      inline void setType(SocketType type) {
	_type = type;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_NETWORK_SOCKET_H_
