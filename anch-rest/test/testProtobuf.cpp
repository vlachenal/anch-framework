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

#include "rpc/petService.hpp"

#include "rest/api.hpp"
#include "rest/body.hpp"
#include "rest/client.hpp"
#include "rest/protobufCodec.hpp"
#include "rest/rpcApi.hpp"
#include "rest/rpcClient.hpp"
#include "rest/transport.hpp"

#ifdef ANCH_REST_TEST_CURL
#include "rest/curlTransport.hpp"
#include "httpTestServer.hpp"
#endif

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Api;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::RpcCode;
using anch::rest::RpcError;
using anch::rest::Status;
using anch::test::pets::GetPetRequest;
using anch::test::pets::Pet;

// Helpers +
namespace {

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

  Response
  post(Api& api, std::string_view target, std::string_view contentType, const std::string& body,
       std::initializer_list<std::pair<std::string_view, std::string_view>> headers = {}) {
    Request request(Method::POST, target);
    if(!contentType.empty()) {
      request.getHeaders().set("Content-Type", contentType);
    }
    for(const auto& [name, value] : headers) {
      request.getHeaders().set(name, value);
    }
    request.setBody(body);
    return api.handle(request);
  }

  void
  expectStatus(const Response& response, Status expected) {
    anch::ut::assertThat(response.getStatus() == expected, "Expected status {}, found {}: {}", anch::rest::code(expected),
			 anch::rest::code(response.getStatus()), response.content());
  }

  std::string
  binary(const google::protobuf::Message& message) {
    return message.SerializeAsString();
  }

  GetPetRequest
  getPet(int64_t id) {
    GetPetRequest request;
    request.set_id(id);
    return request;
  }

  template<typename E, typename F>
  std::optional<E>
  error(F function) {
    try {
      function();
    } catch(const E& e) {
      return e;
    }
    return std::nullopt;
  }

}
// Helpers -

void
testCodecs() {
  Api api;
  api.post("/pets", [](anch::rest::Body<Pet> pet) {
    Pet saved = *pet;
    saved.set_id(7);
    return saved;
  }).consumes<anch::rest::Protobuf>().consumes<anch::rest::ProtobufJson>().produces<anch::rest::Protobuf>().produces<anch::rest::ProtobufJson>();
  api.validate();
  Pet pet;
  pet.set_name("Médor");
  pet.add_tags("dog");
  // Binary
  Request request(Method::POST, "/pets");
  request.getHeaders().set("Content-Type", "application/x-protobuf");
  request.getHeaders().set("Accept", "application/x-protobuf");
  request.setBody(binary(pet));
  Response response = api.handle(request);
  expectStatus(response, Status::OK);
  Pet saved;
  anch::ut::assertThat(saved.ParseFromString(response.content()) && saved.id() == 7 && saved.name() == "Médor" && saved.tags_size() == 1,
		       "Unexpected binary response");
  expectStatus(post(api, "/pets", "application/x-protobuf", "\xff\xff\xff"), Status::BAD_REQUEST);
  // JSON mapping: unknown fields refused on server side
  Request json(Method::POST, "/pets");
  json.getHeaders().set("Content-Type", "application/json");
  json.getHeaders().set("Accept", "application/json");
  json.setBody(R"({"name":"Rex","tags":["a"]})");
  Response jsonResponse = api.handle(json);
  expectStatus(jsonResponse, Status::OK);
  anch::ut::assertThat(jsonResponse.content() == R"({"id":"7","name":"Rex","tags":["a"]})", "Unexpected JSON '{}'", jsonResponse.content());
  Response unknown = post(api, "/pets", "application/json", R"({"name":"Rex","color":"brown"})");
  expectStatus(unknown, Status::BAD_REQUEST);
  // Client side: unknown fields are ignored
  std::istringstream input(R"({"id":"3","name":"Rex","color":"brown"})");
  Pet read;
  anch::rest::ProtobufJson::deserialize(read, input, anch::rest::Side::CLIENT);
  anch::ut::assertThat(read.id() == 3 && read.name() == "Rex", "Client has to ignore unknown fields");
}

