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

#include <cstdint>
#include <string>
#include <vector>

#include <sys/socket.h>

namespace anch::network {

  /*!
   * Socket type
   */
  enum class SocketType {

    /*! TCP socket (\c SOCK_STREAM ) */
    TCP = SOCK_STREAM,

    /*! UDP socket (\c SOCK_DGRAM ) */
    UDP = SOCK_DGRAM,

  };

  /*!
   * \brief Socket address (IPv4 or IPv6 address and port)
   *
   * The address is copied in a \c sockaddr_storage : it does not depend on \c getaddrinfo result lifetime.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class Address {

    friend class Socket;

    // Attributes +
  private:
    /*! Address storage */
    sockaddr_storage _storage;

    /*! Address length (\c 0 when address is not set) */
    socklen_t _length;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Address default constructor (address is not set)
     */
    Address() noexcept;

    /*!
     * \ref Address constructor
     *
     * \param address the address to copy
     * \param length the address length
     *
     * \throw std::invalid_argument length is greater than \c sockaddr_storage
     */
    Address(const sockaddr* address, socklen_t length);
    // Constructors -

    // Methods +
  public:
    /*!
     * Resolve host name (with \c getaddrinfo )
     *
     * \param host the host name or numeric address ; empty for loopback address (or any address when \c passive )
     * \param port the port
     * \param type the socket type
     * \param passive \c true for addresses to bind, \c false for addresses to connect to
     * \param family the address family (\c AF_UNSPEC , \c AF_INET or \c AF_INET6 ; IPv4 addresses are mapped for
     *        \c AF_INET6 )
     *
     * \return the addresses (at least one)
     *
     * \throw anch::network::IOException resolution error
     */
    static std::vector<Address> resolve(const std::string& host,
					uint16_t port,
					SocketType type,
					bool passive = false,
					int family = AF_UNSPEC);

    /*!
     * Address family getter
     *
     * \return \c AF_INET , \c AF_INET6 (or \c AF_UNSPEC when address is not set)
     */
    int getFamily() const noexcept;

    /*!
     * Numeric host getter
     *
     * \return the numeric host (\c 127.0.0.1 or \c ::1 for example)
     */
    std::string getHost() const;

    /*!
     * Port getter
     *
     * \return the port
     */
    uint16_t getPort() const noexcept;

    /*!
     * String representation: \c host:port (\c [host]:port for IPv6)
     *
     * \return the string representation
     */
    std::string toString() const;

    /*!
     * Raw address getter
     *
     * \return the address
     */
    const sockaddr* data() const noexcept;

    /*!
     * Raw address length getter
     *
     * \return the length
     */
    socklen_t size() const noexcept;
    // Methods -

    // Operators +
  public:
    /*!
     * Check if addresses are equals (same bytes)
     *
     * \param other the address to compare to
     *
     * \return \c true when addresses are equals, \c false otherwise
     */
    bool operator==(const Address& other) const noexcept;
    // Operators -

  };

}  // anch::network
