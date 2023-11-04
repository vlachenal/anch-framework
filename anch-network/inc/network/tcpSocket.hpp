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

  class TcpStream;

  /*!
   * TCP socket implementation
   *
   * \author Vincent Lachenal
   */
  class TcpSocket: public anch::network::Socket {

    friend anch::network::TcpStream;

  public:
    // Constructors +
    /*!
     * \ref TcpSocket default constructor
     */
    TcpSocket();

    /*!
     * \ref TcpSocket constructor
     *
     * \param ipAddress the IP address
     * \param port the port number
     *
     * \throw anch::network::IOException Error while creating the socket
     */
    TcpSocket(const std::string& ipAddress, uint16_t port);
    // Constructors -

    // Destructor +
    /*!
     * \ref TcpSocket destructor
     */
    virtual ~TcpSocket() noexcept;
    // Destructor -

  public:
    // Methods +
    /*!
     * Send a message on socket
     *
     * \param message the message to send
     *
     * \throw anch::network::IOException Network error while sending message
     */
    virtual void send(const std::string& message);

    /*!
     * Receive a message on socket
     *
     * \param message the string where to write the message
     *
     * \throw anch::network::IOException Network error while receiving message
     */
    virtual void receive(std::string& message);

  private:
    /*!
     * Write message in buffer
     *
     * \param buffer the buffer
     * \param size the number of bytes to write
     *
     * \return the number of wroten bytes
     */
    std::size_t write(const char* buffer, std::size_t size);

    /*!
     * Read message from buffer
     *
     * \param buffer the buffer
     * \param size the number of bytes to read
     *
     * \return the number of read bytes
     */
    std::size_t read(char* buffer, std::size_t size);
    // Methods -

    // Accessors +
    /*!
     * Get the socket domain
     *
     * \return the POSIX socket domain
     */
    virtual int getDomain() const;

    /*!
     * Get the socket service type
     *
     * \return the POSIX socket service type
     */
    virtual int getType() const;
    // Accessors -

  };

}
