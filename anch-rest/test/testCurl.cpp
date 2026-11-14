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
#include "rest/auth.hpp"
#include "rest/client.hpp"
#include "rest/curlTransport.hpp"
#include "rest/httpException.hpp"
#include "rest/jsonCodec.hpp"

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <thread>

#include "network/tcpServer.hpp"

#include "httpTestServer.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Api;
using anch::rest::Body;
using anch::rest::Client;
using anch::rest::CurlTransport;
using anch::rest::HttpError;
using anch::rest::HttpException;
using anch::rest::Json;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::rest::Text;
using anch::rest::TransportConfig;
using anch::rest::TransportError;

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
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Owner>& mapper) {
  mapper
    .registerField("name", &Owner::name, anch::json::required, anch::json::minLength(1))
    ;
}
// Model -

// Server +
namespace {

  std::unique_ptr<Api> api;
  std::unique_ptr<HttpTestServer> server;

  void
  startServer() {
    api = std::make_unique<Api>();
    api->get("/pets/{id}", [](uint64_t id) -> std::optional<Pet> {
      if(id == 1) {
	return Pet{1, "Rex"};
      }
      return std::nullopt;
    }).produces<Json>();
    api->get("/names/{name}", [](const Request& request, std::string name) {
      return "name=" + name + " q=" + std::string(request.getQueryParameter("q").value_or("-"));
    }).produces<Text>();
    api->post("/pets", [](Body<Pet> pet) {
      return Response(Status::CREATED).header("Location", "/pets/" + std::to_string(pet->id)).contentType("text/plain").body(pet->name);
    }).consumes<Json>();
    api->post("/owners", [](Body<Owner> owner) { return Response(Status::CREATED).contentType("text/plain").body(owner->name); }).consumes<Json>();
    api->get("/owners/invalid", []() { return Owner{""}; }).produces<Json>();
    api->del("/pets/{id}", [](uint64_t id) {
      if(id != 1) {
	throw HttpException(Status::NOT_FOUND, "no pet " + std::to_string(id));
      }
    });
    api->get("/slow", []() {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      return Response();
    });
    api->get("/secured", [](const Request& request) {
      return Response().contentType("text/plain").body(std::string(request.getHeaders().get("Authorization").value_or("")));
    });
    api->validate();
    server = std::make_unique<HttpTestServer>(*api);
  }

  void
  stopServer() {
    server.reset();
    api.reset();
  }

  Client
  makeClient(TransportConfig config = TransportConfig()) {
    return Client(std::make_unique<CurlTransport>(config), server->url());
  }

}
// Server -

void
testGetJson() {
  Client client = makeClient();
  Pet pet = client.get("/pets/{id}", 1).accept<Json>().as<Pet>();
  anch::ut::assertThat(pet.id == 1 && pet.name == "Rex", "Unexpected pet {} '{}'", pet.id, pet.name);
  Response text = client.get("/pets/{id}", 1).accept<Text>().send();
  anch::ut::assertThat(text.getStatus() == Status::NOT_ACCEPTABLE, "Expected 406, found {}", anch::rest::code(text.getStatus()));
}

void
testParameters() {
  Client client = makeClient();
  std::string res = client.get("/names/{name}", "Rex / Médor").query("q", "a&b=c").accept<Text>().as<std::string>();
  anch::ut::assertThat(res == "name=Rex / Médor q=a&b=c", "Unexpected parameters '{}'", res);
}

void
testPostJson() {
  Client client = makeClient();
  Response response = client.post("/pets").body<Json>(Pet{7, "Médor"}).send();
  anch::ut::assertThat(response.getStatus() == Status::CREATED, "Expected 201, found {}", anch::rest::code(response.getStatus()));
  anch::ut::assertThat(response.getHeaders().get("location") == "/pets/7" && response.content() == "Médor", "Unexpected response '{}'", response.content());
  Response invalid = client.post("/pets").body("{\"id\":", "application/json").send();
  anch::ut::assertThat(invalid.getStatus() == Status::BAD_REQUEST, "Invalid JSON has to be refused");
}

