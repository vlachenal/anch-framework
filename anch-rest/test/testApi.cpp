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

#include "codec/json.hpp"
#include "codec/text.hpp"

#include "rest/formCodec.hpp"
#include "rest/multipartCodec.hpp"
#include "rest/httpException.hpp"

#include <chrono>
#include <iterator>
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
using anch::codec::Json;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::codec::Text;

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

// Codec with media type parameters (as a multipart codec with its boundary) +
struct Bounded {
  static std::string_view
  mediaType() noexcept {
    return "multipart/x-test";
  }

  static anch::codec::MediaType
  contentType(const std::string& value) {
    anch::codec::MediaType type("multipart", "x-test");
    type.setParameter("boundary", "b" + std::to_string(value.size()));
    return type;
  }

  static void
  serialize(const std::string& value, std::ostream& out, const anch::codec::MediaType& contentType, anch::codec::Side side) {
    out << "--" << contentType.getParameter("boundary").value_or("") << ' ' << (side == anch::codec::Side::SERVER ? "server" : "client")
	<< ' ' << value;
  }

  static void
  deserialize(std::string& value, std::istream& input, const anch::codec::MediaType& contentType, anch::codec::Side side) {
    std::optional<std::string_view> boundary = contentType.getParameter("boundary");
    if(!boundary) {
      throw std::invalid_argument("missing boundary");
    }
    value = std::string(*boundary) + ' ' + (side == anch::codec::Side::SERVER ? "server" : "client") + ' '
      + std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
  }
};
static_assert(!anch::codec::Serializer<Bounded, int> && !anch::codec::Deserializer<Bounded, int>);
// Codec with media type parameters -

// Form mapping +
struct UploadPhoto {
  std::string description;
  std::optional<uint32_t> age;
  std::vector<std::string> tags;
  anch::rest::Part photo;
  std::optional<Pet> pet;
};

template<>
struct anch::rest::FormMapper<UploadPhoto> {
  static void
  toForm(const UploadPhoto& value, anch::rest::Form& form, anch::codec::Side side) {
    anch::rest::addFormField(form, "description", value.description);
    anch::rest::addFormField(form, "age", value.age);
    anch::rest::addFormField(form, "tags", value.tags);
    anch::rest::addFormFile(form, "photo", value.photo);
    if(value.pet) {
      anch::rest::addJsonFormField(form, "pet", *value.pet, side);
    }
  }

  static void
  fromForm(UploadPhoto& value, const anch::rest::Form& form, anch::codec::Side side) {
    value.description = anch::rest::requiredFormField<std::string>(form, "description");
    value.age = anch::rest::formField<uint32_t>(form, "age");
    value.tags = anch::rest::formFields<std::string>(form, "tags");
    value.photo = anch::rest::requiredFormFile(form, "photo");
    value.pet = anch::rest::jsonFormField<Pet>(form, "pet", side);
  }
};
// Form mapping -

template<>
struct anch::rest::FormMapper<Owner> {
  static void
  toForm(const Owner& value, anch::rest::Form& form, anch::codec::Side) {
    anch::rest::addFormField(form, "name", value.name);
    anch::rest::addFormField(form, "email", value.email);
  }

  static void
  fromForm(Owner& value, const anch::rest::Form& form, anch::codec::Side side) {
    value.name = anch::rest::requiredFormField<std::string>(form, "name");
    value.email = anch::rest::formField<std::string>(form, "email");
    anch::rest::checkFormConstraints(value, side);
  }
};

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
  anch::ut::assertThat(response.getHeaders().get("Allow") == "GET, HEAD, POST, OPTIONS", "Unexpected Allow '{}'",
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
  // R4: Content-Length of the GET response
  const std::string length = std::to_string(call(api, Method::GET, "/pets/1").content().size());
  anch::ut::assertThat(response.getHeaders().get("Content-Length") == length, "HEAD Content-Length {} expected, found '{}'",
		       length, response.getHeaders().get("Content-Length").value_or(""));
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
  // Data after the JSON value is refused
  expectStatus(call(api, Method::POST, "/pets", "", "application/json", "{\"id\":7,\"name\":\"Rex\"}{}"), Status::BAD_REQUEST);
  // Request bodies are limited to 2 MiB by default
  anch::ut::assertThat(Json::options(anch::codec::Side::SERVER).max_payload == 2 * 1024 * 1024, "Unexpected server max_payload {}",
		       Json::options(anch::codec::Side::SERVER).max_payload);
  std::string name(2 * 1024 * 1024, 'a');
  expectStatus(call(api, Method::POST, "/pets", "", "application/json", "{\"id\":7,\"name\":\"" + name + "\"}"), Status::CONTENT_TOO_LARGE);
  Json::options(anch::codec::Side::SERVER).max_payload = 0;
  expectStatus(call(api, Method::POST, "/pets", "", "application/json", "{\"id\":7,\"name\":\"" + name + "\"}"), Status::CREATED);
  Json::options(anch::codec::Side::SERVER).max_payload = 2 * 1024 * 1024;
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
  Json::options(anch::codec::Side::SERVER).deserialize_check_constraints = false;
  expectStatus(call(api, Method::POST, "/owners", "", "application/json", R"({"name":""})"), Status::CREATED);
  Json::options(anch::codec::Side::SERVER).deserialize_check_constraints = true;
}

