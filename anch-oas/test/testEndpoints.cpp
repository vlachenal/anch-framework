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
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "petstore-client.hpp"
#include "petstore-server.hpp"

#include "json/mappingError.hpp"
#include "rest/api.hpp"
#include "rest/client.hpp"
#include "rest/httpError.hpp"
#include "rest/httpException.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"
#include "rest/transport.hpp"
#include "uuid.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Status;
using petstore::model::AdoptRequest;
using petstore::model::AdoptResponse;
using petstore::model::Pet;
using petstore::model::PetStoreClient;
using petstore::model::PetStoreService;
using petstore::model::Tag;

// Service +
namespace {

  /*! Pet store service implementation (in memory) */
  class Store: public PetStoreService {
  public:
    std::optional<int64_t> deleted;
    std::optional<Tag> updated;
    std::string name = "Rex";

    virtual AdoptResponse adopt(const AdoptRequest& body) override {
      AdoptResponse res;
      res.id = anch::UUID("deb94ebc-be28-4899-981a-29199b7a487d");
      anch::ut::assertThat(body.petId == 4, "Unexpected pet {}", body.petId);
      return res;
    }

    virtual std::vector<Tag> listPets(const std::string& status, const std::optional<int32_t>& limit) override {
      std::vector<Tag> tags;
      for(int64_t i = 0 ; i < limit.value_or(3) ; ++i) {
	Tag tag;
	tag.id = i;
	tag.name = status;
	tags.push_back(tag);
      }
      return tags;
    }

    virtual Tag createTag(const Tag& body) override {
      if(body.name == "dup") {
	throw anch::rest::HttpException(Status::CONFLICT, "Tag already exists");
      }
      Tag tag = body;
      tag.id = 42;
      return tag;
    }

    virtual std::optional<Pet> getPet(int64_t id) override {
      if(id != 1) {
	return std::nullopt;
      }
      Pet pet;
      pet.id = 1;
      pet.name = name;
      pet.status = petstore::model::Status::SOLD;
      return pet;
    }

    virtual void updateTag(int64_t id, const Tag& body) override {
      updated = body;
      updated->id = id;
    }

    virtual void deletePetsById(int64_t id) override {
      deleted = id;
    }

    virtual std::string getPetName([[maybe_unused]] int64_t id) override {
      return name;
    }

    virtual void renamePet([[maybe_unused]] int64_t id, const std::string& body) override {
      name = body;
    }

    virtual Response getPetPhoto([[maybe_unused]] int64_t id) override {
      return Response().contentType("image/png").body("PNG");
    }
  };

  /*! Transport which calls the API directly (no network) */
  class ApiTransport: public anch::rest::Transport {
  private:
    anch::rest::Api& _api;
    std::string _base;

