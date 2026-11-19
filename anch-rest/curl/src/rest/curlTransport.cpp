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
#include "rest/curlTransport.hpp"

#include <charconv>
#include <iterator>
#include <memory>
#include <optional>

#include <curl/curl.h>

using anch::rest::CurlTransport;
using anch::rest::Headers;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::rest::TransportConfig;
using anch::rest::TransportError;

namespace {

  /*! Global curl initialization (once per process) */
  void
  globalInit() {
    static const CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
    if(code != CURLE_OK) {
      throw TransportError(std::string("curl initialization failed: ") + curl_easy_strerror(code));
    }
  }

  /*! Response being received */
  struct Received {
    std::string body;
    Headers headers;
    /*! Maximum body size (0: no limit) */
    std::size_t maxSize = 0;
    /*! Body is larger than maxSize: transfer has been aborted */
    bool tooLarge = false;
  };

  /*! Read request body stream (errors abort the transfer) */
  std::size_t
  readBody(char* buffer, std::size_t size, std::size_t count, void* userdata) noexcept {
    std::istream* input = static_cast<std::istream*>(userdata);
    input->read(buffer, static_cast<std::streamsize>(size * count));
    if(input->bad()) {
      return CURL_READFUNC_ABORT;
    }
    return static_cast<std::size_t>(input->gcount());
  }

  /*!
   * Rewind request body stream: curl sends the body again after a 307 / 308 redirection or an authentication
   * negotiation. Streams which can not seek make curl fail (CURLE_SEND_FAIL_REWIND).
   */
  int
  seekBody(void* userdata, curl_off_t offset, int origin) noexcept {
    std::istream* input = static_cast<std::istream*>(userdata);
    if(origin != SEEK_SET) {
      return CURL_SEEKFUNC_CANTSEEK;
    }
    input->clear();
    input->seekg(static_cast<std::streamoff>(offset), std::ios_base::beg);
    return input->fail() ? CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
  }

  /*! Check that header can be written in request: no CR, LF or NUL (header injection), name without ':' */
  void
  checkHeader(std::string_view name, std::string_view value) {
    if(name.empty() || name.find_first_of(":\r\n", 0) != std::string_view::npos || name.find('\0') != std::string_view::npos
       || value.find_first_of("\r\n", 0) != std::string_view::npos || value.find('\0') != std::string_view::npos) {
      throw TransportError("Invalid request header '" + std::string(name) + "': empty name, ':' in name, CR, LF or NUL");
    }
  }

  /*! Check if header name is Content-Length (case insensitive) */
  bool
  isContentLength(std::string_view name) noexcept {
    const anch::rest::CaseInsensitiveLess less;
    return !less(name, anch::rest::header::CONTENT_LENGTH) && !less(anch::rest::header::CONTENT_LENGTH, name);
  }

  /*! Request body size from Content-Length header */
  std::optional<curl_off_t>
  contentLength(const anch::rest::Request& request) {
    std::optional<std::string_view> value = request.getHeaders().get(anch::rest::header::CONTENT_LENGTH);
    curl_off_t length = 0;
    if(!value || std::from_chars(value->data(), value->data() + value->size(), length).ec != std::errc() || length < 0) {
      return std::nullopt;
    }
    return length;
  }

  std::size_t
  writeBody(char* data, std::size_t size, std::size_t count, void* userdata) noexcept {
    Received* received = static_cast<Received*>(userdata);
    if(received->maxSize != 0 && received->body.size() + size * count > received->maxSize) {
      received->tooLarge = true;
      return 0; // CURLE_WRITE_ERROR
    }
    try {
      received->body.append(data, size * count);
    } catch(...) {
      return 0;
    }
    return size * count;
  }

  std::size_t
  writeHeader(char* data, std::size_t size, std::size_t count, void* userdata) {
    Received* received = static_cast<Received*>(userdata);
    std::string_view line(data, size * count);
    while(!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
      line.remove_suffix(1);
    }
    if(line.starts_with("HTTP/")) {
      // New response (redirection, 100 Continue, ...): previous headers are dropped
      received->headers = Headers();
    } else {
      const std::size_t colon = line.find(':');
      if(colon != std::string_view::npos) {
	std::string_view value = line.substr(colon + 1);
	const std::size_t first = value.find_first_not_of(" \t");
	value = (first == std::string_view::npos) ? std::string_view() : value.substr(first);
	received->headers.add(line.substr(0, colon), value);
      }
    }
    return size * count;
  }

  /*! Check curl return code */
  void
  check(CURLcode code, const char* errors) {
    if(code != CURLE_OK) {
      std::string message = curl_easy_strerror(code);
      if(errors != nullptr && *errors != '\0') {
	message += ": ";
	message += errors;
      }
      throw TransportError(message);
    }
  }

