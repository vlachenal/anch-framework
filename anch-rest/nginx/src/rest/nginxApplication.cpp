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
#include "rest/nginxApplication.hpp"

#include <charconv>
#include <cstdio>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "cutils/iostream.hpp"

using anch::rest::Api;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;

namespace {

  /*! Size of request body reading area when module does not give one */
  constexpr std::size_t DEFAULT_BUFFER_SIZE = 16 * 1024;

  std::string_view
  view(const anch_rest_str_t& str) noexcept {
    return str.data == nullptr ? std::string_view() : std::string_view(str.data, str.len);
  }

  anch_rest_str_t
  str(std::string_view value) noexcept {
    return anch_rest_str_t{value.data(), value.size()};
  }

  /*!
   * Build request: its body is read in stream from module reader
   *
   * \param in the module request
   * \param bufferSize the reading area size
   *
   * \return the request, empty when method is unknown
   */
  std::optional<Request>
  request(const anch_rest_request_t& in, std::size_t bufferSize) {
    std::optional<Method> method = anch::rest::parseMethod(view(in.method));
    if(!method) {
      return std::nullopt;
    }
    std::optional<Request> request(std::in_place, *method, view(in.target));
    for(std::size_t i = 0 ; i < in.headers_count ; ++i) {
      request->getHeaders().add(view(in.headers[i].name), view(in.headers[i].value));
    }
    if(in.has_body) {
      const anch_rest_body_reader_t reader = in.body;
      anch::cutils::cbuffer buffer{
	.data = nullptr,
	.size = bufferSize,
	.read = [reader](char* data, std::size_t size) -> std::size_t {
	  const std::size_t read = reader.read(reader.ctx, data, size);
	  if(read == ANCH_REST_READ_ERROR) {
	    // Stream badbit: body is invalid for deserializers
	    throw std::runtime_error("Request body can not be read");
	  }
	  return read;
	},
	.write = nullptr
      };
      request->setBody(std::make_unique<anch::cutils::CIStream>(buffer));
    }
    return request;
  }

  /*!
   * Response sent with module writer: status and headers are sent when more than one block has been written (the
   * body is then sent with chunked transfer encoding), or at the end of the body with its length. Data flushed
   * before (by serializers for example) are kept until then.
   */
  class Sender {
  private:
    /*! Module writer */
    const anch_rest_response_writer_t& _writer;
    /*! Block size */
    std::size_t _blockSize;
    /*! Response (status and headers) */
    const Response* _response;
    /*! Data written before response start */
    std::string _pending;
    /*! Response has been started (successfully or not) */
    bool _started;
    /*! Response has been cancelled by module */
    bool _cancelled;

  public:
    /*! Last block is written: the content length is known when response is not started yet */
    bool finishing;
    /*! Body writer has failed: pending data are dropped */
    bool failed;

  public:
    Sender(const anch_rest_response_writer_t& writer, std::size_t blockSize):
      _writer(writer), _blockSize(blockSize), _response(nullptr), _pending(), _started(false), _cancelled(false),
      finishing(false), failed(false) {
    }

    void
    response(const Response& response) noexcept {
      _response = &response;
    }

    bool
    started() const noexcept {
      return _started;
    }

    bool
    cancelled() const noexcept {
      return _cancelled;
    }

    /*!
     * Start response
     *
     * \param length the content length (\c ANCH_REST_UNKNOWN_LENGTH when unknown)
     *
     * \return \c true on success
     */
    bool
    start(long long length) {
      _started = true;
      std::vector<anch_rest_header_t> headers;
      for(const auto& [name, values] : _response->getHeaders()) {
	for(const std::string& value : values) {
	  headers.push_back(anch_rest_header_t{str(name), str(value)});
	}
      }
      if(_writer.start(_writer.ctx, anch::rest::code(_response->getStatus()), headers.data(), headers.size(), length) != 0) {
	_cancelled = true;
      }
      return !_cancelled;
    }

    /*!
     * Start internal server error response (no header, no body)
     */
    void
    error() noexcept {
      _started = true;
      if(_writer.start(_writer.ctx, anch::rest::code(Status::INTERNAL_SERVER_ERROR), nullptr, 0, 0) != 0) {
	_cancelled = true;
      }
    }

