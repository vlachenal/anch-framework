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
#include <memory>

#include "PetStore.h"
#include "rest/api.hpp"
#include "rest/client.hpp"
#include "codec/thrift.hpp"
#include "rest/thriftRpc.hpp"
#include "rest/thriftTransport.hpp"

pets::Pet
save(pets::Pet pet) {
  pet.id = 2;
  return pet;
}

class PetStoreHandler: public pets::PetStoreIf {
public:
  void getPet(pets::Pet& pet, const int64_t id) override {
    if(id != 1) {
      pets::PetNotFound error;
      error.id = id;
      throw error;
    }
    pet.id = 1;
    pet.name = "Rex";
  }
};

bool
server() {
  anch::rest::Api api;
  //! [thrift codec]
  api.post("/pets", [](anch::rest::Body<pets::Pet> pet) { return save(*pet); })
    .consumes<anch::codec::ThriftBinary>().produces<anch::codec::ThriftBinary>();
  //! [thrift codec]
  //! [thrift rpc]
  auto handler = std::make_shared<PetStoreHandler>();          // implements generated PetStoreIf
  anch::rest::installThrift(api, "/thrift/pets", std::make_shared<pets::PetStoreProcessor>(handler)); // rest/thriftRpc.hpp
  //! [thrift rpc]
  api.validate();
  return true;
}

// Not called: the server of the example does not exist
void
client(anch::rest::Client& client) {
  //! [thrift client]
  auto transport = std::make_shared<anch::rest::ThriftTransport>(client, "/thrift/pets");            // rest/thriftTransport.hpp
  pets::PetStoreClient pets(anch::codec::thriftProtocol(anch::codec::ThriftProtocol::BINARY, transport));
  pets::Pet pet;
  pets.getPet(pet, 1); // declared exceptions are raised by the generated client
  //! [thrift client]
}

int
main() {
  return server() ? 0 : 1;
}