void
testConnect() {
  anch::rest::RpcService service = anch::test::petService();
  Api api;
  anch::rest::installRpc(api, service);
  api.validate();
  const std::string path = "/anch.test.pets.PetStore/GetPet";
  // Binary and JSON, metadata
  Response response = post(api, path, "application/proto", binary(getPet(1)), {{"X-User", "vincent"}});
  expectStatus(response, Status::OK);
  Pet pet;
  anch::ut::assertThat(response.getHeaders().get("Content-Type") == "application/proto" && pet.ParseFromString(response.content())
		       && pet.name() == "Rex" && pet.tags(1) == "vincent", "Unexpected Connect response");
  anch::ut::assertThat(response.getHeaders().get("X-Served-By") == "anch", "Response metadata has to be sent in headers");
  Response json = post(api, path, "application/json", R"({"id":"1"})");
  expectStatus(json, Status::OK);
  anch::ut::assertThat(json.content() == R"({"id":"1","name":"Rex","tags":["dog","anonymous"]})", "Unexpected JSON '{}'", json.content());
  // Errors
  Response notFound = post(api, path, "application/proto", binary(getPet(2)));
  expectStatus(notFound, Status::NOT_FOUND);
  anch::ut::assertThat(notFound.getHeaders().get("Content-Type") == "application/json"
		       && notFound.content() == R"({"code":"not_found","message":"No pet 2"})", "Unexpected error '{}'", notFound.content());
  Pet empty;
  Response invalid = post(api, "/anch.test.pets.PetStore/AddPet", "application/proto", binary(empty));
  expectStatus(invalid, Status::BAD_REQUEST);
  anch::ut::assertThat(invalid.content() == R"({"code":"invalid_argument","message":"Name is required"})", "Unexpected error '{}'", invalid.content());
  Pet crash;
  crash.set_name("crash");
  Response internal = post(api, "/anch.test.pets.PetStore/AddPet", "application/proto", binary(crash));
  expectStatus(internal, Status::INTERNAL_SERVER_ERROR);
  anch::ut::assertThat(internal.content() == R"({"code":"internal"})", "Internal errors must not be exposed: '{}'", internal.content());
  Response garbage = post(api, path, "application/proto", "\xff\xff\xff");
  expectStatus(garbage, Status::BAD_REQUEST);
  anch::ut::assertThat(garbage.content().starts_with(R"({"code":"invalid_argument")"), "Unexpected error '{}'", garbage.content());
  expectStatus(post(api, path, "text/plain", "x"), Status::UNSUPPORTED_MEDIA_TYPE);
  expectStatus(post(api, path, "", "x"), Status::UNSUPPORTED_MEDIA_TYPE);
  expectStatus(post(api, "/anch.test.pets.PetStore/DeletePet", "application/proto", ""), Status::NOT_FOUND);
  // Maximum message size
  service.setMaxMessageSize(4);
  Pet big;
  big.set_name("a long name");
  Response tooLarge = post(api, "/anch.test.pets.PetStore/AddPet", "application/proto", binary(big));
  expectStatus(tooLarge, Status::TOO_MANY_REQUESTS);
  anch::ut::assertThat(tooLarge.content().starts_with(R"({"code":"resource_exhausted")"), "Unexpected error '{}'", tooLarge.content());
  // Prefix
  Api prefixed;
  anch::rest::installRpc(prefixed, service, "/rpc");
  expectStatus(post(prefixed, "/rpc/anch.test.pets.PetStore/GetPet", "application/proto", binary(getPet(1))), Status::OK);
}

void
testGrpcWeb() {
  anch::rest::RpcService service = anch::test::petService();
  Api api;
  anch::rest::installRpc(api, service);
  const std::string path = "/anch.test.pets.PetStore/GetPet";
  const std::string message = binary(getPet(1));
  Response response = post(api, path, "application/grpc-web+proto", anch::rest::rpc::grpcWebFrame(message, false));
  expectStatus(response, Status::OK);
  const std::string body = response.content();
  anch::ut::assertThat(response.getHeaders().get("Content-Type") == "application/grpc-web+proto" && body.size() > 10
		       && body[0] == '\0', "Unexpected gRPC-Web response");
  const std::size_t size = (static_cast<std::size_t>(static_cast<unsigned char>(body[3])) << 8) | static_cast<unsigned char>(body[4]);
  Pet pet;
  anch::ut::assertThat(pet.ParseFromString(body.substr(5, size)) && pet.name() == "Rex", "Unexpected message frame");
  const std::string trailers = body.substr(5 + size);
  anch::ut::assertThat(trailers == anch::rest::rpc::grpcWebFrame("grpc-status:0\r\n", true), "Unexpected trailers frame");
  // Errors: trailers only, HTTP 200
  Response notFound = post(api, path, "application/grpc-web", anch::rest::rpc::grpcWebFrame(binary(getPet(3)), false));
  expectStatus(notFound, Status::OK);
  anch::ut::assertThat(notFound.content() == anch::rest::rpc::grpcWebFrame("grpc-status:5\r\ngrpc-message:No%20pet%203\r\n", true),
		       "Unexpected error trailers");
  Response malformed = post(api, path, "application/grpc-web", "\x00\x00");
  anch::ut::assertThat(malformed.content().find("grpc-status:3") != std::string::npos, "Malformed frame has to be INVALID_ARGUMENT");
  std::string compressed = anch::rest::rpc::grpcWebFrame(message, false);
  compressed[0] = '\x01';
  anch::ut::assertThat(post(api, path, "application/grpc-web", compressed).content().find("grpc-status:12") != std::string::npos,
		       "Compressed frame has to be UNIMPLEMENTED");
  // JSON
  Response json = post(api, path, "application/grpc-web+json", anch::rest::rpc::grpcWebFrame(R"({"id":"1"})", false));
  anch::ut::assertThat(json.content().find(R"("name":"Rex")") != std::string::npos, "Unexpected gRPC-Web JSON response");
}

