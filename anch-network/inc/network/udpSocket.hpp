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
#include <string_view>

#include "network/socket.hpp"

namespace anch::network {

  /*!
   * \brief UDP socket
   *
   * Datagrams keep their boundaries and can contain any byte (\c '\\0' included). A datagram which does not fit in
   * the reception buffer raises \ref IOException (\c EMSGSIZE ) instead of being silently truncated.
   *
   * \author Vincent Lachenal
   */
  class ANCH_NETWORK_EXPORT UdpSocket {

    // Attributes +
  public:
    /*! Maximum UDP payload (IPv4) */
    static constexpr std::size_t MAX_DATAGRAM_SIZE = 65507;

  private:
    /*! The socket */
    anch::network::Socket _socket;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref UdpSocket constructor: bind to local address
     *
     * \param host the local host name or numeric address (empty for any address)
     * \param port the local port (\c 0 for an ephemeral port: see \ref getAddress )
     *
     * \throw anch::network::IOException bind error
     */
    explicit UdpSocket(const std::string& host = "", uint16_t port = 0);
    // Constructors -

    // Methods +
  public:
    /*!
     * Send a datagram
     *
     * \param data the datagram
     * \param to the destination
     *
     * \throw anch::network::IOException send error (or datagram not entirely sent)
     */
    void sendTo(std::string_view data, const anch::network::Address& to);

    /*!
     * Send a datagram (the destination is resolved for the socket address family)
     *
     * \param data the datagram
     * \param host the destination host name or numeric address
     * \param port the destination port
     *
     * \throw anch::network::IOException resolution or send error
     */
    void sendTo(std::string_view data, const std::string& host, uint16_t port);

    /*!
     * Receive a datagram
     *
     * \param data receives the datagram
     * \param maxSize the maximum datagram size
     *
     * \return the sender address
     *
     * \throw anch::network::IOException reception error, or datagram larger than \c maxSize (\c EMSGSIZE )
     */
    anch::network::Address receiveFrom(std::string& data, std::size_t maxSize = MAX_DATAGRAM_SIZE);

    /*!
     * Local address getter (useful for ephemeral port)
     *
     * \return the address
     *
     * \throw anch::network::IOException error on \c getsockname
     */
    anch::network::Address getAddress() const;

    /*!
     * Socket getter (for options, timeouts, ...)
     *
     * \return the socket
     */
    anch::network::Socket& getSocket() noexcept;
    // Methods -

  };

}  // anch::network
