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
#ifndef ANCH_REST_NGINX_H
#define ANCH_REST_NGINX_H

/*!
 * \file anch_rest_nginx.h
 *
 * AnCH REST nginx module C ABI.\n
 * An application is a shared library which exports \c anch_rest_application (see
 * \c ANCH_REST_APPLICATION in \c rest/nginxApplication.hpp ). The nginx module loads it with
 * \c dlopen , creates one application per worker process, and calls \c handle from nginx thread
 * pool threads.
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! ABI version: the module refuses applications with another version */
#define ANCH_REST_NGINX_ABI_VERSION 1u

/*! Application entry point symbol */
#define ANCH_REST_APPLICATION_SYMBOL "anch_rest_application"

/*! String (not null terminated) */
typedef struct {
  /*! Data */
  const char* data;
  /*! Length */
  size_t len;
} anch_rest_str_t;

/*! Header */
typedef struct {
  /*! Name */
  anch_rest_str_t name;
  /*! Value */
  anch_rest_str_t value;
} anch_rest_header_t;

/*! Request: memory is owned by the module and valid during \c handle call */
typedef struct {
  /*! Method (\c GET , ...) */
  anch_rest_str_t method;
  /*! Request target: encoded path and query (\c /pets?limit=10 ) */
  anch_rest_str_t target;
  /*! Headers */
  const anch_rest_header_t* headers;
  /*! Number of headers */
  size_t headers_count;
  /*! Request has a body (possibly empty) */
  int has_body;
  /*! Body */
  anch_rest_str_t body;
} anch_rest_request_t;

/*! Response: memory is owned by the application until \c release */
typedef struct {
  /*! Status code */
  unsigned int status;
  /*! Headers */
  const anch_rest_header_t* headers;
  /*! Number of headers */
  size_t headers_count;
  /*! Body */
  anch_rest_str_t body;
  /*! Application data, released by \c release */
  void* internal;
} anch_rest_response_t;

/*! Application operations */
typedef struct {
  /*! ABI version (\ref ANCH_REST_NGINX_ABI_VERSION ) */
  unsigned int abi_version;

  /*!
   * Create application (called once in each worker process)
   *
   * \param config the configuration string (second \c anch_rest_app argument, empty when not set)
   * \param error the buffer for error message
   * \param error_size the error buffer size
   *
   * \return the application, \c NULL on error
   */
  void* (*create)(const char* config, char* error, size_t error_size);

  /*!
   * Destroy application
   *
   * \param app the application
   */
  void (*destroy)(void* app);

  /*!
   * Handle request (thread safe, never fails: errors are answered with status 500)
   *
   * \param app the application
   * \param request the request
   * \param response the response to fill
   */
  void (*handle)(void* app, const anch_rest_request_t* request, anch_rest_response_t* response);

  /*!
   * Release response memory
   *
   * \param response the response
   */
  void (*release)(anch_rest_response_t* response);
} anch_rest_application_t;

/*! Application entry point type */
typedef const anch_rest_application_t* (*anch_rest_application_fn)(void);

#ifdef __cplusplus
}
#endif

#endif // ANCH_REST_NGINX_H
