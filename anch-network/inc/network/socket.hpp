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

#include "events/observable.hpp"
#include "network/socketEvent.hpp"


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
    class Socket : public anch::events::Observable<anch::network::SocketEvent> {

    private:
      // Attributes +
      /** IP address */
      std::string _ipAddress;

      /** Destination port */
      uint16_t _port;

      /** The socket type */
      SocketType _type;

      /** The socket */
      int _sock;

      /** The client socket (if current socket is server) */
      int _csock;

      /** The number of connection in waiting state */
      int _backlog;
      // Attributes -

    public:
      // Constructors +
      /**
       * {@link Socket} constructor.
       * Backlog is set to 5 by default. You can change it using the setter before call listen method.
       *
       * @param ipAddress IP address
       * @param port Destination port
       * @param type The socket type
       */
      Socket(const std::string& ipAddress,
	     uint16_t port,
	     anch::network::SocketType type = anch::network::SocketType::UNKNOWN);
      // Constructors -

      // Destructor +
      /**
       * {@link Socket} destructor
       */
      virtual ~Socket();
      // Destructor -

    public:
      // Methods +
      /**
       * Bind socket
       */
      void bind();

      /**
       * Connect to remote socket
       */
      void connect();

      /**
       * Listen on socket
       */
      void listen();

      /**
       * Accept client connection
       */
      void accept();

      /**
       * Send a message on socket
       */
      void send(const std::string& message);

      /**
       * Receive a message on socket
       *
       * @param message The string where to write the message
       */
      void receive(std::string message);

      /**
       * Close the socket
       */
      void close();
      // Methods -

    public:
      // Accessors +
      /**
       * Get the socket domain
       *
       * @return The POSIX socket domain
       */
      virtual int getDomain() const = 0;

      /**
       * Get the socket service type
       *
       * @return The POSIX socket service type
       */
      virtual int getType() const = 0;

      /**
       * IP address getter
       *
       * @return The IP address
       */
      inline const std::string& getIpAddress() const {
	return _ipAddress;
      }

      /**
       * IP address setter
       *
       * @param ipAddress The IP address to set
       */
      inline void setIpAddress(const std::string& ipAddress) {
	_ipAddress = ipAddress;
      }
      /**
       * Destination port getter
       *
       * @return The destination port
       */
      inline uint16_t getPort() const {
	return _port;
      }

      /**
       * Destination port setter
       *
       * @param port The destination port to set
       */
      inline void setIpAddress(uint16_t port) {
	_port = port;
      }

      /**
       * Socket type getter
       *
       * @return The socket type
       */
      inline anch::network::SocketType getSocketType() const {
	return _type;
      }

      /**
       * Socket type setter
       *
       * @param type The socket type to set
       */
      inline void setSocketType(SocketType type) {
	_type = type;
      }

      /**
       * Backlog getter
       *
       * @return The backlog
       */
      inline int getBacklog() const {
	return _backlog;
      }

      /**
       * Backlog setter
       *
       * @param backlog The backlog to set
       */
      inline void setBacklog(int backlog) {
	_backlog = backlog;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_NETWORK_SOCKET_H_
