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
#pragma once

#include <string>
#include <string_view>

#include "rest/filter.hpp"

namespace anch::rest {

  /*!
   * HTTP Basic authentication filter: sets \c Authorization header
   *
   * \param user the user
   * \param password the password
   *
   * \return the filter
   *
   * \since 0.2
   */
  anch::rest::Filter basicAuth(std::string_view user, std::string_view password);

  /*!
   * Bearer token authentication filter: sets \c Authorization header
   *
   * \param token the token
   *
   * \return the filter
   *
   * \since 0.2
   */
  anch::rest::Filter bearerAuth(std::string_view token);

  /*!
   * Base64 encoding (RFC 4648, with padding)
   *
   * \param value the value to encode
   *
   * \return the encoded value
   *
   * \since 0.2
   */
  std::string base64Encode(std::string_view value);

}
