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
#include "rest/jsonCodec.hpp"
#include "rest/multipartCodec.hpp"
#include "rest/nginxApplication.hpp"

#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>

using anch::rest::Api;
using anch::rest::Body;
using anch::rest::Json;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::rest::Text;

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
  api->cors({.origins = {"https://app.example.com"}, .headers = {"Content-Type"}, .maxAge = std::chrono::minutes(10)});
  return api;
}

ANCH_REST_APPLICATION(createPetApi)
