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

#include <cstddef>
#include <string>

#include "network/socket.hpp"
#include "network/tcpStream.hpp"

namespace anch::network {

  /*!
   * \brief TCP server: accept connections as \ref TcpStream
   *
   * \code{.cpp}
   * anch::network::TcpServer server("", 40099);
   * while(running) {
   *   anch::network::TcpStream peer = server.accept();
   *   // handle peer (or move server.acceptSocket() to another thread and build the stream there)
   * }
   * \endcode
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class TcpServer {

    // Attributes +
  private:
    /*! The listening socket */
    anch::network::Socket _socket;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref TcpServer constructor: bind and listen
     *
     * \param host the local host name or numeric address (empty for any address)
     * \param port the local port (\c 0 for an ephemeral port: see \ref getAddress )
     * \param backlog the maximum number of pending connections
     *
     * \throw anch::network::IOException bind or listen error
     */
    TcpServer(const std::string& host, uint16_t port, int backlog = SOMAXCONN);
    // Constructors -

    // Methods +
  public:
    /*!
     * Wait for a connection
     *
     * \param bufSize the stream buffer size
     *
     * \return the connection stream
     *
     * \throw anch::network::IOException accept error
     */
    anch::network::TcpStream accept(std::size_t bufSize = anch::network::TcpStream::DEFAULT_BUFFER_SIZE);

    /*!
     * Wait for a connection (the socket can be moved to another thread, and a \ref TcpStream built from it)
     *
     * \param peer if not \c NULL , receives the peer address
     *
     * \return the connected socket
     *
     * \throw anch::network::IOException accept error
     */
    anch::network::Socket acceptSocket(anch::network::Address* peer = NULL);

    /*!
     * Local address getter (useful for ephemeral port)
     *
     * \return the address
     *
     * \throw anch::network::IOException error on \c getsockname
     */
    anch::network::Address getAddress() const;

    /*!
     * Listening socket getter
     *
     * \return the socket
     */
    anch::network::Socket& getSocket() noexcept;
    // Methods -

  };

}  // anch::network
