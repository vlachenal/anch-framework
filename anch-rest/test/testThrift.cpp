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
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include <thrift/TApplicationException.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>

#include "PetStore.h"

#include "rest/api.hpp"
#include "rest/body.hpp"
#include "rest/client.hpp"
#include "rest/httpError.hpp"
#include "rest/thriftCodec.hpp"
#include "rest/thriftRpc.hpp"
#include "rest/thriftTransport.hpp"
#include "rest/transport.hpp"

#ifdef ANCH_REST_TEST_CURL
#include <thrift/transport/THttpClient.h>
#include "rest/curlTransport.hpp"
#include "httpTestServer.hpp"
#endif

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Api;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using anch::rest::ThriftProtocol;
using anch::test::thrift::Pet;
using anch::test::thrift::PetNotFound;
using anch::test::thrift::PetStoreClient;
using anch::test::thrift::PetStoreIf;
using anch::test::thrift::PetStoreProcessor;

// Service +
namespace {

  /*! Pet store: pet 1 exists, added pets get id 42 */
  class PetStoreHandler: public PetStoreIf {
  public:
    virtual void
    getPet(Pet& result, const int64_t id) override {
      if(id != 1) {
	PetNotFound notFound;
	notFound.id = id;
	throw notFound;
      }
      result.id = 1;
      result.name = "Rex";
      result.__set_tags({"dog"});
    }

    virtual void
    addPet(Pet& result, const Pet& pet) override {
      result = pet;
      result.id = 42;
    }
  };

  /*! Transport which calls the API directly (no network) */
  class ApiTransport: public anch::rest::Transport {
  private:
    Api& _api;
    std::string _base;

  public:
    ApiTransport(Api& api, std::string base): _api(api), _base(std::move(base)) {
    }

    virtual Response send(const std::string& url, const Request& request) override {
      Request copy(request.getMethod(), std::string_view(url).substr(_base.size()));
      copy.getHeaders() = request.getHeaders();
      if(request.hasBody()) {
	copy.setBody(std::string(std::istreambuf_iterator<char>(request.getBody()), std::istreambuf_iterator<char>()));
      }
      return _api.handle(copy);
    }
  };

  Api
  petApi() {
    Api api;
    anch::rest::installThrift(api, "/thrift/pets", std::make_shared<PetStoreProcessor>(std::make_shared<PetStoreHandler>()));
    return api;
  }

  Pet
  pet(int64_t id, std::string name) {
    Pet res;
    res.id = id;
    res.name = std::move(name);
    return res;
  }

  void
  expectStatus(const Response& response, Status expected) {
    anch::ut::assertThat(response.getStatus() == expected, "Expected status {}, found {}: {}", anch::rest::code(expected),
			 anch::rest::code(response.getStatus()), response.content());
  }

  /*! Call pet store with a generated client over anch-rest */
  void
  checkClient(anch::rest::Client& client, ThriftProtocol protocol) {
    auto transport = std::make_shared<anch::rest::ThriftTransport>(client, "/thrift/pets", protocol);
    PetStoreClient pets(anch::rest::thriftProtocol(protocol, transport));
    Pet found;
    pets.getPet(found, 1);
    anch::ut::assertThat(found.id == 1 && found.name == "Rex" && found.__isset.tags && found.tags.size() == 1, "Unexpected pet");
    Pet added;
    pets.addPet(added, pet(0, "Médor"));
    anch::ut::assertThat(added.id == 42 && added.name == "Médor", "Unexpected added pet");
    bool notFound = false;
    try {
      pets.getPet(found, 2);
    } catch(const PetNotFound& error) {
      notFound = (error.id == 2);
    }
    anch::ut::assertThat(notFound, "Declared exception has to be raised");
  }

}
// Service -

