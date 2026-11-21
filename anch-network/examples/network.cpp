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
//! [example]
#include "network/tcpServer.hpp"
#include "network/tcpStream.hpp"

#include <iostream>
#include <string>
#include <thread>

int
main() {
  anch::network::TcpServer server("127.0.0.1", 0);              // port 0: free port
  uint16_t port = server.getAddress().getPort();

  std::thread echo([&server]() {
    anch::network::TcpStream peer = server.accept();              // TcpStream is a std::iostream
    std::string line;
    while(std::getline(peer, line)) {
      peer << "echo: " << line << std::endl;
    }
  });

  anch::network::TcpStream client("127.0.0.1", port);
  client << "hello" << std::endl;
  std::string answer;
  std::getline(client, answer);
  std::cout << answer << std::endl;
  client.shutdown();                                              // end of transmission: server loop ends
  echo.join();
  return 0;
}
//! [example]
