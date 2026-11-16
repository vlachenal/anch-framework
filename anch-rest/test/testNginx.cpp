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

#include <algorithm>

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

  /*! Request body given in blocks of 7 bytes (as nginx gives received data) */
  struct Reader {
    std::string_view data;
    std::size_t offset = 0;
    /*! Error after this number of bytes */
    std::size_t errorAt = std::string::npos;
    std::size_t calls = 0;

    static std::size_t
    read(void* ctx, char* buffer, std::size_t size) {
      Reader& reader = *static_cast<Reader*>(ctx);
      ++reader.calls;
      if(reader.offset >= reader.errorAt) {
	return ANCH_REST_READ_ERROR;
      }
      std::size_t n = std::min({size, std::size_t(7), reader.data.size() - reader.offset, reader.errorAt - reader.offset});
      std::copy_n(reader.data.data() + reader.offset, n, buffer);
      reader.offset += n;
      return n;
    }
  };

  /*! Response written by application */
  struct Result {
    int handled = 0;
    unsigned int status = 0;
    std::size_t starts = 0;
    long long length = 0;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    std::vector<std::size_t> blocks;
    /*! Cancel response after this number of bytes */
    std::size_t cancelAt = std::string::npos;

    std::string header(std::string_view name) const {
      for(const auto& [key, value] : headers) {
	if(key == name) {
	  return value;
	}
      }
      return std::string();
    }

    static int
    start(void* ctx, unsigned int status, const anch_rest_header_t* headers, std::size_t count, long long length) {
      Result& result = *static_cast<Result*>(ctx);
      ++result.starts;
      result.status = status;
      result.length = length;
      for(std::size_t i = 0 ; i < count ; ++i) {
	result.headers.emplace_back(view(headers[i].name), view(headers[i].value));
      }
      return 0;
    }

    static std::size_t
    write(void* ctx, const char* data, std::size_t len) {
      Result& result = *static_cast<Result*>(ctx);
      if(result.body.size() >= result.cancelAt) {
	return 0;
      }
      result.blocks.push_back(len);
      result.body.append(data, len);
      return len;
    }
  };

  /*! Module buffer size given to application */
  constexpr std::size_t BUFFER_SIZE = 4096;

  Result
  call(std::string_view method, std::string_view target, std::vector<anch_rest_header_t> headers = {},
       const char* body = nullptr, Reader reader = {}, Result result = {}) {
    if(body != nullptr) {
      reader.data = body;
    }
    anch_rest_request_t request{str(method), str(target), headers.data(), headers.size(), body != nullptr ? 1 : 0,
				anch_rest_body_reader_t{&reader, &Reader::read}};
    anch_rest_response_writer_t writer{&result, BUFFER_SIZE, &Result::start, &Result::write};
    result.handled = ops->handle(app, &request, &writer);
    return result;
  }

  /*! Expected pattern of /api/stream */
  std::string
  pattern(std::size_t size) {
    std::string res(size, ' ');
    for(std::size_t i = 0 ; i < size ; ++i) {
      res[i] = static_cast<char>('a' + static_cast<char>(i % 26));
    }
    return res;
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
testStreamSmall() {
  Result result = call("GET", "/api/stream?size=100");
  anch::ut::assertThat(result.handled == 0 && result.starts == 1 && result.status == 200, "Unexpected response {} {} {}", result.handled, result.starts, result.status);
  anch::ut::assertThat(result.length == 100 && result.body == pattern(100), "Body in one block has to be started with its length: {} {}", result.length, result.body.size());
  Result empty = call("GET", "/api/stream?size=0");
  anch::ut::assertThat(empty.starts == 1 && empty.length == 0 && empty.body.empty(), "Empty body has to be started with length 0: {}", empty.length);
}

void
testStreamLarge() {
  Result result = call("GET", "/api/stream?size=1000000");
  anch::ut::assertThat(result.handled == 0 && result.starts == 1 && result.status == 200, "Unexpected response {} {} {}", result.handled, result.starts, result.status);
  anch::ut::assertThat(result.length == ANCH_REST_UNKNOWN_LENGTH, "Large body length has to be unknown: {}", result.length);
  anch::ut::assertThat(result.body == pattern(1000000), "Unexpected body of {} bytes", result.body.size());
  anch::ut::assertThat(result.blocks.size() >= 1000000 / BUFFER_SIZE
		       && std::ranges::all_of(result.blocks, [](std::size_t len) { return len <= BUFFER_SIZE; }),
		       "Body has to be written in blocks of buffer size: {} blocks", result.blocks.size());
  anch::ut::assertThat(result.header("Content-Type") == "text/plain", "Headers have to be started with first block");
}

void
testEcho() {
  const std::string body = pattern(1000000);
  Reader reader;
  Result result = call("POST", "/api/echo", {}, body.c_str(), reader);
  anch::ut::assertThat(result.handled == 0 && result.status == 200 && result.body == body, "Unexpected echo {} {} of {} bytes", result.handled, result.status, result.body.size());
  Result empty = call("POST", "/api/echo", {}, "");
  anch::ut::assertThat(empty.status == 200 && empty.length == 0, "Empty echo: {} {}", empty.status, empty.length);
}

void
testReadError() {
  Reader reader;
  reader.errorAt = 10;
  Result result = call("POST", "/api/pets", {{str("Content-Type"), str("application/json")}}, "{\"id\":7,\"name\":\"Médor\"}", reader);
  anch::ut::assertThat(result.handled == 0 && result.status == 400, "Read error has to be answered with 400: {} {}", result.handled, result.status);
  // Read error after response start: interrupted
  const std::string body = pattern(100000);
  reader = Reader();
  reader.errorAt = 50000;
  Result echo = call("POST", "/api/echo", {}, body.c_str(), reader);
  anch::ut::assertThat(echo.handled == -1 && echo.starts == 1 && echo.body.size() < 50000, "Read error after start has to interrupt response: {} {}", echo.handled, echo.body.size());
}

void
testWriteCancelled() {
  Result cancel;
  cancel.cancelAt = 3 * BUFFER_SIZE;
  Result result = call("GET", "/api/stream?size=100000000", {}, nullptr, {}, cancel);
  anch::ut::assertThat(result.handled == -1 && result.body.size() == 3 * BUFFER_SIZE, "Cancelled response has to stop writer: {} {}", result.handled, result.body.size());
}

void
testCrash() {
  Result before = call("GET", "/api/crash");
  anch::ut::assertThat(before.handled == 0 && before.starts == 1 && before.status == 500 && before.length == 0, "Error before start has to be 500: {} {} {}", before.handled, before.status, before.length);
  Result after = call("GET", "/api/stream-crash");
  anch::ut::assertThat(after.handled == -1 && after.starts == 1 && after.status == 200 && after.body.size() < 1000000, "Error after start has to interrupt response: {} {} {}", after.handled, after.status, after.body.size());
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
    .add("stream-small", testStreamSmall)
    .add("stream-large", testStreamLarge)
    .add("echo", testEcho)
    .add("read-error", testReadError)
    .add("write-cancelled", testWriteCancelled)
    .add("crash", testCrash)
    ;
}
