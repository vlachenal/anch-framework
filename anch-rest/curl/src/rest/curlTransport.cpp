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
#include "rest/curlTransport.hpp"

#include <iterator>
#include <memory>

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
  };

  std::size_t
  writeBody(char* data, std::size_t size, std::size_t count, void* userdata) {
    static_cast<Received*>(userdata)->body.append(data, size * count);
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
  std::string body;
  if(request.hasBody()) {
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
  check(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeBody), errors);
  check(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &received), errors);
  check(curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writeHeader), errors);
  check(curl_easy_setopt(curl, CURLOPT_HEADERDATA, &received), errors);
  check(curl_easy_perform(curl), errors);
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
