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
#include <iostream>
#include <string>

#include "network/tcpServer.hpp"
#include "network/tcpStream.hpp"

void
serverLoop(bool running) {
  //! [server]
  anch::network::TcpServer server("", 40099);
  while(running) {
    anch::network::TcpStream peer = server.accept();
    // handle peer (or move server.acceptSocket() to another thread and build the stream there)
  }
  //! [server]
}

bool
stream() {
  //! [stream]
  anch::network::TcpServer server("", 40099);
  anch::network::TcpStream client("localhost", 40099);
  anch::network::TcpStream peer = server.accept();
  client << "hello" << std::endl;
  std::string line;
  std::getline(peer, line);
  //! [stream]
  return line == "hello";
}

int
main() {
  serverLoop(false); // the loop of the example never ends
  return stream() ? 0 : 1;
}