void
testClient() {
  anch::rest::RpcService service = anch::test::petService();
  Api api;
  anch::rest::installRpc(api, service);
  anch::rest::Client client(std::make_unique<ApiTransport>(api, "http://localhost"), "http://localhost");
  client.filter([](Request& request, const anch::rest::Next& next) {
    request.getHeaders().set("X-User", "client");
    return next(request);
  });
  Pet pet = anch::rest::callRpc<Pet>(client, "anch.test.pets.PetStore", "GetPet", getPet(1));
  anch::ut::assertThat(pet.name() == "Rex" && pet.tags(1) == "client", "Unexpected pet");
  Pet json = anch::rest::callRpc<Pet>(client, "anch.test.pets.PetStore", "GetPet", getPet(1), anch::rest::RpcFormat::JSON);
  anch::ut::assertThat(json.name() == "Rex", "Unexpected JSON pet");
  std::optional<RpcError> notFound = error<RpcError>([&]() { anch::rest::callRpc<Pet>(client, "anch.test.pets.PetStore", "GetPet", getPet(2)); });
  anch::ut::assertThat(notFound && notFound->getCode() == RpcCode::NOT_FOUND && std::string(notFound->what()) == "No pet 2",
		       "Connect error has to be raised");
  std::optional<RpcError> missing = error<RpcError>([&]() { anch::rest::callRpc<Pet>(client, "anch.test.pets.PetStore", "Unknown", getPet(1)); });
  anch::ut::assertThat(missing && missing->getCode() == RpcCode::NOT_FOUND, "Unknown route is a NOT_FOUND error");
  // Error codes
  anch::ut::assertThat(anch::rest::rpc::parseConnectCode("canceled") == RpcCode::CANCELLED && !anch::rest::rpc::parseConnectCode("ok")
		       && anch::rest::rpc::connectCode(RpcCode::DATA_LOSS) == "data_loss", "Unexpected Connect codes");
  RpcError fromBody = anch::rest::rpc::parseConnectError(Status::BAD_GATEWAY, "<html>proxy error</html>");
  anch::ut::assertThat(fromBody.getCode() == RpcCode::UNAVAILABLE, "Non Connect error has the code of its status (502 is unavailable)");
  RpcError details = anch::rest::rpc::parseConnectError(Status::CONFLICT, R"({"code":"already_exists","message":"Rex","details":[]})");
  anch::ut::assertThat(details.getCode() == RpcCode::ALREADY_EXISTS && std::string(details.what()) == "Rex", "Details have to be ignored");
}

#ifdef ANCH_REST_TEST_CURL
void
testCurl() {
  anch::rest::RpcService service = anch::test::petService();
  Api api;
  anch::rest::installRpc(api, service);
  HttpTestServer server(api);
  anch::rest::Client client(std::make_unique<anch::rest::CurlTransport>(), server.url());
  Pet pet;
  pet.set_name("Médor");
  Pet saved = anch::rest::callRpc<Pet>(client, "anch.test.pets.PetStore", "AddPet", pet);
  anch::ut::assertThat(saved.id() == 42 && saved.name() == "Médor", "Unexpected saved pet");
  std::optional<RpcError> notFound = error<RpcError>([&]() { anch::rest::callRpc<Pet>(client, "anch.test.pets.PetStore", "GetPet", getPet(9)); });
  anch::ut::assertThat(notFound && notFound->getCode() == RpcCode::NOT_FOUND, "Connect error has to be raised through curl");
}
#endif

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST Protocol Buffers tests")
    .description("Test Protobuf codecs, Connect and gRPC-Web RPC")
    .add("codecs", testCodecs)
    .add("connect", testConnect)
    .add("grpc-web", testGrpcWeb)
    .add("client", testClient)
#ifdef ANCH_REST_TEST_CURL
    .add("curl", testCurl)
#endif
    ;
}
