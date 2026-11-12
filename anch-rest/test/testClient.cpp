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
#include "rest/auth.hpp"
#include "rest/client.hpp"
#include "rest/jsonCodec.hpp"

#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Client;
using anch::rest::HttpError;
using anch::rest::Json;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::rest::Text;

// Fake transport +
namespace {

  /*! Transport which records requests and answers a prepared response */
  class FakeTransport: public anch::rest::Transport {
  public:
    std::string url;
    Method method = Method::GET;
    anch::rest::Headers headers;
    std::string body;
    Response response;

    virtual Response send(const std::string& requestUrl, const Request& request) override {
      url = requestUrl;
      method = request.getMethod();
      headers = request.getHeaders();
      body = request.hasBody() ? std::string(std::istreambuf_iterator<char>(request.getBody()), std::istreambuf_iterator<char>()) : std::string();
      return response;
    }
  };

  /*! Client and its transport */
  struct Fixture {
    FakeTransport* transport;
    Client client;

    explicit Fixture(std::string_view baseUrl = "http://localhost:8080/api/"):
      transport(new FakeTransport()),
      client(std::unique_ptr<anch::rest::Transport>(transport), baseUrl) {
    }
  };

  template<typename E, typename F>
  bool
  throws(F function) {
    try {
      function();
    } catch(const E&) {
      return true;
    }
    return false;
  }

}
// Fake transport -

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

void
testFormatPath() {
  anch::ut::assertThat(Client::formatPath("/owners/{owner}/pets/{id}", {"a b/c", "42"}) == "/owners/a%20b%2Fc/pets/42",
		       "Unexpected path '{}'", Client::formatPath("/owners/{owner}/pets/{id}", {"a b/c", "42"}));
  anch::ut::assertThat(throws<std::invalid_argument>([]() { Client::formatPath("/pets/{id}", {}); }), "Missing parameter has to be refused");
  anch::ut::assertThat(throws<std::invalid_argument>([]() { Client::formatPath("/pets", {"1"}); }), "Extra parameter has to be refused");
  anch::ut::assertThat(throws<std::invalid_argument>([]() { Client::formatPath("/pets/{id", {"1"}); }), "Unclosed placeholder has to be refused");
}

void
testUrl() {
  Fixture fixture;
  fixture.client.get("/pets/{id}", 42).query("name", "Rex & co").query("limit", 10).query("all", true).send();
  anch::ut::assertThat(fixture.transport->url == "http://localhost:8080/api/pets/42?name=Rex%20%26%20co&limit=10&all=true",
		       "Unexpected URL '{}'", fixture.transport->url);
  fixture.client.del("pets").send();
  anch::ut::assertThat(fixture.transport->url == "http://localhost:8080/api/pets" && fixture.transport->method == Method::DELETE,
		       "Unexpected URL '{}'", fixture.transport->url);
}

void
testHeaders() {
  Fixture fixture;
  fixture.client.header("X-Tenant", "anch");
  fixture.client.get("/pets").send();
  anch::ut::assertThat(fixture.transport->headers.get("User-Agent") == "anch-rest" && fixture.transport->headers.get("X-Tenant") == "anch",
		       "Default headers have to be sent");
  fixture.client.get("/pets").header("user-agent", "test").accept<Json>().accept<Text>().send();
  anch::ut::assertThat(fixture.transport->headers.get("User-Agent") == "test", "Request header has to override default header");
  anch::ut::assertThat(fixture.transport->headers.format("Accept") == "application/json, text/plain", "Unexpected Accept '{}'",
		       fixture.transport->headers.format("Accept").value_or(""));
}

void
testBody() {
  Fixture fixture;
  fixture.client.post("/pets").body<Json>(Pet{7, "Rex"}).send();
  anch::ut::assertThat(fixture.transport->body == "{\"id\":7,\"name\":\"Rex\"}" && fixture.transport->headers.get("Content-Type") == "application/json",
		       "Unexpected body '{}'", fixture.transport->body);
}

