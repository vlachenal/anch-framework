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

#include <chrono>
#include <cstddef>
#include <string>

#include <sys/socket.h>

#include "network/address.hpp"
#include "network/ioException.hpp"


namespace anch::network {

  /*!
   * Socket direction (for \ref Socket::shutdown )
   */
  enum class Direction {

    /*! Reception */
    RECEPTION = SHUT_RD,

    /*! Transmission */
    TRANSMISSION = SHUT_WR,

    /*! Reception and transmission */
    BOTH = SHUT_RDWR,

  };

  /*!
   * \brief Network socket (POSIX)
   *
   * A \ref Socket owns its file descriptor: it is closed by the destructor. Sockets can be moved, not copied.\n
   * Every method raises \ref IOException with the \c errno value on error. Interrupted system calls (\c EINTR ) are
   * restarted. \c SIGPIPE is not raised when peer has closed the connection: \ref send raises \ref IOException
   * (\c EPIPE ) instead.\n
   * For TCP connections, \ref TcpStream and \ref TcpServer provide a stream interface ; for UDP, \ref UdpSocket .
   *
   * \author Vincent Lachenal
   */
  class Socket {

    // Attributes +
  private:
    /*! File descriptor (\c -1 when closed) */
    int _fd;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Socket default constructor (closed socket)
     */
    Socket() noexcept;

    /*!
     * \ref Socket constructor: take ownership of an opened file descriptor
     *
     * \param fd the file descriptor
     */
    explicit Socket(int fd) noexcept;

    /*!
     * \ref Socket constructor: open a new socket
     *
     * \param family the address family (\c AF_INET or \c AF_INET6 )
     * \param type the socket type
     *
     * \throw anch::network::IOException socket can not be opened
     */
    Socket(int family, anch::network::SocketType type);

    /*!
     * Forbids \ref Socket copy constructor
     */
    Socket(const Socket&) = delete;

    /*!
     * \ref Socket move constructor
     *
     * \param other the socket to move (it is closed afterwards)
     */
    Socket(Socket&& other) noexcept;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Socket destructor: close the socket
     */
    virtual ~Socket() noexcept;
    // Destructor -

    // Operators +
  public:
    /*!
     * Forbids \ref Socket copy assignment
     */
    Socket& operator=(const Socket&) = delete;

    /*!
     * \ref Socket move assignment (the current socket is closed)
     *
     * \param other the socket to move (it is closed afterwards)
     *
     * \return \c this
     */
    Socket& operator=(Socket&& other) noexcept;
    // Operators -

    // Methods +
  public:
    /*!
     * Open a socket connected to remote host. Every resolved address is tried (IPv4 and IPv6).
     *
     * \param host the remote host name or numeric address (empty for loopback)
     * \param port the remote port
     * \param type the socket type
     *
     * \return the connected socket
     *
     * \throw anch::network::IOException resolution or connection error (error of the last address)
     */
    static Socket connect(const std::string& host, uint16_t port, anch::network::SocketType type);

    /*!
     * Open a socket bound to a local address, with \c SO_REUSEADDR . Every resolved address is tried. An IPv6 socket
     * bound to any address also accepts IPv4 connections.
     *
     * \param host the local host name or numeric address (empty for any address)
     * \param port the local port (\c 0 for an ephemeral port: see \ref getLocalAddress )
     * \param type the socket type
     *
     * \return the bound socket
     *
     * \throw anch::network::IOException resolution or bind error (error of the last address)
     */
    static Socket bind(const std::string& host, uint16_t port, anch::network::SocketType type);

    /*!
     * Listen for connections on a bound socket
     *
     * \param backlog the maximum number of pending connections
     *
     * \throw anch::network::IOException error on \c listen
     */
    void listen(int backlog = SOMAXCONN);

    /*!
     * Wait for a connection on a listening socket
     *
     * \param peer if not \c NULL , receives the peer address
     *
     * \return the connected socket
     *
     * \throw anch::network::IOException error on \c accept
     */
    Socket accept(anch::network::Address* peer = NULL);

    /*!
     * Send data (one system call: less bytes than \c size can be sent)
     *
     * \param data the data
     * \param size the data size
     *
     * \return the number of bytes sent
     *
     * \throw anch::network::IOException error on \c send
     */
    std::size_t send(const char* data, std::size_t size);

    /*!
     * Send every data (loop on partial sends)
     *
     * \param data the data
     * \param size the data size
     *
     * \throw anch::network::IOException error on \c send
     */
    void sendAll(const char* data, std::size_t size);

    /*!
     * Receive data
     *
     * \param data the buffer
     * \param size the buffer size
     *
     * \return the number of bytes received, \c 0 when peer has closed the connection
     *
     * \throw anch::network::IOException error on \c recv (\c EAGAIN on timeout)
     */
    std::size_t receive(char* data, std::size_t size);

    /*!
     * Send a datagram
     *
     * \param data the data
     * \param size the data size
     * \param to the destination address
     *
     * \return the number of bytes sent
     *
     * \throw anch::network::IOException error on \c sendto
     */
    std::size_t sendTo(const char* data, std::size_t size, const anch::network::Address& to);

    /*!
     * Receive a datagram
     *
     * \param data the buffer
     * \param size the buffer size
     * \param from receives the sender address
     *
     * \return the datagram size
     *
     * \throw anch::network::IOException error on \c recvmsg , or datagram larger than buffer (\c EMSGSIZE )
     */
    std::size_t receiveFrom(char* data, std::size_t size, anch::network::Address& from);

    /*!
     * Shut down part of a full-duplex connection
     *
     * \param how the direction to shut down
     *
     * \throw anch::network::IOException error on \c shutdown
     */
    void shutdown(anch::network::Direction how = anch::network::Direction::BOTH);

    /*!
     * Close the socket (do nothing when already closed)
     */
    void close() noexcept;

    /*!
     * Set receive timeout (\ref receive and \ref receiveFrom raise \ref IOException with \c EAGAIN )
     *
     * \param timeout the timeout (\c 0 for no timeout)
     *
     * \throw anch::network::IOException error on \c setsockopt
     */
    void setReceiveTimeout(std::chrono::milliseconds timeout);

    /*!
     * Set send timeout
     *
     * \param timeout the timeout (\c 0 for no timeout)
     *
     * \throw anch::network::IOException error on \c setsockopt
     */
    void setSendTimeout(std::chrono::milliseconds timeout);
    // Methods -

    // Accessors +
  public:
    /*!
     * Local address getter
     *
     * \return the address the socket is bound to
     *
     * \throw anch::network::IOException error on \c getsockname
     */
    anch::network::Address getLocalAddress() const;

    /*!
     * Peer address getter
     *
     * \return the address of the connected peer
     *
     * \throw anch::network::IOException error on \c getpeername
     */
    anch::network::Address getPeerAddress() const;

    /*!
     * File descriptor getter
     *
     * \return the file descriptor (\c -1 when closed)
     */
    int getDescriptor() const noexcept;

    /*!
     * Check if socket is opened
     *
     * \return \c true when socket is opened, \c false otherwise
     */
    bool isOpen() const noexcept;
    // Accessors -

  };

}  // anch::network
