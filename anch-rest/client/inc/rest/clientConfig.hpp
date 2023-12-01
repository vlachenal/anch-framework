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

#include <string>
#include <optional>

#include "rest/auth.hpp"

namespace anch::rest {

  struct ProxyConfig {
    std::string host;
    uint16_t port;
    std::optional<anch::rest::Authentication> auth;
  };

  struct ClientConfg {
    uint16_t connectionTimeout;
    uint16_t socketTimeout;
    std::optional<anch::rest::ProxyConfig> proxy;
  };

}  // anch::rest
