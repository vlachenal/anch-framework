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

#include "anch-rest-client-export.hpp"

#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>

#include "rest/request.hpp"
#include "rest/response.hpp"

namespace anch::rest {

  /*!
   * Transport configuration. Each transport applies the options it supports.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_REST_CLIENT_EXPORT TransportConfig {
    /*! Connection timeout */
    std::chrono::milliseconds connectTimeout{10000};

    /*! Whole request timeout (\c 0 for no timeout) */
    std::chrono::milliseconds timeout{30000};

    /*! Proxy URL (\c http://proxy:3128 ), environment configuration when empty */
    std::optional<std::string> proxy;

    /*! Follow redirections */
    bool followRedirects = false;

    /*! Verify TLS peer certificate and host name */
    bool verifyTls = true;

    /*! CA certificates file, system certificates when empty */
    std::optional<std::string> caFile;

    /*!
     * Maximum response body size in bytes (the body is read in memory), \c 0 for no limit. A larger response raises
     * \ref TransportError .
     *
     * \since 0.3
     */
    std::size_t maxResponseSize = 64 * 1024 * 1024;
  };

  /*!
   * Transport error: the request could not be sent or the response could not be received
   * (connection refused, timeout, TLS error, ...).
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_REST_CLIENT_EXPORT TransportError: public std::runtime_error {
  public:
    /*! Use \c std::runtime_error constructors */
    using std::runtime_error::runtime_error;
  };

  /*!
   * HTTP transport: sends requests with a native HTTP library.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_REST_CLIENT_EXPORT Transport {

    // Destructor +
  public:
    /*!
     * \ref Transport destructor
     */
    virtual ~Transport();
    // Destructor -

    // Methods +
  public:
    /*!
     * Send request. The response body is fully read.
     *
     * \param url the request URL (scheme, host, encoded path and query)
     * \param request the request (method, headers and body are used)
     *
     * \return the response, whatever its status
     *
     * \throw anch::rest::TransportError when request fails
     */
    virtual anch::rest::Response send(const std::string& url, const anch::rest::Request& request) = 0;
    // Methods -

  };

}
