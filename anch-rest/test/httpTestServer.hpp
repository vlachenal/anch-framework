/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include <atomic>
#include <istream>
#include <sstream>
#include <string>
#include <thread>

#include "network/tcpServer.hpp"
#include "network/tcpStream.hpp"
#include "rest/api.hpp"

/*!
 * Minimal HTTP/1.1 server for client tests: one connection at a time, \c Content-Length bodies only,
 * connection closed after each response. Requests are handled by an \c anch::rest::Api .
 */
class HttpTestServer {

  // Attributes +
private:
  /*! The API */
  anch::rest::Api& _api;

  /*! The server socket */
  anch::network::TcpServer _server;

  /*! The server port */
  uint16_t _port;

  /*! Running flag */
  std::atomic<bool> _running;

  /*! Server thread */
  std::thread _thread;
  // Attributes -

  // Constructors +
public:
  explicit HttpTestServer(anch::rest::Api& api):
    _api(api),
    _server("127.0.0.1", 0),
    _port(_server.getAddress().getPort()),
    _running(true),
    _thread([this]() { loop(); }) {
  }
  // Constructors -

  // Destructor +
public:
  ~HttpTestServer() {
    _running = false;
    try {
      anch::network::TcpStream wakeUp("127.0.0.1", _port); // unblock accept
    } catch(...) {
      // Server is already stopped
    }
    _thread.join();
  }
  // Destructor -

  // Methods +
public:
  std::string
  url() const {
    return "http://127.0.0.1:" + std::to_string(_port);
  }

private:
  static std::string
  trim(const std::string& value) {
    const std::size_t first = value.find_first_not_of(" \t\r");
    if(first == std::string::npos) {
      return std::string();
    }
    return value.substr(first, value.find_last_not_of(" \t\r") - first + 1);
  }

  void
  loop() {
    while(_running) {
      try {
	anch::network::TcpStream peer = _server.accept();
	if(!_running) {
	  break;
	}
	serve(peer);
      } catch(...) {
	// Client has gone (timeout tests): wait for next connection
      }
    }
  }

  void
  serve(anch::network::TcpStream& peer) {
    std::string line;
    if(!std::getline(peer, line)) {
      return;
    }
    std::istringstream requestLine(trim(line));
    std::string methodName;
    std::string target;
    requestLine >> methodName >> target;
    std::optional<anch::rest::Method> method = anch::rest::parseMethod(methodName);
    anch::rest::Request request(method.value_or(anch::rest::Method::GET), target);
    while(std::getline(peer, line)) {
      line = trim(line);
      if(line.empty()) {
	break;
      }
      const std::size_t colon = line.find(':');
      if(colon != std::string::npos) {
	request.getHeaders().add(line.substr(0, colon), trim(line.substr(colon + 1)));
      }
    }
    std::optional<std::string_view> length = request.getHeaders().get(anch::rest::header::CONTENT_LENGTH);
    if(length) {
      std::string body(std::stoul(std::string(*length)), '\0');
      peer.read(body.data(), static_cast<std::streamsize>(body.size()));
      request.setBody(std::move(body));
    }
    anch::rest::Response response = method ? _api.handle(request) : anch::rest::Response(anch::rest::Status::NOT_IMPLEMENTED);
    const std::string content = response.content();
    std::ostringstream out;
    out << "HTTP/1.1 " << anch::rest::code(response.getStatus()) << ' ' << anch::rest::reasonPhrase(response.getStatus()) << "\r\n";
    for(const auto& [name, values] : response.getHeaders()) {
      for(const std::string& value : values) {
	out << name << ": " << value << "\r\n";
      }
    }
    out << "Content-Length: " << content.size() << "\r\n" << "Connection: close\r\n\r\n" << content;
    peer << out.str();
    peer.flush();
  }
  // Methods -

};
