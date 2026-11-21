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

#include <type_traits>
#include <utility>

namespace anch::rest {

  template<anch::codec::ProtobufMessage Req, anch::codec::ProtobufMessage Res, typename F>
  RpcService&
  RpcService::unary(std::string_view method, F&& handler) {
    static_assert(std::is_invocable_r_v<Res, F&, const Req&, anch::rest::RpcContext&> || std::is_invocable_r_v<Res, F&, const Req&>,
		  "RPC handler has to be Res(const Req&, RpcContext&) or Res(const Req&)");
    add(method, [function = std::forward<F>(handler)](std::string_view input, anch::rest::RpcFormat format,
						       anch::rest::RpcContext& context) mutable {
      Req request;
      if(format == anch::rest::RpcFormat::JSON) {
	try {
	  anch::codec::ProtobufJson::fromJson(request, input, false);
	} catch(const std::invalid_argument& error) {
	  throw anch::rest::RpcError(anch::rest::RpcCode::INVALID_ARGUMENT, error.what());
	}
      } else if(!request.ParseFromString(input)) {
	throw anch::rest::RpcError(anch::rest::RpcCode::INVALID_ARGUMENT, "Invalid " + std::string(request.GetTypeName()) + " message");
      }
      Res response = [&]() {
	if constexpr (std::is_invocable_r_v<Res, F&, const Req&, anch::rest::RpcContext&>) {
	  return function(std::as_const(request), context);
	} else {
	  return function(std::as_const(request));
	}
      }();
      if(format == anch::rest::RpcFormat::JSON) {
	return anch::codec::ProtobufJson::toJson(response);
      }
      std::string output;
      if(!response.SerializeToString(&output)) {
	throw anch::rest::RpcError(anch::rest::RpcCode::INTERNAL, "");
      }
      return output;
    });
    return *this;
  }

}
