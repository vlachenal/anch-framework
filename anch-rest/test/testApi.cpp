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
#include "rest/api.hpp"
#include "rest/httpException.hpp"
#include "rest/jsonCodec.hpp"

#include <optional>
#include <sstream>
#include <stdexcept>
#include <initializer_list>
#include <utility>
#include <iostream>
#include <string>
#include <vector>

#include "uuid.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Api;
using anch::rest::Body;
using anch::rest::HttpException;
using anch::rest::Json;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::rest::Text;

// Model +
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

struct Owner {
  std::string name;
  std::optional<std::string> email;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Owner>& mapper) {
  mapper
    .registerField("name", &Owner::name, anch::json::required, anch::json::minLength(1), anch::json::maxLength(10))
    .registerField<std::string>("email", &Owner::email, anch::json::format("email"))
    ;
}
// Model -

// Helpers +
namespace {

  std::string
  bodyOf(const Response& response) {
    std::ostringstream out;
    response.writeBody(out);
    return out.str();
  }

  Response
  call(Api& api, Method method, std::string_view target, std::string_view accept = "", std::string_view contentType = "",
       std::optional<std::string> body = std::nullopt) {
    Request request(method, target);
    if(!accept.empty()) {
      request.getHeaders().set("Accept", accept);
    }
    if(!contentType.empty()) {
      request.getHeaders().set("Content-Type", contentType);
    }
    if(body) {
      request.setBody(*body);
    }
    return api.handle(request);
  }

  void
  expectStatus(const Response& response, Status expected) {
    anch::ut::assertThat(response.getStatus() == expected, "Expected status {}, found {}",
			 anch::rest::code(expected), anch::rest::code(response.getStatus()));
  }

  std::optional<Pet>
  findPet(uint64_t id) {
    if(id == 1) {
      return Pet{1, "Rex"};
    }
    return std::nullopt;
  }

  Response
  getRoot(const Request&) {
    return Response().contentType("text/plain").body("root");
  }

}
// Helpers -

void
testRouting() {
  Api api;
  api.get("/", getRoot);
  api.get("/pets/mine", []() { return Response().body("mine"); });
  api.get("/pets/{name}", [](std::string name) { return Response().body("pet " + name); });
  api.del("/pets/{id}", [](uint64_t) {});
  Response root = call(api, Method::GET, "/");
  anch::ut::assertThat(bodyOf(root) == "root", "Unexpected root body '{}'", bodyOf(root));
  anch::ut::assertThat(bodyOf(call(api, Method::GET, "/pets/mine")) == "mine", "Literal segment has to have priority");
  anch::ut::assertThat(bodyOf(call(api, Method::GET, "/pets/Rex%20Junior/")) == "pet Rex Junior", "Parameter has to be decoded, trailing slash ignored");
  // DELETE /pets/mine: literal route has no DELETE, parameter route is used
  expectStatus(call(api, Method::DELETE, "/pets/2"), Status::NO_CONTENT);
  expectStatus(call(api, Method::DELETE, "/pets/mine"), Status::BAD_REQUEST);
  expectStatus(call(api, Method::GET, "/owners"), Status::NOT_FOUND);
  expectStatus(call(api, Method::GET, "/pets/a/b"), Status::NOT_FOUND);
  expectStatus(call(api, Method::GET, "/pets/%zz"), Status::BAD_REQUEST);
}

void
testMethodNotAllowed() {
  Api api;
  api.get("/pets", []() { return Response(); });
  api.post("/pets", []() { return Response(Status::CREATED); });
  Response response = call(api, Method::PUT, "/pets");
  expectStatus(response, Status::METHOD_NOT_ALLOWED);
  anch::ut::assertThat(response.getHeaders().get("Allow") == "GET, HEAD, POST", "Unexpected Allow '{}'",
		       response.getHeaders().get("Allow").value_or(""));
}

void
testHead() {
  Api api;
  api.get("/pets/{id}", findPet).produces<Json>();
  Response response = call(api, Method::HEAD, "/pets/1");
  expectStatus(response, Status::OK);
  anch::ut::assertThat(!response.hasBody() && response.getHeaders().get("Content-Type") == "application/json",
		       "HEAD response has to keep headers without body");
}

void
testPathParameters() {
  Api api;
  api.get("/owners/{owner}/pets/{id}", [](const Request& request, anch::UUID owner, int16_t id) {
    return Response().body(owner.toString() + ' ' + std::to_string(id) + ' '
			   + std::string(request.getPathParameter("id").value_or("")));
  });
  std::string uuid = "550e8400-e29b-41d4-a716-446655440000";
  Response response = call(api, Method::GET, "/owners/" + uuid + "/pets/-12");
  anch::ut::assertThat(bodyOf(response) == uuid + " -12 -12", "Unexpected body '{}'", bodyOf(response));
  expectStatus(call(api, Method::GET, "/owners/" + uuid + "/pets/12x"), Status::BAD_REQUEST);
  expectStatus(call(api, Method::GET, "/owners/" + uuid + "/pets/40000"), Status::BAD_REQUEST);
  expectStatus(call(api, Method::GET, "/owners/not-a-uuid/pets/1"), Status::BAD_REQUEST);
}