void
testCodecs() {
  Api api;
  api.post("/pets", [](anch::rest::Body<Pet> body) {
    Pet saved = *body;
    saved.id = 7;
    return saved;
  }).consumes<anch::rest::ThriftBinary>().consumes<anch::rest::ThriftCompact>().consumes<anch::rest::ThriftJson>()
    .produces<anch::rest::ThriftBinary>().produces<anch::rest::ThriftCompact>().produces<anch::rest::ThriftJson>();
  api.validate();
  for(ThriftProtocol protocol : {ThriftProtocol::BINARY, ThriftProtocol::COMPACT, ThriftProtocol::JSON}) {
    const std::string_view type = anch::rest::thriftMediaType(protocol);
    std::ostringstream out;
    switch(protocol) {
    case ThriftProtocol::BINARY: anch::rest::ThriftBinary::serialize(pet(0, "Rex"), out); break;
    case ThriftProtocol::COMPACT: anch::rest::ThriftCompact::serialize(pet(0, "Rex"), out); break;
    case ThriftProtocol::JSON: anch::rest::ThriftJson::serialize(pet(0, "Rex"), out); break;
    }
    Request request(Method::POST, "/pets");
    request.getHeaders().set("Content-Type", type);
    request.getHeaders().set("Accept", type);
    request.setBody(out.str());
    Response response = api.handle(request);
    expectStatus(response, Status::OK);
    anch::ut::assertThat(response.getHeaders().get("Content-Type") == type, "Unexpected content type");
    Pet saved;
    std::istringstream input(response.content());
    switch(protocol) {
    case ThriftProtocol::BINARY: anch::rest::ThriftBinary::deserialize(saved, input, anch::rest::Side::CLIENT); break;
    case ThriftProtocol::COMPACT: anch::rest::ThriftCompact::deserialize(saved, input, anch::rest::Side::CLIENT); break;
    case ThriftProtocol::JSON: anch::rest::ThriftJson::deserialize(saved, input, anch::rest::Side::CLIENT); break;
    }
    anch::ut::assertThat(saved.id == 7 && saved.name == "Rex", "Unexpected pet with {}", type);
  }
  // Server side bodies are bounded (anch::rest::maxBodySize): 413 +
  {
    std::ostringstream out;
    anch::rest::ThriftBinary::serialize(pet(0, std::string(3 * 1024 * 1024, 'a')), out);
    Request large(Method::POST, "/pets");
    large.getHeaders().set("Content-Type", "application/vnd.apache.thrift.binary");
    large.getHeaders().set("Accept", "application/vnd.apache.thrift.binary");
    large.setBody(out.str());
    expectStatus(api.handle(large), Status::CONTENT_TOO_LARGE);
    anch::rest::maxBodySize(anch::rest::Side::SERVER) = 0; // no limit
    Request unbounded(Method::POST, "/pets");
    unbounded.getHeaders().set("Content-Type", "application/vnd.apache.thrift.binary");
    unbounded.getHeaders().set("Accept", "application/vnd.apache.thrift.binary");
    unbounded.setBody(out.str());
    Response response = api.handle(unbounded);
    anch::rest::maxBodySize(anch::rest::Side::SERVER) = 2 * 1024 * 1024;
    expectStatus(response, Status::OK);
  }
  // Server side bodies are bounded (anch::rest::maxBodySize): 413 -
  // Invalid structure and missing required field
  Request invalid(Method::POST, "/pets");
  invalid.getHeaders().set("Content-Type", "application/vnd.apache.thrift.json");
  invalid.setBody("{not json");
  expectStatus(api.handle(invalid), Status::BAD_REQUEST);
  Request missing(Method::POST, "/pets");
  missing.getHeaders().set("Content-Type", "application/vnd.apache.thrift.json");
  missing.setBody(R"({"1":{"i64":1}})");
  Response response = api.handle(missing);
  expectStatus(response, Status::BAD_REQUEST);
  anch::ut::assertThat(response.content().starts_with("Invalid Thrift structure"), "Unexpected body '{}'", response.content());
}

