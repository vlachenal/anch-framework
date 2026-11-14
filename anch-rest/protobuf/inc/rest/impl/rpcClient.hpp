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
#pragma once

#include <stdexcept>

namespace anch::rest {

  template<anch::rest::ProtobufMessage Res, anch::rest::ProtobufMessage Req>
  Res
  callRpc(anch::rest::Client& client, std::string_view service, std::string_view method, const Req& request, anch::rest::RpcFormat format) {
    const bool json = (format == anch::rest::RpcFormat::JSON);
    std::string body;
    if(json) {
      body = anch::rest::ProtobufJson::toJson(request);
    } else if(!request.SerializeToString(&body)) {
      throw std::runtime_error("Unable to write protobuf message " + std::string(request.GetTypeName()));
    }
    anch::rest::Response response = client.post("/" + std::string(service) + "/" + std::string(method))
      .header("Connect-Protocol-Version", "1")
      .body(std::move(body), json ? "application/json" : "application/proto")
      .send();
    if(response.getStatus() != anch::rest::Status::OK) {
      throw anch::rest::rpc::parseConnectError(response.getStatus(), response.content());
    }
    Res result;
    const std::string content = response.content();
    try {
      if(json) {
	anch::rest::ProtobufJson::fromJson(result, content, true);
      } else if(!result.ParseFromString(content)) {
	throw std::invalid_argument("Invalid " + std::string(result.GetTypeName()) + " message");
      }
    } catch(const std::invalid_argument& error) {
      throw anch::rest::RpcError(anch::rest::RpcCode::INTERNAL, std::string("Invalid response: ") + error.what());
    }
    return result;
  }

}