  public:
    ApiTransport(anch::rest::Api& api, std::string base): _api(api), _base(std::move(base)) {
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
  call(anch::rest::Api& api, Method method, std::string_view target, std::string_view contentType = "",
       std::optional<std::string> body = std::nullopt, std::string_view accept = "application/json") {
    Request request(method, target);
    request.getHeaders().set("Accept", accept);
    if(!contentType.empty()) {
      request.getHeaders().set("Content-Type", contentType);
    }
    if(body) {
      request.setBody(*body);
    }
    return api.handle(request);
  }

  void
  expect(const Response& response, Status status, std::string_view content) {
    anch::ut::assertThat(response.getStatus() == status, "Expected status {} ; found {}", anch::rest::code(status), anch::rest::code(response.getStatus()));
    std::string body = response.content();
    anch::ut::assertThat(body.find(content) != std::string::npos, "Expected body containing '{}' ; found '{}'", content, body);
  }

}
// Service -

void
testServer() {
  Store store;
  anch::rest::Api api;
  petstore::model::registerEndpoints(api, store);
  api.validate();
  // Query parameters
  expect(call(api, Method::GET, "/pets?status=AB&limit=1"), Status::OK, R"([{"id":0,"name":"AB"}])");
  expect(call(api, Method::GET, "/pets?limit=1"), Status::BAD_REQUEST, "Missing query parameter 'status'");
  expect(call(api, Method::GET, "/pets?status=AB&limit=x"), Status::BAD_REQUEST, "Invalid query parameter 'limit': 'x'");
  // Body and status
  expect(call(api, Method::POST, "/pets", "application/json", R"({"name":"t"})"), Status::CREATED, R"({"id":42,"name":"t"})");
  expect(call(api, Method::POST, "/pets", "application/json", R"({"name":"dup"})"), Status::CONFLICT, "Tag already exists");
  // Optional result
  expect(call(api, Method::GET, "/pets/1"), Status::OK, R"("id":1,"name":"Rex","nicknames":[],"photos":[],"scores":{},"status":"sold")");
  expect(call(api, Method::GET, "/pets/2"), Status::NOT_FOUND, "");
  expect(call(api, Method::GET, "/pets/a"), Status::BAD_REQUEST, "");
  // Void results
  expect(call(api, Method::PUT, "/pets/3", "application/json", R"({"name":"u"})"), Status::NO_CONTENT, "");
  anch::ut::assertThat(store.updated && store.updated->id == 3 && store.updated->name == "u", "Tag should be updated");
  expect(call(api, Method::DELETE, "/pets/5"), Status::NO_CONTENT, "");
  anch::ut::assertThat(store.deleted == 5, "Pet should be deleted");
  // Text
  expect(call(api, Method::PUT, "/pets/1/name", "text/plain", "Médor"), Status::ACCEPTED, "");
  expect(call(api, Method::GET, "/pets/1/name", "", std::nullopt, "text/plain"), Status::OK, "Médor");
  // Raw response
  Response photo = call(api, Method::GET, "/pets/1/photo", "", std::nullopt, "image/png");
  expect(photo, Status::OK, "PNG");
  anch::ut::assertThat(photo.getHeaders().get("Content-Type") == "image/png", "Unexpected content type");
  // Inline models and their constraints
  expect(call(api, Method::POST, "/adoptions", "application/json", R"({"petId":4})"), Status::CREATED,
	 R"({"id":"deb94ebc-be28-4899-981a-29199b7a487d"})");
  expect(call(api, Method::POST, "/adoptions", "application/json", R"({"petId":0})"), Status::BAD_REQUEST,
	 "/petId: Value 0 is lower than minimum 1");
  expect(call(api, Method::POST, "/adoptions", "application/json", "{}"), Status::BAD_REQUEST, "Missing required field 'petId'");
}

void
testClient() {
  Store store;
  anch::rest::Api api;
  petstore::model::registerEndpoints(api, store);
  anch::rest::Client rest(std::make_unique<ApiTransport>(api, "http://localhost/api"), "http://localhost/api");
  PetStoreClient client(rest);
  // Query parameters
  anch::ut::assertThat(client.listPets("AB").size() == 3 && client.listPets("AB", 1).size() == 1, "Unexpected pets");
  // Body
  Tag tag;
  tag.name = "t";
  Tag created = client.createTag(tag);
  anch::ut::assertThat(created.id == 42 && created.name == "t", "Unexpected created tag");
  tag.name = "dup";
  try {
    client.createTag(tag);
    anch::ut::fail("HttpError expected");
  } catch(const anch::rest::HttpError& error) {
    anch::ut::assertThat(error.getStatus() == Status::CONFLICT && error.getResponse().content() == "Tag already exists", "Unexpected error");
  }
  // Optional result
  std::optional<Pet> pet = client.getPet(1);
  anch::ut::assertThat(pet && pet->name == "Rex" && pet->status == petstore::model::Status::SOLD, "Unexpected pet");
  anch::ut::assertThat(!client.getPet(2).has_value(), "404 should be an empty result");
  // Void results
  tag.name = "u";
  client.updateTag(3, tag);
  anch::ut::assertThat(store.updated && store.updated->id == 3, "Tag should be updated");
  client.deletePetsById(5);
  anch::ut::assertThat(store.deleted == 5, "Pet should be deleted");
  // Text
  client.renamePet(1, "Médor");
  anch::ut::assertThat(client.getPetName(1) == "Médor", "Unexpected name");
  // Raw response
  anch::ut::assertThat(client.getPetPhoto(1).content() == "PNG", "Unexpected photo");
  // Inline models and their constraints: invalid request is not sent
  AdoptRequest adoption;
  adoption.petId = 4;
  anch::ut::assertThat(client.adopt(adoption).id == anch::UUID("deb94ebc-be28-4899-981a-29199b7a487d"), "Unexpected adoption");
  adoption.petId = 0;
  try {
    client.adopt(adoption);
    anch::ut::fail("Invalid request should not be sent");
  } catch(const anch::json::MappingError& error) {
    std::cout << error.what() << std::endl;
    anch::ut::assertThat(error.getPath() == "/petId", "Unexpected path '{}'", error.getPath());
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH OAS generated endpoints unit tests")
    .description("Generated server and client from petstore.json")
    .add("endpoints-server", testServer)
    .add("endpoints-client", testClient)
    ;
}
