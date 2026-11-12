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
#include "rest/method.hpp"

#include <array>
#include <utility>

using anch::rest::Method;

namespace {

  constexpr std::array<std::pair<Method, std::string_view>, 9> METHODS = {{
      {Method::GET, "GET"},
      {Method::HEAD, "HEAD"},
      {Method::POST, "POST"},
      {Method::PUT, "PUT"},
      {Method::DELETE, "DELETE"},
      {Method::CONNECT, "CONNECT"},
      {Method::OPTIONS, "OPTIONS"},
      {Method::TRACE, "TRACE"},
      {Method::PATCH, "PATCH"}
    }};

}

std::string_view
anch::rest::toString(Method method) noexcept {
  for(const auto& [value, name] : METHODS) {
    if(value == method) {
      return name;
    }
  }
  return std::string_view();
}

std::optional<Method>
anch::rest::parseMethod(std::string_view name) noexcept {
  for(const auto& [value, label] : METHODS) {
    if(label == name) {
      return value;
    }
  }
  return std::nullopt;
}