void
testResults() {
  Api api;
  api.get("/pets/{id}", findPet).produces<Json>();
  api.put("/pets/{id}", [](uint64_t) {});
  api.get("/count", []() { return 42; }).produces<Text>();
  Response found = call(api, Method::GET, "/pets/1");
  expectStatus(found, Status::OK);
  anch::ut::assertThat(bodyOf(found) == "{\"id\":1,\"name\":\"Rex\"}", "Unexpected JSON '{}'", bodyOf(found));
  expectStatus(call(api, Method::GET, "/pets/2"), Status::NOT_FOUND);
  expectStatus(call(api, Method::PUT, "/pets/2"), Status::NO_CONTENT);
  Response count = call(api, Method::GET, "/count");
  anch::ut::assertThat(bodyOf(count) == "42" && count.getHeaders().get("Content-Type") == "text/plain", "Unexpected text result");
}

void
testNegotiation() {
  Api api;
  api.get("/pets/{id}", [](uint64_t id) { return "pet " + std::to_string(id); }).produces<Json>().produces<Text>();
  Response text = call(api, Method::GET, "/pets/3", "application/json;q=0.5, text/*");
  anch::ut::assertThat(text.getHeaders().get("Content-Type") == "text/plain" && bodyOf(text) == "pet 3", "text/plain expected");
  Response json = call(api, Method::GET, "/pets/3", "*/*");
  anch::ut::assertThat(json.getHeaders().get("Content-Type") == "application/json" && bodyOf(json) == "\"pet 3\"", "application/json expected, found '{}'", bodyOf(json));
  expectStatus(call(api, Method::GET, "/pets/3", "image/png"), Status::NOT_ACCEPTABLE);
}

void
testBody() {
  Api api;
  api.post("/pets", [](Body<Pet> pet) {
    return Response(Status::CREATED).header("Location", "/pets/" + std::to_string(pet->id)).body(pet->name);
  }).consumes<Json>();
  Response created = call(api, Method::POST, "/pets", "", "application/json; charset=utf-8", "{\"id\":7,\"name\":\"Médor\"}");
  expectStatus(created, Status::CREATED);
  anch::ut::assertThat(created.getHeaders().get("Location") == "/pets/7" && bodyOf(created) == "Médor", "Unexpected created pet");
  expectStatus(call(api, Method::POST, "/pets", "", "text/plain", "plop"), Status::UNSUPPORTED_MEDIA_TYPE);
  expectStatus(call(api, Method::POST, "/pets", "", "", "{}"), Status::UNSUPPORTED_MEDIA_TYPE);
  expectStatus(call(api, Method::POST, "/pets", "", "application/json"), Status::BAD_REQUEST);
  Response invalid = call(api, Method::POST, "/pets", "", "application/json", "{\"id\":\"seven\"}");
  expectStatus(invalid, Status::BAD_REQUEST);
  anch::ut::assertThat(!bodyOf(invalid).empty(), "Invalid body error has to be described");
}

void
testConstraints() {
  Api api;
  api.post("/owners", [](Body<Owner> owner) { return Response(Status::CREATED).body(owner->name); }).consumes<Json>();
  api.get("/owners/invalid", []() { return Owner{"", "x"}; }).produces<Json>();
  expectStatus(call(api, Method::POST, "/owners", "", "application/json", R"({"name":"Rex","email":"rex@pets.org"})"), Status::CREATED);
  // Request bodies are checked: 400 with the path of the invalid value
  for(auto [body, message] : std::initializer_list<std::pair<const char*, const char*>>{
      {R"({"email":"rex@pets.org"})", "Missing required field 'name'"},
      {R"({"name":""})", "/name: Length 0 is lower than minLength 1"},
      {R"({"name":"Rex","email":"rex"})", "/email: Value is not a valid email (format)"}}) {
    Response invalid = call(api, Method::POST, "/owners", "", "application/json", body);
    expectStatus(invalid, Status::BAD_REQUEST);
    anch::ut::assertThat(bodyOf(invalid).starts_with(message), "'{}' expected ; found '{}'", message, bodyOf(invalid));
  }
  // Responses are not checked by the server
  Response response = call(api, Method::GET, "/owners/invalid", "application/json");
  expectStatus(response, Status::OK);
  anch::ut::assertThat(bodyOf(response) == R"({"name":"","email":"x"})", "Unexpected body '{}'", bodyOf(response));
  // Server stays strict: unknown fields and whitespaces are refused
  expectStatus(call(api, Method::POST, "/owners", "", "application/json", R"({"name":"Rex","age":3})"), Status::BAD_REQUEST);
  expectStatus(call(api, Method::POST, "/owners", "", "application/json", R"({ "name": "Rex" })"), Status::BAD_REQUEST);
  // Server options can be changed
  Json::options(anch::rest::Side::SERVER).deserialize_check_constraints = false;
  expectStatus(call(api, Method::POST, "/owners", "", "application/json", R"({"name":""})"), Status::CREATED);
  Json::options(anch::rest::Side::SERVER).deserialize_check_constraints = true;
}

