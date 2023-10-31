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
#include "network/tcpStream.hpp"

#include <functional>

using anch::network::TcpStream;
using anch::network::TcpSocket;


TcpStream::TcpStream(const std::string& ipAddress, uint16_t port, std::size_t bufSize):
  anch::cutils::CIOStream(),
  _socket(ipAddress, port),
  _buffer({
	    .data = NULL,
	    .size = bufSize,
	    .read = std::bind_front(&TcpSocket::read, &_socket),
	    .write = std::bind_front(&TcpSocket::write, &_socket)
	  }) {
  setBuffer(_buffer);
}

TcpStream::~TcpStream() noexcept {
  // Nothing to do
}
