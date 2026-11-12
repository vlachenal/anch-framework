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
#include "network/tcpStream.hpp"
#include "network/tcpServer.hpp"

#include <utility>

using anch::network::Address;
using anch::network::Direction;
using anch::network::Socket;
using anch::network::SocketType;
using anch::network::TcpServer;
using anch::network::TcpStream;

// TcpStream +
TcpStream::TcpStream(const std::string& host, uint16_t port, std::size_t bufSize):
  anch::network::SocketHolder{Socket::connect(host, port, SocketType::TCP)},
  anch::cutils::CIOStream() {
  init(bufSize);
}

TcpStream::TcpStream(Socket&& socket, std::size_t bufSize):
  anch::network::SocketHolder{std::move(socket)},
  anch::cutils::CIOStream() {
  init(bufSize);
}

TcpStream::~TcpStream() noexcept {
  // Pending data are sent by CIOStream destructor, before SocketHolder destructor closes the socket
}

void
TcpStream::init(std::size_t bufSize) {
  setBuffer({
      .data = nullptr,
      .size = bufSize,
      .read = [this](char* data, std::size_t size) -> std::size_t { return _socket.receive(data, size); },
      .write = [this](const char* data, std::size_t size) -> std::size_t { return _socket.send(data, size); }
    });
}

void
TcpStream::shutdown(Direction how) {
  if(how != Direction::RECEPTION) {
    flush();
  }
  _socket.shutdown(how);
}

Socket&
TcpStream::getSocket() noexcept {
  return _socket;
}
// TcpStream -

// TcpServer +
TcpServer::TcpServer(const std::string& host, uint16_t port, int backlog):
  _socket(Socket::bind(host, port, SocketType::TCP)) {
  _socket.listen(backlog);
}

TcpStream
TcpServer::accept(std::size_t bufSize) {
  return TcpStream(_socket.accept(), bufSize); // guaranteed copy elision: TcpStream is not movable
}

Socket
TcpServer::acceptSocket(Address* peer) {
  return _socket.accept(peer);
}

Address
TcpServer::getAddress() const {
  return _socket.getLocalAddress();
}

Socket&
TcpServer::getSocket() noexcept {
  return _socket;
}
// TcpServer -