void
testStatusAndQuery() {
  Api api;
  api.post("/pets", [](Body<Pet> pet) { return *pet; }).consumes<Json>().produces<Json>().status(Status::CREATED);
  api.del("/pets/{id}", [](uint64_t) {}).status(Status::ACCEPTED);
  api.get("/pets/{id}", findPet).produces<Json>().status(Status::NON_AUTHORITATIVE_INFORMATION);
  api.get("/search", [](const Request& request) {
    std::optional<int32_t> limit = anch::rest::queryParameter<int32_t>(request, "limit");
    std::string name = anch::rest::requiredQueryParameter<std::string>(request, "name");
    return Response().body(name + " " + (limit ? std::to_string(*limit) : std::string("-")));
  });
  Response created = call(api, Method::POST, "/pets", "application/json", "application/json", R"({"id":7,"name":"Rex"})");
  expectStatus(created, Status::CREATED);
  anch::ut::assertThat(bodyOf(created) == R"({"id":7,"name":"Rex"})", "Unexpected body '{}'", bodyOf(created));
  expectStatus(call(api, Method::DELETE, "/pets/7"), Status::ACCEPTED);
  expectStatus(call(api, Method::GET, "/pets/1", "application/json"), Status::NON_AUTHORITATIVE_INFORMATION);
  expectStatus(call(api, Method::GET, "/pets/2", "application/json"), Status::NOT_FOUND); // empty optional is still 404
  anch::ut::assertThat(bodyOf(call(api, Method::GET, "/search?name=Rex%20Jr&limit=3")) == "Rex Jr 3", "Unexpected search");
  anch::ut::assertThat(bodyOf(call(api, Method::GET, "/search?name=Rex")) == "Rex -", "Optional parameter has to be empty");
  Response missing = call(api, Method::GET, "/search?limit=3");
  expectStatus(missing, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(missing) == "Missing query parameter 'name'", "Unexpected message '{}'", bodyOf(missing));
  Response invalid = call(api, Method::GET, "/search?name=Rex&limit=x");
  expectStatus(invalid, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(invalid) == "Invalid query parameter 'limit': 'x'", "Unexpected message '{}'", bodyOf(invalid));
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

std::string
headerOf(const Response& response, std::string_view name) {
  return response.getHeaders().format(name).value_or("<none>");
}

void
testOptions() {
  Api api;
  api.get("/pets", []() { return Response(); });
  api.post("/pets", []() { return Response(Status::CREATED); });
  api.del("/pets/{id}", [](uint64_t) {});
  api.route(Method::OPTIONS, "/owners", []() { return Response().header("Allow", "custom"); });
  Response pets = call(api, Method::OPTIONS, "/pets");
  expectStatus(pets, Status::NO_CONTENT);
  anch::ut::assertThat(headerOf(pets, "Allow") == "GET, HEAD, POST, OPTIONS", "Unexpected Allow '{}'", headerOf(pets, "Allow"));
  anch::ut::assertThat(!pets.hasBody(), "OPTIONS response has no body");
  Response pet = call(api, Method::OPTIONS, "/pets/12");
  anch::ut::assertThat(headerOf(pet, "Allow") == "DELETE, OPTIONS", "Unexpected Allow '{}'", headerOf(pet, "Allow"));
  Response owners = call(api, Method::OPTIONS, "/owners");
  expectStatus(owners, Status::OK);
  anch::ut::assertThat(headerOf(owners, "Allow") == "custom", "OPTIONS endpoint has to be used");
  expectStatus(call(api, Method::OPTIONS, "/unknown"), Status::NOT_FOUND);
  Response any = call(api, Method::OPTIONS, "*");
  expectStatus(any, Status::NO_CONTENT);
  anch::ut::assertThat(headerOf(any, "Allow") == "<none>", "OPTIONS * has no Allow");
  expectStatus(call(api, Method::GET, "*"), Status::NOT_FOUND);
}

Response
corsCall(Api& api, Method method, std::string_view target, std::initializer_list<std::pair<std::string_view, std::string_view>> headers) {
  Request request(method, target);
  for(const auto& [name, value] : headers) {
    request.getHeaders().add(name, value);
  }
  return api.handle(request);
}

void
testCors() {
  Api api;
  api.get("/pets", []() { return Response().header("Location", "/pets/1"); });
  api.put("/pets/{id}", [](uint64_t) {});
  api.get("/crash", []() -> Response { throw std::runtime_error("crash"); });
  api.filter([](Request& request, const anch::rest::Next& next) {
    if(!request.getHeaders().has("Authorization")) {
      return Response(Status::UNAUTHORIZED);
    }
    return next(request);
  });
  api.cors({
      .origins = {"https://app.example.com", "https://admin.example.com"},
      .headers = {"Content-Type", "Authorization"},
      .exposedHeaders = {"Location", "X-Request-ID"},
      .credentials = true,
      .maxAge = std::chrono::minutes(10)
    });
  const std::string_view origin = "https://app.example.com";

  // Preflight: answered before filters (no Authorization)
  Response preflight = corsCall(api, Method::OPTIONS, "/pets/12", {{"Origin", origin}, {"Access-Control-Request-Method", "PUT"},
								  {"Access-Control-Request-Headers", "content-type, authorization"}});
  expectStatus(preflight, Status::NO_CONTENT);
  anch::ut::assertThat(headerOf(preflight, "Access-Control-Allow-Origin") == origin, "Unexpected origin '{}'", headerOf(preflight, "Access-Control-Allow-Origin"));
  anch::ut::assertThat(headerOf(preflight, "Access-Control-Allow-Methods") == "PUT, OPTIONS", "Unexpected methods '{}'", headerOf(preflight, "Access-Control-Allow-Methods"));
  anch::ut::assertThat(headerOf(preflight, "Access-Control-Allow-Headers") == "content-type, authorization", "Unexpected headers '{}'", headerOf(preflight, "Access-Control-Allow-Headers"));
  anch::ut::assertThat(headerOf(preflight, "Access-Control-Allow-Credentials") == "true", "Credentials have to be allowed");
  anch::ut::assertThat(headerOf(preflight, "Access-Control-Max-Age") == "600", "Unexpected max age '{}'", headerOf(preflight, "Access-Control-Max-Age"));
  anch::ut::assertThat(headerOf(preflight, "Vary") == "Origin, Access-Control-Request-Method, Access-Control-Request-Headers", "Unexpected Vary '{}'", headerOf(preflight, "Vary"));
  Response noHeaders = corsCall(api, Method::OPTIONS, "/pets", {{"Origin", origin}, {"Access-Control-Request-Method", "GET"}});
  expectStatus(noHeaders, Status::NO_CONTENT);
  anch::ut::assertThat(headerOf(noHeaders, "Access-Control-Allow-Headers") == "<none>", "No requested header, no allowed header");

  // Refused preflights
  auto refused = [&api](std::string_view target, std::string_view from, std::string_view method, std::string_view headers) {
    Response response = corsCall(api, Method::OPTIONS, target, {{"Origin", from}, {"Access-Control-Request-Method", method},
								{"Access-Control-Request-Headers", headers}});
    expectStatus(response, Status::FORBIDDEN);
    anch::ut::assertThat(headerOf(response, "Access-Control-Allow-Origin") == "<none>", "Refused preflight has no CORS headers");
  };
  refused("/pets/12", "https://evil.example.com", "PUT", "");
  refused("/pets/12", origin, "POST", "");
  refused("/pets/12", origin, "BREW", "");
  refused("/pets/12", origin, "PUT", "Content-Type, X-Secret");
  expectStatus(corsCall(api, Method::OPTIONS, "/unknown", {{"Origin", origin}, {"Access-Control-Request-Method", "GET"}}), Status::NOT_FOUND);

  // OPTIONS without Access-Control-Request-Method is not a preflight: filters are executed
  expectStatus(corsCall(api, Method::OPTIONS, "/pets", {{"Origin", origin}}), Status::UNAUTHORIZED);

  // Actual requests: CORS headers on success and on errors
  for(const auto& [target, status] : std::initializer_list<std::pair<std::string_view, Status>>{
      {"/pets", Status::OK}, {"/unknown", Status::NOT_FOUND}, {"/crash", Status::INTERNAL_SERVER_ERROR}}) {
    Response response = corsCall(api, Method::GET, target, {{"Origin", "https://admin.example.com"}, {"Authorization", "Bearer token"}});
    expectStatus(response, status);
    anch::ut::assertThat(headerOf(response, "Access-Control-Allow-Origin") == "https://admin.example.com", "{}: unexpected origin '{}'",
			 target, headerOf(response, "Access-Control-Allow-Origin"));
    anch::ut::assertThat(headerOf(response, "Access-Control-Expose-Headers") == "Location, X-Request-ID", "{}: unexpected exposed headers '{}'",
			 target, headerOf(response, "Access-Control-Expose-Headers"));
    anch::ut::assertThat(headerOf(response, "Access-Control-Allow-Credentials") == "true", "{}: credentials have to be allowed", target);
    anch::ut::assertThat(headerOf(response, "Vary") == "Origin", "{}: unexpected Vary '{}'", target, headerOf(response, "Vary"));
  }
  Response unauthorized = corsCall(api, Method::GET, "/pets", {{"Origin", origin}});
  expectStatus(unauthorized, Status::UNAUTHORIZED);
  anch::ut::assertThat(headerOf(unauthorized, "Access-Control-Allow-Origin") == origin, "Filter response has to get CORS headers");
  Response evil = corsCall(api, Method::GET, "/pets", {{"Origin", "https://evil.example.com"}, {"Authorization", "Bearer token"}});
  expectStatus(evil, Status::OK);
  anch::ut::assertThat(headerOf(evil, "Access-Control-Allow-Origin") == "<none>" && headerOf(evil, "Vary") == "Origin",
		       "Not allowed origin gets no CORS headers");
  Response sameOrigin = corsCall(api, Method::GET, "/pets", {{"Authorization", "Bearer token"}});
  anch::ut::assertThat(headerOf(sameOrigin, "Access-Control-Allow-Origin") == "<none>", "Request without origin gets no CORS headers");

  // Every origin and every header
  Api open;
  open.post("/pets", []() { return Response(Status::CREATED); });
  open.cors({.origins = {"*"}, .headers = {"*"}});
  Response openPreflight = corsCall(open, Method::OPTIONS, "/pets", {{"Origin", "https://any.example.com"}, {"Access-Control-Request-Method", "POST"},
								    {"Access-Control-Request-Headers", "X-Custom"}});
  expectStatus(openPreflight, Status::NO_CONTENT);
  anch::ut::assertThat(headerOf(openPreflight, "Access-Control-Allow-Origin") == "*", "Every origin has to be allowed");
  anch::ut::assertThat(headerOf(openPreflight, "Access-Control-Allow-Headers") == "X-Custom", "Requested headers have to be allowed");
  anch::ut::assertThat(headerOf(openPreflight, "Access-Control-Allow-Credentials") == "<none>" && headerOf(openPreflight, "Access-Control-Max-Age") == "<none>",
		       "No credentials and no max age by default");
  Response created = corsCall(open, Method::POST, "/pets", {{"Origin", "null"}});
  expectStatus(created, Status::CREATED);
  anch::ut::assertThat(headerOf(created, "Access-Control-Allow-Origin") == "*" && headerOf(created, "Vary") == "<none>",
		       "'*' origin does not vary");

  // Invalid options
  auto invalid = [](anch::rest::CorsOptions options) {
    Api other;
    try {
      other.cors(std::move(options));
    } catch(const std::invalid_argument&) {
      return true;
    }
    return false;
  };
  anch::ut::assertThat(invalid({}), "Origins are required");
  anch::ut::assertThat(invalid({.origins = {"*"}, .credentials = true}), "Credentials are not allowed with '*'");
}

void
testCodecMediaType() {
  Api api;
  api.post("/echo", [](Body<std::string> body) { return *body; }).consumes<Bounded>().produces<Bounded>();
  api.validate();
  Response response = call(api, Method::POST, "/echo", "multipart/x-test", "multipart/x-test; boundary=xyz", "hello");
  expectStatus(response, Status::OK);
  // Deserialized value is "xyz server hello" (16 characters): its content type is computed before it is written
  std::optional<anch::codec::MediaType> type = anch::codec::MediaType::parse(headerOf(response, "Content-Type"));
  anch::ut::assertThat(type && type->essence() == "multipart/x-test" && type->getParameter("boundary") == "b16",
		       "Unexpected content type '{}'", headerOf(response, "Content-Type"));
  anch::ut::assertThat(bodyOf(response) == "--b16 server xyz server hello", "Unexpected body '{}'", bodyOf(response));
  Response missing = call(api, Method::POST, "/echo", "", "multipart/x-test", "hello");
  expectStatus(missing, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(missing) == "missing boundary", "Unexpected body '{}'", bodyOf(missing));
}

void
testForm() {
  Api api;
  api.post("/pets", [](Body<anch::rest::Form> form) {
    const std::string name = anch::rest::requiredFormField<std::string>(*form, "name");
    const std::optional<uint32_t> age = anch::rest::formField<uint32_t>(*form, "age");
    return name + ' ' + (age ? std::to_string(*age) : std::string("?")) + ' ' + std::to_string(form->fields("tag").size());
  }).consumes<anch::rest::FormUrlEncoded>().produces<Text>();
  api.get("/search", []() { return anch::rest::Form().add("q", "a b").add("page", "2"); }).produces<anch::rest::FormUrlEncoded>();
  api.validate();
  const std::string_view form = "application/x-www-form-urlencoded";
  Response created = call(api, Method::POST, "/pets", "", form, "name=M%C3%A9dor&age=3&tag=a&tag=b");
  expectStatus(created, Status::OK);
  anch::ut::assertThat(bodyOf(created) == "Médor 3 2", "Unexpected body '{}'", bodyOf(created));
  anch::ut::assertThat(bodyOf(call(api, Method::POST, "/pets", "", form, "name=Rex")) == "Rex ? 0", "Optional field has to be empty");
  Response missing = call(api, Method::POST, "/pets", "", form, "age=3");
  expectStatus(missing, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(missing) == "Missing form field 'name'", "Unexpected body '{}'", bodyOf(missing));
  Response invalid = call(api, Method::POST, "/pets", "", form, "name=Rex&age=old");
  expectStatus(invalid, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(invalid) == "Invalid form field 'age': 'old'", "Unexpected body '{}'", bodyOf(invalid));
  expectStatus(call(api, Method::POST, "/pets", "", form, "name=%zz"), Status::BAD_REQUEST);
  expectStatus(call(api, Method::POST, "/pets", "", "application/x-www-form-urlencoded; charset=latin1", "name=Rex"), Status::BAD_REQUEST);
  expectStatus(call(api, Method::POST, "/pets", "", "multipart/form-data; boundary=x", "name=Rex"), Status::UNSUPPORTED_MEDIA_TYPE);
  Response search = call(api, Method::GET, "/search");
  anch::ut::assertThat(headerOf(search, "Content-Type") == form && bodyOf(search) == "q=a+b&page=2", "Unexpected form response '{}'", bodyOf(search));
}

void
testMultipart() {
  Api api;
  api.post("/pets/{id}/photo", [](uint64_t id, Body<anch::rest::Form> form) {
    const anch::rest::Part* photo = form->part("photo");
    if(photo == nullptr || !photo->isFile()) {
      throw HttpException(Status::BAD_REQUEST, "Missing photo");
    }
    return std::to_string(id) + ' ' + std::string(form->field("description").value_or("")) + ' ' + std::string(*photo->getFilename()) + ' '
      + photo->getMediaType().essence() + ' ' + std::to_string(photo->size());
  }).consumes<anch::rest::Multipart>().consumes<anch::rest::FormUrlEncoded>().produces<Text>();
  api.get("/pets/{id}/card", [](uint64_t id) {
    return anch::rest::Form().add("id", std::to_string(id)).addFile("card", "card.txt", anch::codec::MediaType("text", "plain"), "Rex");
  }).produces<anch::rest::Multipart>();
  api.validate();
  const std::string body = "--b\r\nContent-Disposition: form-data; name=\"description\"\r\n\r\nRex\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"rex.png\"\r\nContent-Type: image/png\r\n\r\nPNG...\r\n--b--\r\n";
  Response uploaded = call(api, Method::POST, "/pets/1/photo", "", "multipart/form-data; boundary=b", body);
  expectStatus(uploaded, Status::OK);
  anch::ut::assertThat(bodyOf(uploaded) == "1 Rex rex.png image/png 6", "Unexpected body '{}'", bodyOf(uploaded));
  Response noBoundary = call(api, Method::POST, "/pets/1/photo", "", "multipart/form-data", body);
  expectStatus(noBoundary, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(noBoundary) == "Missing multipart boundary", "Unexpected body '{}'", bodyOf(noBoundary));
  expectStatus(call(api, Method::POST, "/pets/1/photo", "", "application/x-www-form-urlencoded", "description=Rex"), Status::BAD_REQUEST);

  // Limits: 413
  anch::rest::FormLimits& limits = anch::rest::Multipart::limits(anch::codec::Side::SERVER);
  const anch::rest::FormLimits saved = limits;
  limits.maxPartSize = 4;
  Response tooLarge = call(api, Method::POST, "/pets/1/photo", "", "multipart/form-data; boundary=b", body);
  limits = saved;
  expectStatus(tooLarge, Status::CONTENT_TOO_LARGE);
  anch::ut::assertThat(bodyOf(tooLarge) == "Multipart part is larger than 4 bytes", "Unexpected body '{}'", bodyOf(tooLarge));

  // Multipart response: boundary of Content-Type is the one of the body
  Response card = call(api, Method::GET, "/pets/7/card");
  std::optional<anch::codec::MediaType> type = anch::codec::MediaType::parse(headerOf(card, "Content-Type"));
  anch::ut::assertThat(type && type->essence() == "multipart/form-data" && type->getParameter("boundary"), "Unexpected content type '{}'",
		       headerOf(card, "Content-Type"));
  std::istringstream input(bodyOf(card));
  anch::rest::Form form;
  anch::rest::Multipart::deserialize(form, input, *type, anch::codec::Side::CLIENT);
  anch::ut::assertThat(form.field("id") == "7" && form.part("card")->getFilename() == "card.txt", "Unexpected multipart response");
}

void
testFormMapper() {
  Api api;
  api.post("/pets/{id}/photo", [](uint64_t id, Body<UploadPhoto> upload) {
    return std::to_string(id) + ' ' + upload->description + ' ' + (upload->age ? std::to_string(*upload->age) : std::string("?")) + ' '
      + std::to_string(upload->tags.size()) + ' ' + std::string(upload->photo.getFilename().value_or("-")) + ' '
      + std::to_string(upload->photo.size()) + ' ' + (upload->pet ? upload->pet->name : std::string("-"));
  }).consumes<anch::rest::Multipart>().consumes<anch::rest::FormUrlEncoded>().produces<Text>();
  api.post("/owners", [](Body<Owner> owner) { return owner->name; }).consumes<anch::rest::FormUrlEncoded>().produces<Text>();
  api.get("/photo", []() {
    return UploadPhoto{"Rex", 3, {"a", "b"}, anch::rest::Part("ignored", "rex.png", anch::codec::MediaType("image", "png"), "PNG"), Pet{1, "Rex"}};
  }).produces<anch::rest::Multipart>();
  api.validate();
  const std::string body = "--b\r\nContent-Disposition: form-data; name=\"description\"\r\n\r\nRex\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"tags\"\r\n\r\na\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"tags\"\r\n\r\nb\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"pet\"\r\nContent-Type: application/json\r\n\r\n{\"id\":2,\"name\":\"Médor\"}\r\n"
    "--b\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"rex.png\"\r\nContent-Type: image/png\r\n\r\nPNG...\r\n--b--\r\n";
  Response uploaded = call(api, Method::POST, "/pets/1/photo", "", "multipart/form-data; boundary=b", body);
  anch::ut::assertThat(bodyOf(uploaded) == "1 Rex ? 2 rex.png 6 Médor", "Unexpected body '{}'", bodyOf(uploaded));
  Response urlEncoded = call(api, Method::POST, "/pets/1/photo", "", "application/x-www-form-urlencoded", "description=Rex&age=4&photo=inline");
  anch::ut::assertThat(bodyOf(urlEncoded) == "1 Rex 4 0 - 6 -", "Unexpected body '{}'", bodyOf(urlEncoded));
  Response missing = call(api, Method::POST, "/pets/1/photo", "", "application/x-www-form-urlencoded", "description=Rex");
  expectStatus(missing, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(missing) == "Missing form part 'photo'", "Unexpected body '{}'", bodyOf(missing));
  Response invalidJson = call(api, Method::POST, "/pets/1/photo", "", "multipart/form-data; boundary=b",
			      "--b\r\nContent-Disposition: form-data; name=\"description\"\r\n\r\nRex\r\n"
			      "--b\r\nContent-Disposition: form-data; name=\"photo\"\r\n\r\nP\r\n"
			      "--b\r\nContent-Disposition: form-data; name=\"pet\"\r\n\r\n{\"id\":\r\n--b--");
  expectStatus(invalidJson, Status::BAD_REQUEST);

  // anch-json constraints checked on server side
  expectStatus(call(api, Method::POST, "/owners", "", "application/x-www-form-urlencoded", "name=Vincent"), Status::OK);
  Response invalid = call(api, Method::POST, "/owners", "", "application/x-www-form-urlencoded", "name=");
  expectStatus(invalid, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(invalid).starts_with("/name: "), "Unexpected body '{}'", bodyOf(invalid));

  // Mapped response
  Response photo = call(api, Method::GET, "/photo");
  std::optional<anch::codec::MediaType> type = anch::codec::MediaType::parse(headerOf(photo, "Content-Type"));
  anch::ut::assertThat(type && type->getParameter("boundary"), "Unexpected content type '{}'", headerOf(photo, "Content-Type"));
  std::istringstream input(bodyOf(photo));
  UploadPhoto back;
  anch::rest::Multipart::deserialize(back, input, *type, anch::codec::Side::CLIENT);
  anch::ut::assertThat(back.description == "Rex" && back.age == 3u && back.tags == std::vector<std::string>({"a", "b"})
		       && back.photo.getName() == "photo" && back.photo.getFilename() == "rex.png" && back.pet && back.pet->id == 1,
		       "Unexpected mapped response");
}

void
testRawBody() {
  Api api;
  api.post("/files/{name}", [](const Request& request, std::string name, std::istream& body) {
    std::string content{std::istreambuf_iterator<char>(body), std::istreambuf_iterator<char>()};
    return name + ' ' + std::string(request.getHeaders().get("Content-Type").value_or("-")) + ' ' + content;
  }).produces<Text>();
  api.put("/blobs/{id}", [](uint64_t id, std::istream& body) {
    return Response(Status::CREATED).body(std::to_string(id) + ' '
					  + std::to_string(std::distance(std::istreambuf_iterator<char>(body), std::istreambuf_iterator<char>())));
  });
  api.validate();
  for(const auto& [contentType, expected] : std::initializer_list<std::pair<std::string_view, std::string_view>>{
      {"", "a.png - PNG"}, {"*/*", "a.png */* PNG"}, {"image/png", "a.png image/png PNG"}, {"application/json", "a.png application/json PNG"}}) {
    Response response = call(api, Method::POST, "/files/a.png", "", contentType, "PNG");
    expectStatus(response, Status::OK);
    anch::ut::assertThat(bodyOf(response) == expected, "Unexpected body '{}' for content type '{}'", bodyOf(response), contentType);
  }
  Response empty = call(api, Method::POST, "/files/b");
  anch::ut::assertThat(bodyOf(empty) == "b - ", "Request without body has an empty stream, found '{}'", bodyOf(empty));
  Response blob = call(api, Method::PUT, "/blobs/7", "", "application/octet-stream", std::string(100000, 'x'));
  expectStatus(blob, Status::CREATED);
  anch::ut::assertThat(bodyOf(blob) == "7 100000", "Unexpected body '{}'", bodyOf(blob));
  expectStatus(call(api, Method::PUT, "/blobs/x", "", "application/octet-stream", "x"), Status::BAD_REQUEST);
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
    .add("options", testOptions)
    .add("path-parameters", testPathParameters)
    .add("results", testResults)
    .add("negotiation", testNegotiation)
    .add("body", testBody)
    .add("constraints", testConstraints)
    .add("status-and-query", testStatusAndQuery)
    .add("filters", testFilters)
    .add("cors", testCors)
    .add("codec-media-type", testCodecMediaType)
    .add("form", testForm)
    .add("multipart", testMultipart)
    .add("form-mapper", testFormMapper)
    .add("raw-body", testRawBody)
    .add("errors", testErrors)
    .add("registration", testRegistration)
    .add("validate", testValidate)
    ;
}
