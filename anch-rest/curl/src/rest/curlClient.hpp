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
#include "rest/curlClient.hpp"

using anch::rest::CURLClient;


CURLClient::CURLClient(): _curl(NULL) {
}

CURLClient::~CURLClient() {
  if(_curl != NULL) {
    curl_easy_cleanup(_curl);
  }
}

anch::rest::Reponse CURLClient::GET(const anch::rest::Request& request) {}

anch::rest::Reponse CURLClient::GET(const std::string& path) {}

anch::rest::Reponse CURLClient::POST(const anch::rest::Request& request) {}

anch::rest::Reponse CURLClient::PUT(const anch::rest::Request& request) {}

anch::rest::Reponse CURLClient::PATCH(const anch::rest::Request& request) {}

anch::rest::Reponse CURLClient::DELETE(const anch::rest::Request& request) {}

anch::rest::Reponse CURLClient::DELETE(const std::string& path) {}

anch::rest::Reponse CURLClient::HEAD(const anch::rest::Request& request) {}

anch::rest::Reponse CURLClient::HEAD(const std::string& path) {}

anch::rest::Reponse CURLClient::OPTIONS(const anch::rest::Request& request) {}

anch::rest::Reponse CURLClient::OPTIONS(const std::string& path) {}

anch::rest::Reponse CURLClient::CONNECT(const anch::rest::Request& request) {}

anch::rest::Reponse CURLClient::CONNECT(const std::string& path) {}

anch::rest::Reponse CURLClient::TRACE(const std::string& path) {}