  /*! curl headers list owner */
  struct HeaderList {
    curl_slist* list = nullptr;
    ~HeaderList() {
      curl_slist_free_all(list);
    }
    void append(const std::string& header) {
      curl_slist* res = curl_slist_append(list, header.c_str());
      if(res == nullptr) {
	throw TransportError("curl headers allocation failed");
      }
      list = res;
    }
  };

}

CurlTransport::CurlTransport(TransportConfig config): _config(std::move(config)), _curl(nullptr), _mutex() {
  globalInit();
  _curl = curl_easy_init();
  if(_curl == nullptr) {
    throw TransportError("curl handle initialization failed");
  }
}

CurlTransport::~CurlTransport() {
  curl_easy_cleanup(static_cast<CURL*>(_curl));
}

Response
CurlTransport::send(const std::string& url, const Request& request) {
  std::lock_guard<std::mutex> lock(_mutex);
  CURL* curl = static_cast<CURL*>(_curl);
  curl_easy_reset(curl); // connections are kept
  char errors[CURL_ERROR_SIZE] = {};
  check(curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errors), nullptr);

  // Configuration +
  check(curl_easy_setopt(curl, CURLOPT_URL, url.c_str()), errors);
  check(curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L), errors);
  check(curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, static_cast<long>(_config.connectTimeout.count())), errors);
  check(curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(_config.timeout.count())), errors);
  check(curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, _config.followRedirects ? 1L : 0L), errors);
  check(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, _config.verifyTls ? 1L : 0L), errors);
  check(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, _config.verifyTls ? 2L : 0L), errors);
  if(_config.proxy) {
    check(curl_easy_setopt(curl, CURLOPT_PROXY, _config.proxy->c_str()), errors);
  }
  if(_config.caFile) {
    check(curl_easy_setopt(curl, CURLOPT_CAINFO, _config.caFile->c_str()), errors);
  }
  // Configuration -

  // Method and body +
  // Body with Content-Length is streamed (large bodies), other bodies are read in memory
  std::string body;
  const std::optional<curl_off_t> length = request.hasBody() ? contentLength(request) : std::nullopt;
  if(length) {
    check(curl_easy_setopt(curl, CURLOPT_POST, 1L), errors);
    check(curl_easy_setopt(curl, CURLOPT_READFUNCTION, readBody), errors);
    check(curl_easy_setopt(curl, CURLOPT_READDATA, &request.getBody()), errors);
    check(curl_easy_setopt(curl, CURLOPT_SEEKFUNCTION, seekBody), errors);
    check(curl_easy_setopt(curl, CURLOPT_SEEKDATA, &request.getBody()), errors);
    check(curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, *length), errors);
  } else if(request.hasBody()) {
    std::istream& input = request.getBody();
    body.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    check(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.data()), errors);
    check(curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.size())), errors);
  }
  if(request.getMethod() == Method::HEAD) {
    check(curl_easy_setopt(curl, CURLOPT_NOBODY, 1L), errors);
  } else if(request.getMethod() != Method::GET || request.hasBody()) {
    const std::string method(anch::rest::toString(request.getMethod()));
    check(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str()), errors);
  }
  // Method and body -

  // Headers +
  HeaderList headers;
  for(const auto& [name, values] : request.getHeaders()) {
    for(const std::string& value : values) {
      checkHeader(name, value);
    }
    if(length && isContentLength(name)) {
      continue; // set by curl from body size
    }
    for(const std::string& value : values) {
      headers.append(name + ": " + value);
    }
  }
  headers.append("Expect:"); // no 100-continue round trip
  if(!request.hasBody() && request.getMethod() != Method::GET && request.getMethod() != Method::HEAD
     && !request.getHeaders().has(anch::rest::header::CONTENT_LENGTH)) {
    headers.append("Content-Length: 0");
  }
  check(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.list), errors);
  // Headers -

  // Response +
  Received received;
  received.maxSize = _config.maxResponseSize;
  if(_config.maxResponseSize != 0) { // aborted before the transfer when Content-Length is known
    check(curl_easy_setopt(curl, CURLOPT_MAXFILESIZE_LARGE, static_cast<curl_off_t>(_config.maxResponseSize)), errors);
  }
  check(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeBody), errors);
  check(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &received), errors);
  check(curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writeHeader), errors);
  check(curl_easy_setopt(curl, CURLOPT_HEADERDATA, &received), errors);
  const CURLcode performed = curl_easy_perform(curl);
  if(received.tooLarge || performed == CURLE_FILESIZE_EXCEEDED) {
    throw TransportError("Response body is larger than " + std::to_string(_config.maxResponseSize) + " bytes");
  }
  check(performed, errors);
  long code = 0;
  check(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code), errors);
  // Response -

  Response response(static_cast<Status>(code));
  for(const auto& [name, values] : received.headers) {
    for(const std::string& value : values) {
      response.getHeaders().add(name, value);
    }
  }
  if(request.getMethod() != Method::HEAD) {
    response.body(std::move(received.body));
  }
  return response;
}
