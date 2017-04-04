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
#ifndef _ANCH_NETWORK_TCP_SOCKET_H_
#define _ANCH_NETWORK_TCP_SOCKET_H_

#include "network/socket.hpp"

namespace anch {
  namespace network {

    /*!
     * TCP socket implementation
     *
     * \author Vincent Lachenal
     */
    class TcpSocket: public Socket {

    public:
      // Constructors +
      /*!
       * \ref TcpSocket default constructor
       */
      TcpSocket();

      /*!
       * \ref TcpSocket constructor
       *
       * \param ipAddress The IP address
       * \param port The port number
       *
       * \throw anch::network::IOException Error while creating the socket
       */
      TcpSocket(const std::string& ipAddress, uint16_t port)
	throw(anch::network::IOException);
      // Constructors -

      // Destructors +
      /*!
       * \ref TcpSocket destructor
       */
      virtual ~TcpSocket() noexcept;
      // Destructors -

    public:
      // Methods +
      /*!
       * Send a message on socket
       *
       * \param message The message to send
       *
       * \throw anch::network::IOException Network error while sending message
       */
      virtual void send(const std::string& message) throw(anch::network::IOException);

      /*!
       * Receive a message on socket
       *
       * \param message The string where to write the message
       *
       * \throw anch::network::IOException Network error while receiving message
       */
      virtual void receive(std::string& message) throw(anch::network::IOException);
      // Methods -

      // Accessors +
      /*!
       * Get the socket domain
       *
       * \return The POSIX socket domain
       */
      virtual int getDomain() const;

      /*!
       * Get the socket service type
       *
       * \return The POSIX socket service type
       */
      virtual int getType() const;
      // Accessors -

    };

  }
}

#endif // _ANCH_NETWORK_TCP_SOCKET_H_