void
testAs() {
  Fixture fixture;
  fixture.transport->response = Response().contentType("application/json; charset=utf-8").body("{\"id\":1,\"name\":\"Rex\"}");
  Pet pet = fixture.client.get("/pets/{id}", 1).accept<Text>().accept<Json>().as<Pet>();
  anch::ut::assertThat(pet.id == 1 && pet.name == "Rex", "Unexpected pet");
  fixture.transport->response = Response().contentType("text/plain").body("plop");
  anch::ut::assertThat(fixture.client.get("/pets/1").accept<Json>().accept<Text>().as<std::string>() == "plop", "text/plain has to be read with Text");
  anch::ut::assertThat(throws<std::runtime_error>([&]() { fixture.client.get("/pets/1").accept<Json>().as<Pet>(); }),
		       "Not accepted content type has to be refused");
}

void
testHttpError() {
  Fixture fixture;
  fixture.transport->response = Response(Status::NOT_FOUND).contentType("text/plain").body("no pet");
  try {
    fixture.client.get("/pets/{id}", 2).accept<Json>().as<Pet>();
  } catch(const HttpError& e) {
    anch::ut::assertThat(e.getStatus() == Status::NOT_FOUND && e.getResponse().content() == "no pet", "Error response has to be kept");
    anch::ut::assertThat(std::string(e.what()) == "HTTP 404 Not Found", "Unexpected message '{}'", e.what());
    anch::ut::assertThat(fixture.client.get("/pets/2").send().getStatus() == Status::NOT_FOUND, "send() must not throw");
    anch::ut::assertThat(throws<HttpError>([&]() { fixture.client.del("/pets/2").execute(); }), "execute() has to throw");
    return;
  }
  anch::ut::assertThat(false, "HttpError expected");
}

void
testFilters() {
  Fixture fixture;
  std::vector<std::string> calls;
  fixture.client.filter([&calls](Request& request, const anch::rest::Next& next) {
    calls.push_back("first");
    Response response = next(request);
    response.header("X-Filtered", "1");
    return response;
  });
  fixture.client.filter(anch::rest::bearerAuth("token"));
  fixture.client.filter([&calls](Request& request, const anch::rest::Next& next) {
    calls.push_back("last " + std::string(request.getHeaders().get("Authorization").value_or("")));
    return next(request);
  });
  Response response = fixture.client.get("/pets").send();
  anch::ut::assertThat(calls == std::vector<std::string>({"first", "last Bearer token"}), "Filters have to be executed in order");
  anch::ut::assertThat(response.getHeaders().has("X-Filtered"), "Filter has to change response");
}

void
testBase64() {
  anch::ut::assertThat(anch::rest::base64Encode("Aladdin:open sesame") == "QWxhZGRpbjpvcGVuIHNlc2FtZQ==", "Unexpected encoding");
  anch::ut::assertThat(anch::rest::base64Encode("") == "" && anch::rest::base64Encode("f") == "Zg==" && anch::rest::base64Encode("fo") == "Zm8="
		       && anch::rest::base64Encode("foo") == "Zm9v" && anch::rest::base64Encode("\xff\xfe") == "//4=", "Unexpected padding");
  Fixture fixture;
  fixture.client.filter(anch::rest::basicAuth("Aladdin", "open sesame"));
  fixture.client.get("/").send();
  anch::ut::assertThat(fixture.transport->headers.get("Authorization") == "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==", "Unexpected Basic authorization");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST client unit tests")
    .description("Test client requests building with a fake transport")
    .add("format-path", testFormatPath)
    .add("url", testUrl)
    .add("headers", testHeaders)
    .add("body", testBody)
    .add("as", testAs)
    .add("http-error", testHttpError)
    .add("filters", testFilters)
    .add("base64", testBase64)
    ;
}
