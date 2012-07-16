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
#ifndef _ANCH_NETWORK_UDP_SOCKET_H_
#define _ANCH_NETWORK_UDP_SOCKET_H_

#include "network/socket.hpp"


namespace anch {
  namespace network {

    /**
     * UDP socket implementation
     *
     * @author Vincent Lachenal
     */
    class UdpSocket: public Socket {

    private:
      // Attributes +
      // Attributes -

    public:
      // Constructors +
      /**
       * {@link UdpSocket} constructor
       *
       * @param ipAddress The IP address
       * @param port The port number
       */
      UdpSocket(const std::string& ipAddress, uint16_t port)
	throw(anch::network::IOException);
      // Constructors -

      // Destructors +
      /**
       * {@link UdpSocket} destructor
       */
      virtual ~UdpSocket() throw();
      // Destructors -

    public:
      // Methods +
      /**
       * Listen on socket
       */
      virtual void listen() throw(anch::network::IOException);

      /**
       * Accept client connection
       *
       * @param socket The socket which describes client connection
       */
      virtual void accept(Socket& socket) throw(anch::network::IOException);

      /**
       * Send a message on socket
       *
       * @param message The message to send
       *
       * @throws IOException Network error while sending message
       */
      virtual void send(const std::string& message) throw(anch::network::IOException);

      /**
       * Receive a message on socket
       *
       * @param message The string where to write the message
       *
       * @throws IOException Network error while sending message
       */
      virtual void receive(std::string& message) throw(anch::network::IOException);
      // Methods -

      // Accessors +
      /**
       * Get the socket domain
       *
       * @return The POSIX socket domain
       */
      virtual int getDomain() const;

      /**
       * Get the socket service type
       *
       * @return The POSIX socket service type
       */
      virtual int getType() const;
      // Accessors -

    };

  }
}

#endif // _ANCH_NETWORK_UDP_SOCKET_H_