void
testFilters() {
  Api api;
  std::vector<std::string> calls;
  api.get("/pets", [&calls]() { calls.push_back("endpoint"); return Response(); });
  api.filter([&calls](Request& request, const anch::rest::Next& next) {
    calls.push_back("first");
    if(!request.getHeaders().has("Authorization")) {
      return Response(Status::UNAUTHORIZED);
    }
    Response response = next(request);
    response.header("X-Request-ID", "42");
    return response;
  });
  api.filter([&calls](Request& request, const anch::rest::Next& next) {
    calls.push_back("second");
    return next(request);
  });
  expectStatus(call(api, Method::GET, "/pets"), Status::UNAUTHORIZED);
  anch::ut::assertThat(calls == std::vector<std::string>({"first"}), "First filter has to stop the chain");
  calls.clear();
  Request request(Method::GET, "/pets");
  request.getHeaders().set("Authorization", "Bearer token");
  Response response = api.handle(request);
  anch::ut::assertThat(calls == std::vector<std::string>({"first", "second", "endpoint"}), "Filters have to be executed in order");
  anch::ut::assertThat(response.getHeaders().get("X-Request-ID") == "42", "Filter has to change response");
}

void
testErrors() {
  Api api;
  api.get("/conflict", []() -> Response { throw HttpException(Status::CONFLICT, "already exists"); });
  api.get("/crash", []() -> Response { throw std::runtime_error("secret detail"); });
  Response conflict = call(api, Method::GET, "/conflict");
  expectStatus(conflict, Status::CONFLICT);
  anch::ut::assertThat(bodyOf(conflict) == "already exists", "HttpException message has to be the body");
  Response crash = call(api, Method::GET, "/crash");
  expectStatus(crash, Status::INTERNAL_SERVER_ERROR);
  anch::ut::assertThat(!crash.hasBody(), "Internal errors must not be exposed");
}

void
testRegistration() {
  Api api;
  api.get("/pets/{id}", [](uint64_t) {});
  auto throws = [](auto declare) {
    try {
      declare();
    } catch(const std::invalid_argument&) {
      return true;
    }
    return false;
  };
  anch::ut::assertThat(throws([&]() { api.get("/pets/{petId}", [](uint64_t) {}); }), "Duplicate route has to be refused");
  anch::ut::assertThat(throws([&]() { api.get("/owners/{id}", []() {}); }), "Missing path parameter argument has to be refused");
  anch::ut::assertThat(throws([&]() { api.get("/owners/{id}/{id}", [](int, int) {}); }), "Duplicate parameter name has to be refused");
  anch::ut::assertThat(throws([&]() { api.get("owners", []() {}); }), "Pattern has to start with '/'");
  anch::ut::assertThat(throws([&]() { api.get("/owners/id-{id}", [](int) {}); }), "Parameter has to be a whole segment");
}

void
testValidate() {
  Api api;
  api.get("/pets/{id}", findPet);
  api.post("/pets", [](Body<Pet>) {});
  try {
    api.validate();
  } catch(const std::logic_error& e) {
    std::string message = e.what();
    anch::ut::assertThat(message.find("GET /pets/{id}: no produced") != std::string::npos
			 && message.find("POST /pets: no consumed") != std::string::npos, "Unexpected message '{}'", message);
    return;
  }
  anch::ut::assertThat(false, "validate() has to fail");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST API unit tests")
    .description("Test endpoints declaration, routing, negotiation and filters")
    .add("routing", testRouting)
    .add("method-not-allowed", testMethodNotAllowed)
    .add("head", testHead)
    .add("path-parameters", testPathParameters)
    .add("results", testResults)
    .add("negotiation", testNegotiation)
    .add("body", testBody)
    .add("constraints", testConstraints)
    .add("filters", testFilters)
    .add("errors", testErrors)
    .add("registration", testRegistration)
    .add("validate", testValidate)
    ;
}
