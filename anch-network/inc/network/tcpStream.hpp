/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "anch-network-export.hpp"

#include <cstddef>
#include <string>

#include "cutils/iostream.hpp"
#include "network/socket.hpp"

namespace anch::network {

  /*!
   * \brief Socket owner base class of \ref TcpStream
   *
   * It is declared as the first base class of \ref TcpStream so that the socket is still opened when the stream
   * buffer sends pending data on destruction (base classes are destroyed in reverse order).
   *
   * \since 0.2
   */
  struct ANCH_NETWORK_EXPORT SocketHolder {
    /*! The socket */
    anch::network::Socket _socket;
  };

  /*!
   * \brief TCP connection as a \c std::iostream
   *
   * Written data are buffered: they are sent when the buffer is full, on \c flush (or \c std::endl ), before reading
   * (request / response protocols do not need an explicit flush) and on destruction.\n
   * End of stream (\c eof ) is reached when the peer has closed the connection (or shut down its transmission).
   * Network errors set \c badbit ; with \c exceptions(std::ios::badbit) , the \ref IOException is raised.\n
   * \ref TcpStream can not be copied nor moved: it is built from a host and a port (client) or from a connected
   * \ref Socket (\ref TcpServer::accept ).
   *
   * \code{.cpp}
   * anch::network::TcpServer server("", 40099);
   * anch::network::TcpStream client("localhost", 40099);
   * anch::network::TcpStream peer = server.accept();
   * client << "hello" << std::endl;
   * std::string line;
   * std::getline(peer, line);
   * \endcode
   *
   * \author Vincent Lachenal
   */
  class ANCH_NETWORK_EXPORT TcpStream: private anch::network::SocketHolder, public anch::cutils::CIOStream {

    // Attributes +
  public:
    /*! Default buffer size (for reading and for writing) */
    static constexpr std::size_t DEFAULT_BUFFER_SIZE = 8192;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref TcpStream constructor: connect to remote host
     *
     * \param host the remote host name or numeric address (empty for loopback)
     * \param port the remote port
     * \param bufSize the buffer size
     *
     * \throw anch::network::IOException connection error
     */
    TcpStream(const std::string& host, uint16_t port, std::size_t bufSize = DEFAULT_BUFFER_SIZE);

    /*!
     * \ref TcpStream constructor: use a connected socket
     *
     * \param socket the connected socket
     * \param bufSize the buffer size
     */
    explicit TcpStream(anch::network::Socket&& socket, std::size_t bufSize = DEFAULT_BUFFER_SIZE);

    /*!
     * Forbids \ref TcpStream copy constructor
     */
    TcpStream(const TcpStream&) = delete;

    /*!
     * Forbids \ref TcpStream move constructor (the stream buffer refers to the socket)
     */
    TcpStream(TcpStream&&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref TcpStream destructor: send pending data and close the connection
     */
    virtual ~TcpStream() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Shut down part of the connection. Pending data are sent before shutting down transmission: the peer reaches
     * the end of stream once it has read them, and this stream can still read the response.
     *
     * \param how the direction to shut down
     *
     * \throw anch::network::IOException error on \c shutdown
     */
    void shutdown(anch::network::Direction how = anch::network::Direction::TRANSMISSION);

    /*!
     * Socket getter (for options, addresses, ...)
     *
     * \return the socket
     */
    anch::network::Socket& getSocket() noexcept;
    // Methods -

  private:
    /*!
     * Initialize stream buffer
     *
     * \param bufSize the buffer size
     */
    void init(std::size_t bufSize);

  };

}  // anch::network