    /*!
     * Write block (\c anch::cutils::cbuffer write function)
     *
     * \param data the data
     * \param len the data length
     *
     * \return the number of written bytes, \c 0 on error
     */
    std::size_t
    write(const char* data, std::size_t len) {
      if(failed || _cancelled) {
	return 0;
      }
      if(!_started) {
	if(!finishing && _pending.size() + len <= _blockSize) {
	  _pending.append(data, len); // flushed before the end of the first block: response is not started yet
	  return len;
	}
	const long long length = finishing ? static_cast<long long>(_pending.size() + len) : ANCH_REST_UNKNOWN_LENGTH;
	if(!start(length) || !writeAll(_pending.data(), _pending.size())) {
	  return 0;
	}
	_pending.clear();
      }
      return writeAll(data, len) ? len : 0;
    }

    /*!
     * End of body: start response with its length when it has not been started yet
     */
    void
    finish() {
      if(!_started && start(static_cast<long long>(_pending.size()))) {
	writeAll(_pending.data(), _pending.size());
      }
    }

  private:
    /*!
     * Write data with module writer (which can write less than requested)
     *
     * \return \c true on success, \c false when response has been cancelled
     */
    bool
    writeAll(const char* data, std::size_t len) {
      while(len > 0) {
	const std::size_t written = _writer.write(_writer.ctx, data, len);
	if(written == 0 || written > len) {
	  _cancelled = true;
	  return false;
	}
	data += written;
	len -= written;
      }
      return true;
    }
  };

  /*!
   * Write response body in stream
   *
   * \param sender the sender
   * \param response the response
   * \param bufferSize the block size
   * \param head \c true for a \c HEAD request
   */
  void
  send(Sender& sender, const Response& response, std::size_t bufferSize, bool head) {
    sender.response(response);
    if(head && !response.hasBody()) {
      // HEAD response: Content-Length of the GET response (given by Api) or no length, never 0 (empty body)
      long long length = ANCH_REST_UNKNOWN_LENGTH;
      if(std::optional<std::string_view> value = response.getHeaders().get(anch::rest::header::CONTENT_LENGTH)) {
	long long parsed = 0;
	if(std::from_chars(value->data(), value->data() + value->size(), parsed).ec == std::errc() && parsed >= 0) {
	  length = parsed;
	}
      }
      sender.start(length);
      return;
    }
    // Declared out of try block: destructor flush is done after failure is known (pending data are dropped)
    anch::cutils::COStream out(anch::cutils::cbuffer{
	.data = nullptr,
	.size = bufferSize,
	.read = nullptr,
	.write = [&sender](const char* data, std::size_t len) { return sender.write(data, len); }
      });
    out.exceptions(std::ios_base::badbit); // body writer is stopped when response is cancelled
    try {
      response.writeBody(out);
      sender.finishing = true;
      out.flush();
      sender.finish();
    } catch(...) {
      sender.failed = true;
      throw;
    }
  }

}

void*
anch::rest::nginx::create(Factory factory, const char* config, char* error, std::size_t errorSize) noexcept {
  try {
    std::unique_ptr<Api> api = factory(config == nullptr ? std::string_view() : std::string_view(config));
    if(!api) {
      std::snprintf(error, errorSize, "application factory returned no API");
      return nullptr;
    }
    api->validate();
    return api.release();
  } catch(const std::exception& e) {
    std::snprintf(error, errorSize, "%s", e.what());
  } catch(...) {
    std::snprintf(error, errorSize, "unknown error");
  }
  return nullptr;
}

void
anch::rest::nginx::destroy(void* app) noexcept {
  delete static_cast<Api*>(app);
}

int
anch::rest::nginx::handle(void* app, const anch_rest_request_t* in, const anch_rest_response_writer_t* writer) noexcept {
  const std::size_t bufferSize = writer->buffer_size == 0 ? DEFAULT_BUFFER_SIZE : writer->buffer_size;
  Sender sender(*writer, bufferSize);
  try {
    // Request is kept until response body has been written: body writers can read it
    std::optional<Request> request = ::request(*in, bufferSize);
    const Response response = request ? static_cast<Api*>(app)->handle(*request) : Response(Status::NOT_IMPLEMENTED);
    send(sender, response, bufferSize, request && request->getMethod() == anch::rest::Method::HEAD);
  } catch(...) {
    if(!sender.started()) {
      sender.error(); // nothing has been sent: internal server error
    } else {
      return -1; // response has to be interrupted
    }
  }
  return sender.cancelled() ? -1 : 0;
}
