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
#include <memory>
#include <string>

#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "pets.grpc.pb.h"
#include "rpc/petService.hpp"

#include "rest/api.hpp"
#include "rest/grpcServer.hpp"
#include "rest/rpcApi.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::test::pets::GetPetRequest;
using anch::test::pets::Pet;
using anch::test::pets::PetStore;

namespace {

  std::unique_ptr<PetStore::Stub>
  stub(int port) {
    return PetStore::NewStub(grpc::CreateChannel("127.0.0.1:" + std::to_string(port), grpc::InsecureChannelCredentials()));
  }

}

void
testGrpcCalls() {
  anch::rest::RpcService service = anch::test::petService();
  anch::rest::GrpcServer server;
  server.addService(service).listen("127.0.0.1:0");
  const int port = server.start();
  anch::ut::assertThat(port > 0 && server.getPort() == port, "Port has to be selected, found {}", port);
  auto client = stub(port);
  // Success with metadata
  GetPetRequest request;
  request.set_id(1);
  Pet pet;
  grpc::ClientContext context;
  context.AddMetadata("x-user", "vincent");
  grpc::Status status = client->GetPet(&context, request, &pet);
  anch::ut::assertThat(status.ok() && pet.name() == "Rex" && pet.tags(1) == "vincent", "Unexpected call: {}", status.error_message());
  auto served = context.GetServerInitialMetadata().find("x-served-by");
  anch::ut::assertThat(served != context.GetServerInitialMetadata().end() && std::string(served->second.data(), served->second.size()) == "anch",
		       "Response metadata has to be initial metadata");
  // Errors
  request.set_id(2);
  grpc::ClientContext notFoundContext;
  status = client->GetPet(&notFoundContext, request, &pet);
  anch::ut::assertThat(status.error_code() == grpc::StatusCode::NOT_FOUND && status.error_message() == "No pet 2", "Unexpected status {} '{}'",
		       static_cast<int>(status.error_code()), status.error_message());
  Pet crash;
  crash.set_name("crash");
  grpc::ClientContext crashContext;
  status = client->AddPet(&crashContext, crash, &pet);
  anch::ut::assertThat(status.error_code() == grpc::StatusCode::INTERNAL && status.error_message().empty(), "Internal errors must not be exposed");
  Pet empty;
  grpc::ClientContext invalidContext;
  status = client->AddPet(&invalidContext, empty, &pet);
  anch::ut::assertThat(status.error_code() == grpc::StatusCode::INVALID_ARGUMENT && status.error_message() == "Name is required",
		       "HttpException has to be converted");
  anch::test::pets::DeletePetRequest remove;
  anch::test::pets::Empty nothing;
  grpc::ClientContext unimplementedContext;
  status = client->DeletePet(&unimplementedContext, remove, &nothing);
  anch::ut::assertThat(status.error_code() == grpc::StatusCode::UNIMPLEMENTED, "Unknown method has to be UNIMPLEMENTED");
  server.shutdown();
}

void
testGrpcSharedService() {
  // One service, two transports: gRPC (HTTP/2) and Connect (anch-rest API)
  anch::rest::RpcService service = anch::test::petService();
  anch::rest::GrpcServer server;
  server.addService(service).listen("127.0.0.1:0");
  auto client = stub(server.start());
  anch::rest::Api api;
  anch::rest::installRpc(api, service);
  Pet pet;
  pet.set_name("Médor");
  Pet saved;
  grpc::ClientContext context;
  anch::ut::assertThat(client->AddPet(&context, pet, &saved).ok() && saved.id() == 42, "Unexpected gRPC call");
  anch::rest::Request request(anch::rest::Method::POST, "/anch.test.pets.PetStore/AddPet");
  request.getHeaders().set("Content-Type", "application/proto");
  request.setBody(pet.SerializeAsString());
  anch::rest::Response response = api.handle(request);
  Pet connect;
  anch::ut::assertThat(response.getStatus() == anch::rest::Status::OK && connect.ParseFromString(response.content())
		       && connect.SerializeAsString() == saved.SerializeAsString(),
		       "Connect call has to give the same result");
  bool duplicate = false;
  try {
    server.addService(service);
  } catch(const std::invalid_argument&) {
    duplicate = true;
  }
  anch::ut::assertThat(duplicate, "Service can not be added twice");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH REST gRPC tests")
    .description("Test RPC services served by gRPC server")
    .add("calls", testGrpcCalls)
    .add("shared-service", testGrpcSharedService)
    ;
}
