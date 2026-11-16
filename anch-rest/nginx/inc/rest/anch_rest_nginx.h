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
 * pool threads (or from the event loop when thread pool is disabled).\n
 * \n
 * Request and response bodies are streamed: the application reads the request body with
 * \ref anch_rest_body_reader_t and writes the response with \ref anch_rest_response_writer_t .
 * In a thread pool, these functions block until nginx has received or sent data.
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! ABI version: the module refuses applications with another version */
#define ANCH_REST_NGINX_ABI_VERSION 2u

/*! Application entry point symbol */
#define ANCH_REST_APPLICATION_SYMBOL "anch_rest_application"

/*! Error returned by \ref anch_rest_body_reader_t read function */
#define ANCH_REST_READ_ERROR ((size_t) -1)

/*! Unknown response content length (body is sent with chunked transfer encoding) */
#define ANCH_REST_UNKNOWN_LENGTH (-1LL)

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

/*! Request body reader */
typedef struct {
  /*! Reader context */
  void* ctx;

  /*!
   * Read body data: blocks until data is available
   *
   * \param ctx the reader context
   * \param data the buffer to fill
   * \param size the buffer size
   *
   * \return the number of read bytes (at least \c 1 ), \c 0 at the end of body,
   *         \ref ANCH_REST_READ_ERROR on error (client closed connection, timeout, body too large, ...)
   */
  size_t (*read)(void* ctx, char* data, size_t size);
} anch_rest_body_reader_t;

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
  /*! Body reader (when \c has_body is set) */
  anch_rest_body_reader_t body;
} anch_rest_request_t;

/*! Response writer: valid during \c handle call */
typedef struct {
  /*! Writer context */
  void* ctx;

  /*! Preferred size of written blocks (size of module buffers) */
  size_t buffer_size;

  /*!
   * Start response: send status and headers. It has to be called once, before \c write .
   * Memory is copied by the module: it can be released after the call.
   *
   * \param ctx the writer context
   * \param status the status code
   * \param headers the headers (\c Content-Length is ignored)
   * \param headers_count the number of headers
   * \param content_length the body length, \ref ANCH_REST_UNKNOWN_LENGTH when unknown
   *
   * \return \c 0 on success, \c -1 when response has been cancelled (client closed connection, ...)
   */
  int (*start)(void* ctx, unsigned int status, const anch_rest_header_t* headers, size_t headers_count,
               long long content_length);

  /*!
   * Write body data: blocks while module buffers are full (client is slower than application)
   *
   * \param ctx the writer context
   * \param data the data
   * \param len the data length
   *
   * \return the number of written bytes (at least \c 1 ), \c 0 when response has been cancelled
   */
  size_t (*write)(void* ctx, const char* data, size_t len);
} anch_rest_response_writer_t;

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
   * Handle request (thread safe). The response is started with \c writer->start , then its body is written
   * with \c writer->write . Errors before start are answered with status 500 by the application.
   *
   * \param app the application
   * \param request the request
   * \param writer the response writer
   *
   * \return \c 0 when response is complete, \c -1 when response has been interrupted after its start (the
   *         connection has to be closed) ; a response which has not been started is answered with 500
   */
  int (*handle)(void* app, const anch_rest_request_t* request, const anch_rest_response_writer_t* writer);
} anch_rest_application_t;

/*! Application entry point type */
typedef const anch_rest_application_t* (*anch_rest_application_fn)(void);

#ifdef __cplusplus
}
#endif

#endif // ANCH_REST_NGINX_H
