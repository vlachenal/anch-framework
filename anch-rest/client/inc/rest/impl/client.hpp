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


namespace anch::rest {

  template<typename T, typename... Q>
  std::string
  computePath(const std::string& path, const T param, const Q...) {
    return computePath<Q...>(path);
  }

  template<typename T>
  std::string
  computePath(const std::string& path, const T param) {
    return path;
  }

  std::string
  computePath(const std::string& path) {
    return path;
  }

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::GET(anch::rest::Request& request, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::GET(const std::string& path, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::POST(anch::rest::Request& request, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::PUT(anch::rest::Request& request, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::PATCH(anch::rest::Request& request, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::DELETE(anch::rest::Request& request, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::DELETE(const std::string& path, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::HEAD(anch::rest::Request& request, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::HEAD(const std::string& path, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::OPTIONS(anch::rest::Request& request, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::OPTIONS(const std::string& path, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::CONNECT(anch::rest::Request& request, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::CONNECT(const std::string& path, P... params);

  template<typename... P>
  anch::rest::Response
  anch::rest::WebClient::TRACE(const std::string& path, P... params);

}
