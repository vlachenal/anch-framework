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
#ifndef _ANCH_NETWORK_SOCKET_H_
#define _ANCH_NETWORK_SOCKET_H_

#include <cstring>

#include "events/observable.hpp"
#include "network/socketEvent.hpp"
#include "network/ioException.hpp"


// Microsoft Windows operating systems defintions +
#ifdef ANCH_WINDOWS
typedef int socklen_t;
// Microsoft Windows operating systems defintions -

// POSIX operating systems definitions +
#elif defined ANCH_POSIX
#include <netdb.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#endif
// POSIX operating systems definitions -


namespace anch {
  namespace network {

    /*!
     * Socket type enumerations
     *
     * \author Vincent Lachenal
     */
    enum class SocketType {
      /*! Unknown socket */
      UNKNOWN = 0,

      /*! TCP socket */
      TCP,

      /*! UDP socket */
      UDP,

#ifdef ANCH_POSIX
      /*! POSIX socket (not available for Microsoft Windows TM operating system) */
      POSIX,
#endif
    };

    /*!
     * Socket direction.\n
     * Usefull for shutdown method.
     *
     * \author Vincent Lachenal
     */
    enum class Direction {
      /*! Reception */
      RECEPTION = 0,

      /*! Transmission */
      TRANSMISSION = 1,

      /*! Reception and transmission */
      BOTH = 2,
    };

    /*!
     * Network socket management class
     *
     * \author Vincent Lachenal
     */
    class Socket : public anch::events::Observable<SocketEvent> {

    private:
      // Attributes +
      /*! IP address */
      std::string _ipAddress;

      /*! Destination port */
      uint16_t _port;

      /*! The socket type */
      SocketType _type;

    protected:
      /*! The socket */
      SOCKET _sock;

      /*! The number of connection in waiting state */
      int _backlog;

      /*! The address informations */
      addrinfo* _address;
      // Attributes -

      // Constructors +
    protected:
      /*!
       * \ref Socket constructor.
       *
       * \param type The socket type
       */
      Socket(SocketType type);

    public:
      /*!
       * \ref Socket constructor.\n
       * Backlog is set to 5 by default. You can change it using the setter before call listen method.
       *
       * \param ipAddress IP address
       * \param port Destination port
       * \param type The socket type
       *
       * \throw anch::network::IOException Error while creating the socket
       */
      Socket(const std::string& ipAddress, uint16_t port, SocketType type = SocketType::UNKNOWN);
      // Constructors -

      // Destructor +
      /*!
       * \ref Socket destructor
       */
      virtual ~Socket() noexcept;
      // Destructor -

    public:
      // Methods +
      /*!
       * Bind socket
       *
       * \throw anch::network::IOException Error while binding the socket
       */
      virtual void bind();

      /*!
       * Connect to remote socket
       *
       * \throw anch::network::IOException Error while connectin the client socket to the server socket
       */
      virtual void connect();

      /*!
       * Listen on socket
       *
       * \throw anch::network::IOException Error while listening on the socket
       */
      virtual void listen();

      /*!
       * Accept client connection
       *
       * \param socket The socket which describes client connection
       *
       * \throw anch::network::IOException Error while accepting client connection
       */
      virtual void accept(Socket& socket);

      /*!
       * Send a message on socket
       *
       * \param message The message to send
       *
       * \throw anch::network::IOException Network error while sending message
       */
      virtual void send(const std::string& message) = 0;

      /*!
       * Receive a message on socket
       *
       * \param message The messages which has been received
       *
       * \throw anch::network::IOException Network error while receiving message
       */
      virtual void receive(std::string& message) = 0;

      /*!
       * Receive a message on socket
       *
       * \throw anch::network::IOException Network error while receiving message
       */
      virtual void receive();

      /*!
       * Shutdown data flow between client and server.\n
       * This method has to be called by server.
       *
       * \param how Direction of the data flow which has to be closed
       *
       * \throw anch::network::IOException Network error while shutting down data transfer
       */
      virtual void shutdown(Direction how = Direction::BOTH);

      /*!
       * Close the socket
       */
      virtual void close() noexcept;
      // Methods -

    public:
      // Accessors +
      /*!
       * Get the socket domain
       *
       * \return The POSIX socket domain
       */
      virtual int getDomain() const = 0;

      /*!
       * Get the socket service type
       *
       * \return The POSIX socket service type
       */
      virtual int getType() const = 0;

      /*!
       * IP address getter
       *
       * \return The IP address
       */
      inline const std::string& getIpAddress() const {
	return _ipAddress;
      }

      /*!
       * IP address setter
       *
       * \param ipAddress The IP address to set
       */
      inline void setIpAddress(const std::string& ipAddress) {
	_ipAddress = ipAddress;
      }

      /*!
       * Destination port getter
       *
       * \return The destination port
       */
      inline uint16_t getPort() const {
	return _port;
      }

      /*!
       * Destination port setter
       *
       * \param port The destination port to set
       */
      inline void setIpAddress(uint16_t port) {
	_port = port;
      }

      /*!
       * Socket type getter
       *
       * \return The socket type
       */
      inline anch::network::SocketType getSocketType() const {
	return _type;
      }

      /*!
       * Socket type setter
       *
       * \param type The socket type to set
       */
      inline void setSocketType(SocketType type) {
	_type = type;
      }

      /*!
       * Backlog getter
       *
       * \return The backlog
       */
      inline int getBacklog() const {
	return _backlog;
      }

      /*!
       * Backlog setter
       *
       * \param backlog The backlog to set
       */
      inline void setBacklog(int backlog) {
	_backlog = backlog;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_NETWORK_SOCKET_H_
