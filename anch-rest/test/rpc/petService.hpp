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
/*
 * Pet store RPC service shared by Protobuf and gRPC tests
 */
#pragma once

#include <string>

#include "pets.pb.h"

#include "rest/httpException.hpp"
#include "rest/rpcService.hpp"

namespace anch::test {

  /*!
   * Pet store service: GetPet (pet 1, NOT_FOUND otherwise, metadata echo), AddPet (HttpException for
   * empty name, std::runtime_error for name "crash")
   */
  inline anch::rest::RpcService
  petService() {
    anch::rest::RpcService service("anch.test.pets.PetStore");
    service.unary<anch::test::pets::GetPetRequest, anch::test::pets::Pet>("GetPet", [](const anch::test::pets::GetPetRequest& request,
										       anch::rest::RpcContext& context) {
      if(request.id() != 1) {
	throw anch::rest::RpcError(anch::rest::RpcCode::NOT_FOUND, "No pet " + std::to_string(request.id()));
      }
      anch::test::pets::Pet pet;
      pet.set_id(1);
      pet.set_name("Rex");
      pet.add_tags("dog");
      pet.add_tags(std::string(context.getRequestMetadata().get("x-user").value_or("anonymous")));
      context.getResponseMetadata().add("X-Served-By", "anch");
      context.getResponseMetadata().add("X-Trace", "a"); // multi-valued metadata (R2)
      context.getResponseMetadata().add("X-Trace", "b");
      return pet;
    });
    service.unary<anch::test::pets::Pet, anch::test::pets::Pet>("AddPet", [](const anch::test::pets::Pet& pet) {
      if(pet.name().empty()) {
	throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, "Name is required");
      }
      if(pet.name() == "crash") {
	throw std::runtime_error("secret detail");
      }
      anch::test::pets::Pet saved = pet;
      saved.set_id(42);
      return saved;
    });
    return service;
  }

}
