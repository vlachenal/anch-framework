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
#include "network/socket.hpp"

//#ifdef WIN32
//#include <winsock32.h>
//typedef int socklen_t;
//
//#elifdef POSIX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) ::close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

//#endif


using std::string;

using anch::events::Observable;
using anch::network::Socket;
using anch::network::SocketEvent;
using anch::network::SocketType;


// Constructors +
/**
 * {@link Socket} constructor.
 * Backlog is set to 5 by default. You can change it using the setter before call listen method.
 *
 * @param ipAddress IP address
 * @param port Destination port
 * @param type The socket type
 */
Socket::Socket(const string& ipAddress, uint16_t port, SocketType type):
  Observable<SocketEvent>(),
  _ipAddress(ipAddress),
  _port(port),
  _type(type),
  _csock(INVALID_SOCKET),
  _backlog(5) {
  switch(type) {
  case SocketType::TCP:
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    break;
  case SocketType::UDP:
    _sock = socket(AF_INET, SOCK_DGRAM, 0);
    break;
#ifdef POSIX
  case SocketType::POSIX:
    _sock = socket(AF_INET, SOCK_DGRAM, 0); // TODO change parameters
    break;
#endif
  default:
    // Nothing to do
    break;
  }
}
// Constructors -

// Destructor +
/**
 * {@link Socket} destructor
 */
Socket::~Socket() {
  // Nothing to do
}
// Destructor -


// Methods +
/**
 * Bind socket
 */
void
Socket::bind() {
  // Compute socket address +
  SOCKADDR_IN sin;
  if(_ipAddress == "") {
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    sin.sin_addr.s_addr = inet_addr(_ipAddress.c_str());
  }
  sin.sin_family = getDomain();
  sin.sin_port = htons(_port);
  // Compute socket address -

  ::bind(_sock, (SOCKADDR*)&sin, sizeof(sin));
}

/**
 * Connect to remote socket
 */
void
Socket::connect() {
  SOCKADDR_IN sin;
  ::connect(_sock, (SOCKADDR*)&sin, sizeof(sin));
}

/**
 * Listen on socket
 */
void
Socket::listen() {
    ::listen(_sock, _backlog);
}

/**
 * Accept client connection
 */
void
Socket::accept() {
  SOCKADDR_IN csin;
  socklen_t length = sizeof(csin);
  _csock = ::accept(_sock, (SOCKADDR*)&csin, &length);
}

/**
 * Send a message on socket
 */
void
Socket::send(const string& message) {

}

/**
 * Receive a message on socket
 *
 * @param message The string where to write the message
 */
void
Socket::receive(std::string message) {

}

/**
 * Close the socket
 */
void
Socket::close() {
  closesocket(_sock);
  if(_csock != INVALID_SOCKET) {
    closesocket(_csock);
  }
}
// Methods -
