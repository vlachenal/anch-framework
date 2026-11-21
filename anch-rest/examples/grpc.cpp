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
#include "pets.pb.h"
#include "rest/grpcServer.hpp"
#include "rest/rpcService.hpp"

// Built, not run: the example server listens on a fixed port
void
startGrpc(anch::rest::RpcService& service) {
  //! [grpc server]
  anch::rest::GrpcServer server;              // anch-rest-grpc
  server.addService(service).listen("0.0.0.0:50051");
  server.start();
  // ...
  server.shutdown();
  //! [grpc server]
}

int
main() {
  return 0;
}
