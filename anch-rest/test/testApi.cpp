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

// Codec with media type parameters (as a multipart codec with its boundary) +
struct Bounded {
  static std::string_view
  mediaType() noexcept {
    return "multipart/x-test";
  }

  static anch::rest::MediaType
  contentType(const std::string& value) {
    anch::rest::MediaType type("multipart", "x-test");
    type.setParameter("boundary", "b" + std::to_string(value.size()));
    return type;
  }

  static void
  serialize(const std::string& value, std::ostream& out, const anch::rest::MediaType& contentType, anch::rest::Side side) {
    out << "--" << contentType.getParameter("boundary").value_or("") << ' ' << (side == anch::rest::Side::SERVER ? "server" : "client")
	<< ' ' << value;
  }

  static void
  deserialize(std::string& value, std::istream& input, const anch::rest::MediaType& contentType, anch::rest::Side side) {
    std::optional<std::string_view> boundary = contentType.getParameter("boundary");
    if(!boundary) {
      throw std::invalid_argument("missing boundary");
    }
    value = std::string(*boundary) + ' ' + (side == anch::rest::Side::SERVER ? "server" : "client") + ' '
      + std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
  }
};
static_assert(!anch::rest::Serializer<Bounded, int> && !anch::rest::Deserializer<Bounded, int>);
// Codec with media type parameters -

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
  std::optional<anch::rest::MediaType> type = anch::rest::MediaType::parse(headerOf(response, "Content-Type"));
  anch::ut::assertThat(type && type->essence() == "multipart/x-test" && type->getParameter("boundary") == "b16",
		       "Unexpected content type '{}'", headerOf(response, "Content-Type"));
  anch::ut::assertThat(bodyOf(response) == "--b16 server xyz server hello", "Unexpected body '{}'", bodyOf(response));
  Response missing = call(api, Method::POST, "/echo", "", "multipart/x-test", "hello");
  expectStatus(missing, Status::BAD_REQUEST);
  anch::ut::assertThat(bodyOf(missing) == "missing boundary", "Unexpected body '{}'", bodyOf(missing));
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
    .add("errors", testErrors)
    .add("registration", testRegistration)
    .add("validate", testValidate)
    ;
}
