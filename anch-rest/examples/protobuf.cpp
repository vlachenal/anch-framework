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
#include <string>

#include "pets.pb.h"
#include "rest/api.hpp"
#include "rest/client.hpp"
#include "codec/protobuf.hpp"
#include "rest/request.hpp"
#include "rest/rpcApi.hpp"
#include "rest/rpcClient.hpp"
#include "rest/rpcService.hpp"

pets::Pet
save(pets::Pet pet) {
  pet.set_id(2);
  return pet;
}

pets::Pet
findPet(int64_t id) {
  pets::Pet pet;
  pet.set_id(id);
  pet.set_name("Rex");
  return pet;
}

bool
server() {
  anch::rest::Api api;
  //! [protobuf codec]
  api.post("/pets", [](anch::rest::Body<pets::Pet> pet) { return save(*pet); })
    .consumes<anch::codec::Protobuf>().produces<anch::codec::Protobuf>();
  //! [protobuf codec]
  //! [rpc service]
  anch::rest::RpcService service("pets.PetStore");
  service.unary<pets::GetPetRequest, pets::Pet>("GetPet", [](const pets::GetPetRequest& request, anch::rest::RpcContext&) {
      if(request.id() != 1) {
        throw anch::rest::RpcError(anch::rest::RpcCode::NOT_FOUND, "No pet"); // Connect, gRPC-Web and gRPC status
      }
      return findPet(request.id());
    });
  anch::rest::installRpc(api, service);       // POST /pets.PetStore/GetPet (Connect, gRPC-Web): rest/rpcApi.hpp
  //! [rpc service]
  api.validate();
  pets::Pet pet;
  pet.set_name("Rex");
  anch::rest::Request request(anch::rest::Method::POST, "/pets");
  request.getHeaders().set("Content-Type", "application/x-protobuf");
  request.getHeaders().set("Accept", "application/x-protobuf");
  request.setBody(pet.SerializeAsString());
  pets::Pet saved;
  return saved.ParseFromString(api.handle(request).content()) && saved.id() == 2;
}

// Not called: the server of the example does not exist
void
client(anch::rest::Client& client) {
  {
    //! [protobuf client]
    pets::Pet pet = client.get("/pets/{id}", 1).accept<anch::codec::Protobuf>().as<pets::Pet>();
    //! [protobuf client]
    static_cast<void>(pet);
  }
  //! [rpc client]
  pets::GetPetRequest request;
  request.set_id(1);
  pets::Pet pet = anch::rest::callRpc<pets::Pet>(client, "pets.PetStore", "GetPet", request); // Connect client: rest/rpcClient.hpp
  //! [rpc client]
  static_cast<void>(pet);
}

int
main() {
  return server() ? 0 : 1;
}
