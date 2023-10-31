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

#include "network/tcpSocket.hpp"
#include "cutils/iostream.hpp"

namespace anch::network {

  /*!
   * TCP stream
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class TcpStream: public anch::cutils::CIOStream {

    // Attributes +
  private:
    /*! The TCP socket */
    anch::network::TcpSocket _socket;

    /*! I/O buffer */
    anch::cutils::cbuffer _buffer;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref TcpStream constructor
     *
     * \param ipAddress the IP address
     * \param port the port number
     * \param bufSize the buffer size in byte (default = 1500)
     *
     * \throw anch::network::IOException Error while creating the socket
     */
    TcpStream(const std::string& ipAddress, uint16_t port, std::size_t bufSize = 1500);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref TcpStream destructor
     */
    virtual ~TcpStream() noexcept;
    // Destructor -

  };

}  // anch::network
