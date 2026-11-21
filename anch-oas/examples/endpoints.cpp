/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "petstore.hpp"
#include "petstore-client.hpp"
#include "petstore-server.hpp"
#include "rest/api.hpp"
#include "rest/client.hpp"
#include "rest/curlTransport.hpp"
#include "rest/request.hpp"

//! [service]
// Server: implement the service and register its endpoints
class Store: public petstore::model::PetStoreService {
public:
  std::optional<petstore::model::Pet> getPet(int64_t id) override;   // GET /pets/{id} (404 when empty)
  petstore::model::Tag createTag(const petstore::model::Tag& body) override; // POST /pets (201)
  std::vector<petstore::model::Tag> listPets(const std::string& status, const std::optional<int32_t>& limit) override; // GET /pets
};
//! [service]

std::optional<petstore::model::Pet>
Store::getPet(int64_t id) {
  if(id != 1) {
    return std::nullopt;
  }
  return petstore::model::Pet{.id = 1, .name = "Rex", .status = petstore::model::Status::AVAILABLE};
}

petstore::model::Tag
Store::createTag(const petstore::model::Tag& body) {
  petstore::model::Tag tag = body;
  tag.id = 7;
  return tag;
}

std::vector<petstore::model::Tag>
Store::listPets(const std::string&, const std::optional<int32_t>&) {
  return {};
}

bool
server() {
  //! [server]
  Store store;
  anch::rest::Api api;
  petstore::model::registerEndpoints(api, store);
  api.validate();
  //! [server]
  anch::rest::Request request(anch::rest::Method::GET, "/pets/1");
  request.getHeaders().set("Accept", "application/json");
  return anch::rest::code(api.handle(request).getStatus()) == 200;
}

// Not called: the server of the example does not exist
void
client() {
  //! [client]
  // Client: typed calls over an anch-rest client (transport, base URL and filters are configured by the caller)
  anch::rest::Client rest(std::make_unique<anch::rest::CurlTransport>(), "https://api.example.com");
  petstore::model::PetStoreClient client(rest);
  std::optional<petstore::model::Pet> pet = client.getPet(1);
  std::vector<petstore::model::Tag> tags = client.listPets("AB");      // optional query parameters default to std::nullopt
  //! [client]
}

int
main() {
  return server() ? 0 : 1;
}
