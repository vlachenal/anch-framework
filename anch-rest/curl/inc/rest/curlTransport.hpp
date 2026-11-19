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

#include "anch-rest-curl-export.hpp"

#include <mutex>
#include <string>

#include "rest/transport.hpp"

namespace anch::rest {

  /*!
   * libcurl transport.\n
   * It keeps one curl handle (connections are reused between requests). Requests are serialized: a
   * transport can be shared between threads but sends one request at a time. Request bodies are
   * buffered before sending.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_REST_CURL_EXPORT CurlTransport: public anch::rest::Transport {

    // Attributes +
  private:
    /*! Configuration */
    anch::rest::TransportConfig _config;

    /*! curl easy handle (\c CURL* , curl.h is not exposed) */
    void* _curl;

    /*! Handle mutex */
    std::mutex _mutex;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref CurlTransport constructor
     *
     * \param config the configuration
     *
     * \throw anch::rest::TransportError when curl can not be initialized
     */
    explicit CurlTransport(anch::rest::TransportConfig config = anch::rest::TransportConfig());

    /*! Prohibit \ref CurlTransport copy */
    CurlTransport(const CurlTransport&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref CurlTransport destructor
     */
    virtual ~CurlTransport();
    // Destructor -

    // Methods +
  public:
    /*!
     * Send request with curl
     *
     * \param url the request URL
     * \param request the request
     *
     * \return the response
     *
     * \throw anch::rest::TransportError when request fails
     */
    virtual anch::rest::Response send(const std::string& url, const anch::rest::Request& request) override;

    /*! Prohibit \ref CurlTransport copy */
    CurlTransport& operator=(const CurlTransport&) = delete;
    // Methods -

  };

}
