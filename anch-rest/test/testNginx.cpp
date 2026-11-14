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
#include "rest/anch_rest_nginx.h"

#include <dlfcn.h>

#include <string>
#include <string_view>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

// Application loaded like nginx module does +
namespace {

  void* handle = nullptr;
  const anch_rest_application_t* ops = nullptr;
  void* app = nullptr;

  anch_rest_str_t
  str(std::string_view value) {
    return anch_rest_str_t{value.data(), value.size()};
  }

  std::string_view
  view(const anch_rest_str_t& value) {
    return value.data == nullptr ? std::string_view() : std::string_view(value.data, value.len);
  }

  /*! Response copy (released) */
  struct Result {
    unsigned int status = 0;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;

    std::string header(std::string_view name) const {
      for(const auto& [key, value] : headers) {
	if(key == name) {
	  return value;
	}
      }
      return std::string();
    }
  };

  Result
  call(std::string_view method, std::string_view target, std::vector<anch_rest_header_t> headers = {},
       const char* body = nullptr) {
    anch_rest_request_t request{str(method), str(target), headers.data(), headers.size(), body != nullptr ? 1 : 0,
				body != nullptr ? str(body) : anch_rest_str_t{nullptr, 0}};
    anch_rest_response_t response;
    ops->handle(app, &request, &response);
    Result result;
    result.status = response.status;
    for(std::size_t i = 0 ; i < response.headers_count ; ++i) {
      result.headers.emplace_back(view(response.headers[i].name), view(response.headers[i].value));
    }
    result.body = std::string(view(response.body));
    ops->release(&response);
    return result;
  }

  void
  load() {
    handle = ::dlopen(ANCH_REST_NGINX_TEST_APP, RTLD_NOW | RTLD_LOCAL);
    anch::ut::assertThat(handle != nullptr, "dlopen failed: {}", ::dlerror());
    auto entry = reinterpret_cast<anch_rest_application_fn>(::dlsym(handle, ANCH_REST_APPLICATION_SYMBOL));
    anch::ut::assertThat(entry != nullptr, "anch_rest_application is not exported");
    ops = entry();
    anch::ut::assertThat(ops != nullptr && ops->abi_version == ANCH_REST_NGINX_ABI_VERSION, "Unexpected ABI version");
    char error[256] = {};
    app = ops->create("test configuration", error, sizeof(error));
    anch::ut::assertThat(app != nullptr, "Application creation failed: {}", error);
  }

  void
  unload() {
    if(app != nullptr) {
      ops->destroy(app);
    }
    if(handle != nullptr) {
      ::dlclose(handle);
    }
  }

}
// Application loaded like nginx module does -

void
testCreateError() {
  char error[256] = {};
  void* failed = ops->create("fail", error, sizeof(error));
  anch::ut::assertThat(failed == nullptr && std::string_view(error) == "configured to fail", "Unexpected error '{}'", error);
}

void
testGet() {
  Result result = call("GET", "/api/pets/1", {{str("Accept"), str("application/json")}});
  anch::ut::assertThat(result.status == 200 && result.body == "{\"id\":1,\"name\":\"Rex\"}", "Unexpected response {} '{}'", result.status, result.body);
  anch::ut::assertThat(result.header("Content-Type") == "application/json", "Unexpected content type '{}'", result.header("Content-Type"));
  anch::ut::assertThat(call("GET", "/api/pets/2").status == 404, "Expected 404");
}

void
testPost() {
  Result result = call("POST", "/api/pets", {{str("Content-Type"), str("application/json")}}, "{\"id\":7,\"name\":\"Médor\"}");
  anch::ut::assertThat(result.status == 201 && result.header("Location") == "/api/pets/7" && result.body == "Médor",
		       "Unexpected response {} '{}'", result.status, result.body);
  anch::ut::assertThat(call("POST", "/api/pets", {{str("Content-Type"), str("application/json")}}).status == 400, "Missing body has to be refused");
}

void
testRequest() {
  Result result = call("GET", "/api/headers?q=a%20b", {{str("x-test"), str("value")}});
  anch::ut::assertThat(result.body == "value a b", "Unexpected body '{}'", result.body);
  anch::ut::assertThat(call("GET", "/api/config").body == "test configuration", "Configuration has to be given to factory");
}

void
testErrors() {
  anch::ut::assertThat(call("BREW", "/api/pets/1").status == 501, "Unknown method has to be 501");
  anch::ut::assertThat(call("GET", "/api/crash").status == 500, "Exception has to be 500");
}

void
testCors() {
  Result preflight = call("OPTIONS", "/api/pets", {{str("Origin"), str("https://app.example.com")},
						   {str("Access-Control-Request-Method"), str("POST")},
						   {str("Access-Control-Request-Headers"), str("content-type")}});
  anch::ut::assertThat(preflight.status == 204, "Unexpected preflight status {}", preflight.status);
  anch::ut::assertThat(preflight.header("Access-Control-Allow-Origin") == "https://app.example.com"
		       && preflight.header("Access-Control-Allow-Methods") == "POST, OPTIONS"
		       && preflight.header("Access-Control-Allow-Headers") == "content-type"
		       && preflight.header("Access-Control-Max-Age") == "600", "Unexpected preflight headers");
  Result options = call("OPTIONS", "/api/pets/1");
  anch::ut::assertThat(options.status == 204 && options.header("Allow") == "GET, HEAD, OPTIONS", "Unexpected OPTIONS response {} '{}'",
		       options.status, options.header("Allow"));
  Result get = call("GET", "/api/pets/1", {{str("Origin"), str("https://app.example.com")}});
  anch::ut::assertThat(get.status == 200 && get.header("Access-Control-Allow-Origin") == "https://app.example.com"
		       && get.header("Vary") == "Origin", "Unexpected CORS headers on GET");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST nginx bridge unit tests")
    .description("Test nginx application C ABI with a test application loaded with dlopen")
    .initialize(load)
    .uninitialize(unload)
    .add("create-error", testCreateError)
    .add("get", testGet)
    .add("post", testPost)
    .add("request", testRequest)
    .add("errors", testErrors)
    .add("cors", testCors)
    ;
}
