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

#include <cstdio>
#include <exception>
#include <new>
#include <string>
#include <vector>

using anch::rest::Api;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;

namespace {

  /*! Response memory owned by application */
  struct Internal {
    std::vector<std::string> strings;
    std::vector<anch_rest_header_t> headers;
    std::string body;
  };

  std::string_view
  view(const anch_rest_str_t& str) noexcept {
    return str.data == nullptr ? std::string_view() : std::string_view(str.data, str.len);
  }

  anch_rest_str_t
  str(const std::string& value) noexcept {
    return anch_rest_str_t{value.data(), value.size()};
  }

  Response
  execute(Api& api, const anch_rest_request_t& in) {
    std::optional<Method> method = anch::rest::parseMethod(view(in.method));
    if(!method) {
      return Response(Status::NOT_IMPLEMENTED);
    }
    Request request(*method, view(in.target));
    for(std::size_t i = 0 ; i < in.headers_count ; ++i) {
      request.getHeaders().add(view(in.headers[i].name), view(in.headers[i].value));
    }
    if(in.has_body) {
      request.setBody(std::string(view(in.body)));
    }
    return api.handle(request);
  }

  void
  fill(Internal& internal, const Response& response, anch_rest_response_t& out) {
    internal.body = response.content();
    for(const auto& [name, values] : response.getHeaders()) {
      for(const std::string& value : values) {
	internal.strings.push_back(name);
	internal.strings.push_back(value);
      }
    }
    // Strings are not moved anymore: headers can point to them
    for(std::size_t i = 0 ; i < internal.strings.size() ; i += 2) {
      internal.headers.push_back(anch_rest_header_t{str(internal.strings[i]), str(internal.strings[i + 1])});
    }
    out.status = anch::rest::code(response.getStatus());
    out.headers = internal.headers.data();
    out.headers_count = internal.headers.size();
    out.body = str(internal.body);
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

void
anch::rest::nginx::handle(void* app, const anch_rest_request_t* request, anch_rest_response_t* response) noexcept {
  *response = anch_rest_response_t{500, nullptr, 0, anch_rest_str_t{nullptr, 0}, nullptr};
  Internal* internal = new(std::nothrow) Internal();
  if(internal == nullptr) {
    return;
  }
  response->internal = internal;
  try {
    fill(*internal, execute(*static_cast<Api*>(app), *request), *response);
  } catch(...) {
    // Response body writer failed: internal server error
    *internal = Internal();
    *response = anch_rest_response_t{500, nullptr, 0, anch_rest_str_t{nullptr, 0}, internal};
  }
}

void
anch::rest::nginx::release(anch_rest_response_t* response) noexcept {
  delete static_cast<Internal*>(response->internal);
  response->internal = nullptr;
}
