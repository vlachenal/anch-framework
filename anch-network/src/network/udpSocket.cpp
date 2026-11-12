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
#include "network/udpSocket.hpp"

#include <cerrno>

using anch::network::Address;
using anch::network::IOException;
using anch::network::Socket;
using anch::network::SocketType;
using anch::network::UdpSocket;

// Constructors +
UdpSocket::UdpSocket(const std::string& host, uint16_t port): _socket(Socket::bind(host, port, SocketType::UDP)) {
  // Nothing to do
}
// Constructors -

// Methods +
void
UdpSocket::sendTo(std::string_view data, const Address& to) {
  if(_socket.sendTo(data.data(), data.size(), to) != data.size()) {
    throw IOException("Datagram has not been entirely sent to " + to.toString(), EMSGSIZE);
  }
}

void
UdpSocket::sendTo(std::string_view data, const std::string& host, uint16_t port) {
  sendTo(data, Address::resolve(host, port, SocketType::UDP, false, getAddress().getFamily()).front());
}

Address
UdpSocket::receiveFrom(std::string& data, std::size_t maxSize) {
  Address from;
  data.resize(maxSize);
  try {
    data.resize(_socket.receiveFrom(data.data(), maxSize, from));
  } catch(...) {
    data.clear();
    throw;
  }
  return from;
}

Address
UdpSocket::getAddress() const {
  return _socket.getLocalAddress();
}

Socket&
UdpSocket::getSocket() noexcept {
  return _socket;
}
// Methods -