void
testConstraints() {
  // Server and client in the same process: each one uses its own options
  Client client = makeClient();
  Response created = client.post("/owners").body<Json>(Owner{"Vincent"}).send();
  anch::ut::assertThat(created.getStatus() == Status::CREATED, "Expected 201, found {}", anch::rest::code(created.getStatus()));
  // Client refuses to send invalid request
  bool refused = false;
  try {
    client.post("/owners").body<Json>(Owner{""}).send();
  } catch(const anch::json::MappingError& e) {
    refused = (e.getPath() == "/name");
  }
  anch::ut::assertThat(refused, "Client should refuse invalid request");
  // Server refuses invalid request (sent without client check)
  Response invalid = client.post("/owners").body(R"({"name":""})", "application/json").send();
  anch::ut::assertThat(invalid.getStatus() == Status::BAD_REQUEST && invalid.content().starts_with("/name: Length 0 is lower than minLength 1"),
		       "Unexpected response {} '{}'", anch::rest::code(invalid.getStatus()), invalid.content());
  // Client trusts server response
  Owner owner = client.get("/owners/invalid").accept<Json>().as<Owner>();
  anch::ut::assertThat(owner.name.empty(), "Response should be trusted");
}

void
testHttpError() {
  Client client = makeClient();
  try {
    client.del("/pets/{id}", 3).execute();
  } catch(const HttpError& e) {
    anch::ut::assertThat(e.getStatus() == Status::NOT_FOUND && e.getResponse().content() == "no pet 3",
			 "Unexpected error {} '{}'", anch::rest::code(e.getStatus()), e.getResponse().content());
    client.del("/pets/{id}", 1).execute();
    anch::ut::assertThat(client.get("/pets/{id}", 2).accept<Json>().send().getStatus() == Status::NOT_FOUND, "Expected 404");
    return;
  }
  anch::ut::assertThat(false, "HttpError expected");
}

void
testHead() {
  Client client = makeClient();
  Response response = client.head("/pets/{id}", 1).header("Accept", "application/json").send();
  anch::ut::assertThat(response.getStatus() == Status::OK && response.content().empty()
		       && response.getHeaders().get("Content-Type") == "application/json", "Unexpected HEAD response");
}

void
testAuth() {
  Client client = makeClient();
  client.filter(anch::rest::basicAuth("Aladdin", "open sesame"));
  std::string res = client.get("/secured").accept<Text>().as<std::string>();
  anch::ut::assertThat(res == "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==", "Unexpected authorization '{}'", res);
}

void
testRefused() {
  uint16_t port = 0;
  {
    anch::network::TcpServer closed("127.0.0.1", 0); // free port, closed at end of scope
    port = closed.getAddress().getPort();
  }
  Client client(std::make_unique<CurlTransport>(), "http://127.0.0.1:" + std::to_string(port));
  try {
    client.get("/pets").send();
  } catch(const TransportError& e) {
    anch::ut::assertThat(std::string(e.what()).find("onnect") != std::string::npos, "Unexpected error '{}'", e.what());
    return;
  }
  anch::ut::assertThat(false, "TransportError expected");
}

void
testTimeout() {
  TransportConfig config;
  config.timeout = std::chrono::milliseconds(200);
  Client client = makeClient(config);
  auto start = std::chrono::steady_clock::now();
  try {
    client.get("/slow").send();
  } catch(const TransportError& e) {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    anch::ut::assertThat(elapsed.count() < 900, "Timeout has to be applied ({} ms)", elapsed.count());
    // Wait for slow endpoint end: next tests need the server
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return;
  }
  anch::ut::assertThat(false, "TransportError expected");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST curl transport unit tests")
    .description("Test client with curl transport against a local HTTP server")
    .initialize(startServer)
    .uninitialize(stopServer)
    .add("get-json", testGetJson)
    .add("parameters", testParameters)
    .add("post-json", testPostJson)
    .add("constraints", testConstraints)
    .add("http-error", testHttpError)
    .add("head", testHead)
    .add("auth", testAuth)
    .add("refused", testRefused)
    .add("timeout", testTimeout)
    ;
}
