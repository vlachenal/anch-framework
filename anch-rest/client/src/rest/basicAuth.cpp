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
#include "rest/basicAuth.hpp"

#include <sstream>

#include "crypto/b2t/base64.hpp"

using anch::rest::BasicAuthentication;

BasicAuthentication::BasicAuthentication(const std::string& login, const std::string& password) {
  std::istringstream iss(login + ":" + password);
  std::ostringstream oss;
  anch::crypto::Base64::encode(iss, oss);
  _auth = oss.str();
}

BasicAuthentication::~BasicAuthentication() {
  // Nothing to do
}

void
BasicAuthentication::setAuthentication(anch::rest::Headers& headers, const std::string& header) {
  headers.put(header, _auth);
}
