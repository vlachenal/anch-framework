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

#include "anch-rest-server-export.hpp"

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace anch::rest {

  /*!
   * Cross-Origin Resource Sharing (CORS) options, set with \ref Api::cors .\n
   * \n
   * Allowed methods are not configured: they are the methods of the routes matching the request
   * path (\c HEAD with \c GET , and \c OPTIONS ).
   * \code{.cpp}
api.cors({
  .origins = {"https://app.example.com"},
  .headers = {"Content-Type", "Authorization"},
  .exposedHeaders = {"Location"},
  .credentials = true,
  .maxAge = std::chrono::hours(1)
});
   * \endcode
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_REST_SERVER_EXPORT CorsOptions {

    /*!
     * Allowed origins (\c scheme://host[:port] ), compared exactly. \c "*" allows every origin.
     * It can not be empty.
     */
    std::vector<std::string> origins = {};

    /*!
     * Allowed request headers (case insensitive), checked on preflight requests. \c "*" allows
     * every requested header.
     */
    std::vector<std::string> headers = {};

    /*! Response headers exposed to the browser ( \c Access-Control-Expose-Headers ) */
    std::vector<std::string> exposedHeaders = {};

    /*!
     * Allow credentials (cookies, \c Authorization ): \c Access-Control-Allow-Credentials: true .
     * It can not be used with the \c "*" origin.
     */
    bool credentials = false;

    /*! Preflight response cache duration ( \c Access-Control-Max-Age ), not sent when empty */
    std::optional<std::chrono::seconds> maxAge = std::nullopt;

  };

}