void
testRpc() {
  Api api = petApi();
  api.validate();
  anch::rest::Client client(std::make_unique<ApiTransport>(api, "http://localhost"), "http://localhost");
  for(ThriftProtocol protocol : {ThriftProtocol::BINARY, ThriftProtocol::COMPACT, ThriftProtocol::JSON}) {
    checkClient(client, protocol);
  }
  // Unknown method: Thrift application exception
  auto buffer = std::make_shared<apache::thrift::transport::TMemoryBuffer>();
  apache::thrift::protocol::TBinaryProtocol writer(buffer);
  writer.writeMessageBegin("deletePet", apache::thrift::protocol::T_CALL, 1);
  writer.writeStructBegin("args");
  writer.writeFieldStop();
  writer.writeStructEnd();
  writer.writeMessageEnd();
  Request request(Method::POST, "/thrift/pets");
  request.getHeaders().set("Content-Type", "application/x-thrift");
  request.setBody(buffer->getBufferAsString());
  Response response = api.handle(request);
  expectStatus(response, Status::OK);
  const std::string content = response.content();
  std::vector<uint8_t> data(content.begin(), content.end());
  auto input = std::make_shared<apache::thrift::transport::TMemoryBuffer>(data.data(), static_cast<uint32_t>(data.size()));
  apache::thrift::protocol::TBinaryProtocol reader(input);
  std::string name;
  apache::thrift::protocol::TMessageType type;
  int32_t sequence = 0;
  reader.readMessageBegin(name, type, sequence);
  apache::thrift::TApplicationException exception;
  exception.read(&reader);
  anch::ut::assertThat(type == apache::thrift::protocol::T_EXCEPTION && exception.getType() == apache::thrift::TApplicationException::UNKNOWN_METHOD,
		       "Unknown method has to be a Thrift application exception");
  // Errors
  Request text(Method::POST, "/thrift/pets");
  text.getHeaders().set("Content-Type", "text/plain");
  text.setBody("x");
  expectStatus(api.handle(text), Status::UNSUPPORTED_MEDIA_TYPE);
  Request garbage(Method::POST, "/thrift/pets");
  garbage.getHeaders().set("Content-Type", "application/x-thrift");
  garbage.setBody("\x01\x02");
  expectStatus(api.handle(garbage), Status::BAD_REQUEST);
  // HTTP error raised by the transport
  anch::rest::Client wrong(std::make_unique<ApiTransport>(api, "http://localhost"), "http://localhost");
  auto transport = std::make_shared<anch::rest::ThriftTransport>(wrong, "/thrift/unknown");
  PetStoreClient pets(anch::rest::thriftProtocol(ThriftProtocol::BINARY, transport));
  bool httpError = false;
  try {
    Pet found;
    pets.getPet(found, 1);
  } catch(const anch::rest::HttpError& error) {
    httpError = (error.getStatus() == Status::NOT_FOUND);
  }
  anch::ut::assertThat(httpError, "HTTP errors have to be raised by the transport");
}

#ifdef ANCH_REST_TEST_CURL
void
testCurl() {
  Api api = petApi();
  HttpTestServer server(api);
  // Generated client over anch-rest curl transport
  anch::rest::Client client(std::make_unique<anch::rest::CurlTransport>(), server.url());
  checkClient(client, ThriftProtocol::BINARY);
  checkClient(client, ThriftProtocol::COMPACT);
  // Interoperability: Thrift HTTP client
  const std::string url = server.url();
  const std::string authority = url.substr(url.find("//") + 2);
  const std::string host = authority.substr(0, authority.find(':'));
  const int port = std::stoi(authority.substr(authority.find(':') + 1));
  auto http = std::make_shared<apache::thrift::transport::THttpClient>(host, port, "/thrift/pets");
  http->open();
  PetStoreClient pets(std::make_shared<apache::thrift::protocol::TBinaryProtocol>(http));
  Pet found;
  pets.getPet(found, 1);
  anch::ut::assertThat(found.name == "Rex", "Thrift HTTP client has to be served");
  http->close();
}
#endif

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST Thrift tests")
    .description("Test Thrift codecs and RPC over HTTP")
    .add("codecs", testCodecs)
    .add("rpc", testRpc)
#ifdef ANCH_REST_TEST_CURL
    .add("curl", testCurl)
#endif
    ;
}
