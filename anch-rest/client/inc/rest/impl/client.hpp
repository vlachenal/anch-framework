/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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
#pragma once

#include <iostream>


namespace anch::rest {

  // template<typename... P>
  // std::string
  // computePath(const std::string& path, P... params);

  // template<typename T, typename... Q>
  // std::string
  // computePath(const std::string& path, T param, Q... params);

  template<typename T, typename... Q>
  std::string
  computePath(const std::string& path, T param, Q... params) {
    // \todo bind param
    std::cout << path << " ; " << param << std::endl;
    return anch::rest::computePath<Q...>(path, params...);
  }

  template<typename T>
  std::string
  computePath(const std::string& path, T param) {
    std::cout << param << std::endl;
    return path;
  }

  // std::string
  // computePath(const std::string& path) {
  //   return path;
  // }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::GET(anch::rest::Request& request, P... params) {
    if constexpr (sizeof...(P) != 0) {
      std::string url = anch::rest::computePath<P...>(request.getPath(), params...);
      request.setPath(url);
    }
    return _client.GET(request);
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::GET(const std::string& path, P... params) {
    std::string url = path;
    if constexpr (sizeof...(P) != 0) {
      url = anch::rest::computePath<P...>(path, params...);
    }
    return _client.GET(url);
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::POST(anch::rest::Request& request, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::PUT(anch::rest::Request& request, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::PATCH(anch::rest::Request& request, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::DELETE(anch::rest::Request& request, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::DELETE(const std::string& path, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::HEAD(anch::rest::Request& request, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::HEAD(const std::string& path, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::OPTIONS(anch::rest::Request& request, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::OPTIONS(const std::string& path, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::CONNECT(anch::rest::Request& request, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::CONNECT(const std::string& path, P... params) {
    return anch::rest::Response();
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::TRACE(const std::string& path, P... params) {
    return anch::rest::Response();
  }

}
