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
#include "rest/auth.hpp"

#include "rest/headers.hpp"

using anch::rest::Filter;
using anch::rest::Next;
using anch::rest::Request;
using anch::rest::Response;

namespace {

  Filter
  authorization(std::string value) {
    return [value = std::move(value)](Request& request, const Next& next) -> Response {
      request.getHeaders().set(anch::rest::header::AUTHORIZATION, value);
      return next(request);
    };
  }

}

std::string
anch::rest::base64Encode(std::string_view value) {
  static constexpr std::string_view ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string res;
  res.reserve((value.size() + 2) / 3 * 4);
  std::size_t i = 0;
  for(; i + 2 < value.size() ; i += 3) {
    const uint32_t block = (static_cast<uint32_t>(static_cast<unsigned char>(value[i])) << 16)
      | (static_cast<uint32_t>(static_cast<unsigned char>(value[i + 1])) << 8)
      | static_cast<uint32_t>(static_cast<unsigned char>(value[i + 2]));
    res += ALPHABET[(block >> 18) & 0x3F];
    res += ALPHABET[(block >> 12) & 0x3F];
    res += ALPHABET[(block >> 6) & 0x3F];
    res += ALPHABET[block & 0x3F];
  }
  const std::size_t rest = value.size() - i;
  if(rest > 0) {
    uint32_t block = static_cast<uint32_t>(static_cast<unsigned char>(value[i])) << 16;
    if(rest == 2) {
      block |= static_cast<uint32_t>(static_cast<unsigned char>(value[i + 1])) << 8;
    }
    res += ALPHABET[(block >> 18) & 0x3F];
    res += ALPHABET[(block >> 12) & 0x3F];
    res += (rest == 2) ? ALPHABET[(block >> 6) & 0x3F] : '=';
    res += '=';
  }
  return res;
}

Filter
anch::rest::basicAuth(std::string_view user, std::string_view password) {
  std::string credentials(user);
  credentials += ':';
  credentials += password;
  return authorization("Basic " + anch::rest::base64Encode(credentials));
}

Filter
anch::rest::bearerAuth(std::string_view token) {
  return authorization("Bearer " + std::string(token));
}
