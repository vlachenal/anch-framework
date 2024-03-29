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
#pragma once

#include "network/socket.hpp"


namespace anch::network {

  /*!
   * UDP socket implementation
   *
   * \author Vincent Lachenal
   */
  class UdpSocket: public Socket {

  public:
    // Constructors +
    /*!
     * \ref UdpSocket default constructor
     */
    UdpSocket();

    /*!
     * \ref UdpSocket constructor
     *
     * \param ipAddress The IP address
     * \param port The port number
     *
     * \throw anch::network::IOException Error while creating the socket
     */
    UdpSocket(const std::string& ipAddress, uint16_t port);
    // Constructors -

    // Destructors +
    /*!
     * \ref UdpSocket destructor
     */
    virtual ~UdpSocket() noexcept;
    // Destructors -

  public:
    // Methods +
    /*!
     * Listen on socket
     *
     * \throw anch::network::IOException Error while listening on the socket
     */
    virtual void listen();

    /*!
     * Connect to remote socket.\n
     * This method do nothing since UDP is not connected.
     *
     * \throw anch::network::IOException Never on UDP socket
     */
    virtual void connect();

    /*!
     * Accept client connection.\n
     * This method do nothing since UDP is not connected
     *
     * \param socket The socket which describes client connection
     *
     * \throw anch::network::IOException Never on UDP socket
     */
    virtual void accept(Socket& socket);

    /*!
     * Send a message on socket
     *
     * \param message The message to send
     *
     * \throw anch::network::IOException Network error while sending message
     */
    virtual void send(const std::string& message);

    /*!
     * Send a message on socket
     *
     * \param message The message to send
     * \param peerAddr The address where the message has to be sent
     *
     * \throw anch::network::IOException Network error while sending message
     */
    virtual void send(const std::string& message, const sockaddr_storage& peerAddr);

    /*!
     * Receive a message on socket
     *
     * \param message The string where to write the message
     *
     * \throw anch::network::IOException Network error while receiving message
     */
    virtual void receive(std::string& message);
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
