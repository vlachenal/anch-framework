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

#include "anch-rest-nginx-export.hpp"

#include <memory>
#include <string_view>

#include "rest/anch_rest_nginx.h"
#include "rest/api.hpp"

namespace anch::rest::nginx {

  /*!
   * Application factory: creates the API of an nginx application (once in each worker process).
   * The configuration is the second \c anch_rest_app directive argument (empty when not set).
   */
  using Factory = std::unique_ptr<anch::rest::Api>(*)(std::string_view config);

  /*!
   * Create application: call factory and validate API
   *
   * \param factory the factory
   * \param config the configuration
   * \param error the error buffer
   * \param errorSize the error buffer size
   *
   * \return the API, \c nullptr on error (message in \c error )
   */
  ANCH_REST_NGINX_EXPORT void* create(Factory factory, const char* config, char* error, std::size_t errorSize) noexcept;

  /*!
   * Destroy application
   *
   * \param app the API
   */
  ANCH_REST_NGINX_EXPORT void destroy(void* app) noexcept;

  /*!
   * Handle request with API (see \ref anch::rest::Api::handle ). Unknown methods are answered with
   * \c 501 , errors with \c 500 .\n
   * \n
   * The request body is read in stream (\c anch::cutils::CIStream on the module reader): it is read while
   * nginx receives it. The response body is written in stream (\c anch::cutils::COStream on the module writer,
   * blocks of \c writer->buffer_size bytes): the response is started when the first block is full, with
   * chunked transfer encoding, or at the end of the body with its length when it fits in one block. An error
   * raised before the start is answered with \c 500 , an error raised after interrupts the response.\n
   * The request outlives the response body writing: a body writer can read the request body (echo, ...).
   *
   * \param app the API
   * \param request the request
   * \param writer the response writer
   *
   * \return \c 0 when response is complete, \c -1 when it has been interrupted after its start
   */
  ANCH_REST_NGINX_EXPORT int handle(void* app, const anch_rest_request_t* request, const anch_rest_response_writer_t* writer) noexcept;

  /*!
   * Application operations for a factory
   *
   * \tparam F the factory
   *
   * \return the operations
   */
  template<anch::rest::nginx::Factory F>
  const anch_rest_application_t* application() noexcept;

}

/*!
 * Declare nginx application: exports \c anch_rest_application from the application shared library.
 * \snippet{trimleft} anch-rest/examples/nginxApp.cpp nginx application
 *
 * \param factory the factory function (see \ref anch::rest::nginx::Factory )
 */
#define ANCH_REST_APPLICATION(factory)					\
  extern "C" __attribute__((visibility("default"))) const anch_rest_application_t* \
  anch_rest_application(void) {						\
    return anch::rest::nginx::application<&factory>();			\
  }

#include "rest/impl/nginxApplication.hpp"
