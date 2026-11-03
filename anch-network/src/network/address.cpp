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
#include "network/address.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

#include <netdb.h>
#include <netinet/in.h>

#include "network/ioException.hpp"

using anch::network::Address;
using anch::network::IOException;
using anch::network::SocketType;

// Constructors +
Address::Address() noexcept: _storage(), _length(0) {
  // Nothing to do
}

Address::Address(const sockaddr* address, socklen_t length): _storage(), _length(length) {
  if(length > sizeof(_storage)) {
    throw std::invalid_argument("Address length is greater than sockaddr_storage");
  }
  std::memcpy(&_storage, address, length);
}
// Constructors -

// Methods +
std::vector<Address>
Address::resolve(const std::string& host, uint16_t port, SocketType type, bool passive, int family) {
  addrinfo hints{};
  hints.ai_family = family;
  hints.ai_socktype = static_cast<int>(type);
  hints.ai_flags = AI_NUMERICSERV | (passive ? AI_PASSIVE : 0) | (family == AF_INET6 ? AI_V4MAPPED : 0);
  addrinfo* result = NULL;
  std::string service = std::to_string(port);
  int res = ::getaddrinfo(host.empty() ? NULL : host.data(), service.data(), &hints, &result);
  if(res != 0) {
    if(res == EAI_SYSTEM) {
      throw IOException("Error on getaddrinfo() for " + host);
    }
    throw IOException("Error on getaddrinfo() for " + host, std::error_code(res, anch::network::addrinfoCategory()));
  }
  std::vector<Address> addresses;
  for(addrinfo* iter = result ; iter != NULL ; iter = iter->ai_next) {
    addresses.emplace_back(iter->ai_addr, iter->ai_addrlen);
  }
  ::freeaddrinfo(result); // addresses have been copied
  return addresses;
}

int
Address::getFamily() const noexcept {
  return _length == 0 ? AF_UNSPEC : _storage.ss_family;
}

std::string
Address::getHost() const {
  char host[NI_MAXHOST];
  int res = ::getnameinfo(data(), _length, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
  if(res != 0) {
    throw IOException("Error on getnameinfo()", std::error_code(res, anch::network::addrinfoCategory()));
  }
  return host;
}

uint16_t
Address::getPort() const noexcept {
  switch(getFamily()) {
  case AF_INET:
    return ntohs(reinterpret_cast<const sockaddr_in*>(&_storage)->sin_port);
  case AF_INET6:
    return ntohs(reinterpret_cast<const sockaddr_in6*>(&_storage)->sin6_port);
  default:
    return 0;
  }
}

std::string
Address::toString() const {
  if(getFamily() == AF_INET6) {
    return "[" + getHost() + "]:" + std::to_string(getPort());
  }
  return getHost() + ":" + std::to_string(getPort());
}

const sockaddr*
Address::data() const noexcept {
  return reinterpret_cast<const sockaddr*>(&_storage);
}

socklen_t
Address::size() const noexcept {
  return _length;
}
// Methods -

// Operators +
bool
Address::operator==(const Address& other) const noexcept {
  return _length == other._length && std::memcmp(&_storage, &other._storage, _length) == 0;
}
// Operators -
