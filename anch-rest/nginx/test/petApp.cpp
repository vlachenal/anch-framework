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
/*
 * Test application for AnCH REST nginx module: built as a shared library and loaded with
 * anch_rest_app directive (or with dlopen by anch-rest-nginx-test).
 */
#include "codec/json.hpp"
#include "codec/text.hpp"

#include "rest/multipartCodec.hpp"
#include "rest/nginxApplication.hpp"
#include "rest/paramConverter.hpp"

#include <algorithm>
#include <chrono>
#include <istream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <thread>

using anch::rest::Api;
using anch::rest::Body;
using anch::codec::Json;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::codec::Text;

/*!
 * Write \c size bytes of a deterministic pattern ('a' to 'z' repeated) in blocks of 1000 bytes
 *
 * \param out the output stream
 * \param size the number of bytes
 */
void
writePattern(std::ostream& out, uint64_t size) {
  char block[1000];
  for(std::size_t i = 0 ; i < sizeof(block) ; ++i) {
    block[i] = static_cast<char>('a' + static_cast<char>(i % 26));
  }
  uint64_t offset = 0;
  while(offset < size) {
    // Pattern is continuous: each block starts at offset % 26
    const std::size_t len = static_cast<std::size_t>(std::min<uint64_t>(size - offset, 974)); // 974 = 26 * 37 + 12
    out.write(block + offset % 26, static_cast<std::streamsize>(len));
    offset += len;
  }
}

struct Pet {
  uint64_t id;
  std::string name;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Pet>& mapper) {
  mapper
    .registerField("id", &Pet::id)
    .registerField("name", &Pet::name)
    ;
}

std::unique_ptr<Api>
createPetApi(std::string_view config) {
  if(config == "fail") {
    throw std::runtime_error("configured to fail");
  }
  auto api = std::make_unique<Api>();
  api->get("/api/pets/{id}", [](uint64_t id) -> std::optional<Pet> {
    if(id == 1) {
      return Pet{1, "Rex"};
    }
    return std::nullopt;
  }).produces<Json>();
  api->post("/api/pets", [](Body<Pet> pet) {
    return Response(Status::CREATED).header("Location", "/api/pets/" + std::to_string(pet->id))
      .contentType("text/plain").body(pet->name);
  }).consumes<Json>();
  std::string configuration(config);
  api->get("/api/config", [configuration]() { return configuration; }).produces<Text>();
  api->get("/api/headers", [](const Request& request) {
    return std::string(request.getHeaders().get("X-Test").value_or("")) + ' '
      + std::string(request.getQueryParameter("q").value_or(""));
  }).produces<Text>();
  api->get("/api/slow", []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return std::string("slow");
  }).produces<Text>();
  api->post("/api/pets/{id}/photo", [](uint64_t id, Body<anch::rest::Form> form) {
    const anch::rest::Part* photo = form->part("photo");
    if(photo == nullptr || !photo->isFile()) {
      throw anch::rest::HttpException(Status::BAD_REQUEST, "Missing photo");
    }
    return std::to_string(id) + ' ' + std::string(form->field("description").value_or("")) + ' '
      + std::string(*photo->getFilename()) + ' ' + photo->getMediaType().essence() + ' ' + std::to_string(photo->size());
  }).consumes<anch::rest::Multipart>().produces<Text>();
  api->get("/api/crash", []() -> Response { throw std::runtime_error("crash"); });
  // Streamed bodies +
  auto stream = [](const Request& request) {
    const uint64_t size = anch::rest::queryParameter<uint64_t>(request, "size").value_or(0);
    return Response(Status::OK).contentType("text/plain").stream([size](std::ostream& out) { writePattern(out, size); });
  };
  api->get("/api/stream", stream);
  api->get("/big/stream", stream); // location with limit_rate and small buffers
  api->get("/api/stream-crash", []() {
    return Response(Status::OK).contentType("text/plain").stream([](std::ostream& out) {
      writePattern(out, 1000000);
      throw std::runtime_error("crash while streaming");
    });
  });
  // Request body is copied to response body while it is read (request outlives response body writing)
  api->post("/api/echo", [](const Request& request) {
    std::istream& body = request.getBody();
    return Response(Status::OK).contentType("application/octet-stream").stream([&body](std::ostream& out) {
      char buffer[8192];
      while(body.read(buffer, sizeof(buffer)) || body.gcount() > 0) {
	out.write(buffer, body.gcount());
      }
      if(body.bad()) {
	throw std::runtime_error("request body can not be read");
      }
    });
  });
  // Request body is read before response start: reading errors (413, ...) are answered by the module
  auto count = [](const Request& request) {
    std::istream& body = request.getBody();
    uint64_t count = 0;
    char buffer[8192];
    while(body.read(buffer, sizeof(buffer)) || body.gcount() > 0) {
      count += static_cast<uint64_t>(body.gcount());
    }
    if(body.bad()) {
      throw anch::rest::HttpException(Status::BAD_REQUEST, "request body can not be read");
    }
    return std::to_string(count);
  };
  api->post("/api/count", count).produces<Text>();
  api->post("/big/count", count).produces<Text>(); // location without body size limit
  // Streamed bodies -
  api->cors({.origins = {"https://app.example.com"}, .headers = {"Content-Type"}, .maxAge = std::chrono::minutes(10)});
  return api;
}

ANCH_REST_APPLICATION(createPetApi)
