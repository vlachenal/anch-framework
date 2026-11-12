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
#pragma once

namespace anch::rest::nginx {

  template<anch::rest::nginx::Factory F>
  const anch_rest_application_t*
  application() noexcept {
    // One table per factory: applications loaded by the same nginx do not share state
    static const anch_rest_application_t operations = {
      ANCH_REST_NGINX_ABI_VERSION,
      [](const char* config, char* error, std::size_t errorSize) -> void* {
	return anch::rest::nginx::create(F, config, error, errorSize);
      },
      &anch::rest::nginx::destroy,
      &anch::rest::nginx::handle,
      &anch::rest::nginx::release
    };
    return &operations;
  }

}
