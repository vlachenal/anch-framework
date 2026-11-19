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

#include "rest/headers.hpp"
#include "rest/mediaType.hpp"

/*! \cond INTERNAL */
namespace anch::rest::detail {

  /*!
   * Handle Connect or gRPC-Web unary call
   */
  inline anch::rest::Response
  handleRpc(const anch::rest::RpcService& service, const std::string& method, const anch::rest::Request& request, std::istream& body) {
    std::optional<anch::rest::MediaType> type = request.getContentType();
    const std::string essence = type ? type->essence() : std::string();
    bool grpcWeb = false;
    anch::rest::RpcFormat format = anch::rest::RpcFormat::BINARY;
    if(essence == "application/proto" || essence == "application/x-protobuf") {
      format = anch::rest::RpcFormat::BINARY;
    } else if(essence == "application/json") {
      format = anch::rest::RpcFormat::JSON;
    } else if(essence == "application/grpc-web" || essence == "application/grpc-web+proto") {
      grpcWeb = true;
    } else if(essence == "application/grpc-web+json") {
      grpcWeb = true;
      format = anch::rest::RpcFormat::JSON;
    } else {
      throw anch::rest::HttpException(anch::rest::Status::UNSUPPORTED_MEDIA_TYPE);
    }
    anch::rest::RpcContext context(request.getHeaders());
    auto withMetadata = [&context](anch::rest::Response response) {
      for(const auto& [name, values] : context.getResponseMetadata()) {
	for(const std::string& value : values) {
	  response.getHeaders().add(name, value); // every value is sent (header() would keep the last one)
	}
      }
      return response;
    };
    try {
      std::string content;
      try {
	content = anch::rest::readContent(body, service.getMaxMessageSize() + (grpcWeb ? 5 : 0));
      } catch(const anch::rest::ContentTooLarge& error) {
	throw anch::rest::RpcError(anch::rest::RpcCode::RESOURCE_EXHAUSTED, error.what());
      }
      if(grpcWeb) {
	content = anch::rest::rpc::grpcWebMessage(content);
      }
      std::string output = service.call(method, content, format, context);
      if(grpcWeb) {
	output = anch::rest::rpc::grpcWebFrame(output, false)
	  + anch::rest::rpc::grpcWebFrame(anch::rest::rpc::grpcWebTrailers(anch::rest::RpcCode::OK, ""), true);
      }
      return withMetadata(anch::rest::Response(anch::rest::Status::OK).contentType(essence).body(std::move(output)));
    } catch(const anch::rest::RpcError& error) {
      if(grpcWeb) {
	return withMetadata(anch::rest::Response(anch::rest::Status::OK).contentType(essence)
			    .body(anch::rest::rpc::grpcWebFrame(anch::rest::rpc::grpcWebTrailers(error.getCode(), error.what()), true)));
      }
      return withMetadata(anch::rest::Response(anch::rest::rpc::httpStatus(error.getCode()))
			  .contentType("application/json").body(anch::rest::rpc::connectError(error)));
    }
  }

}
/*! \endcond */

namespace anch::rest {

  inline void
  installRpc(anch::rest::Api& api, const anch::rest::RpcService& service, std::string_view prefix) {
    for(const std::string& method : service.getMethods()) {
      api.post(std::string(prefix) + '/' + service.getName() + '/' + method,
	       [&service, method](const anch::rest::Request& request, std::istream& body) {
		 return anch::rest::detail::handleRpc(service, method, request, body);
	       });
    }
  }

}
