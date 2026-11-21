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

#include <sstream>
#include <string>
#include <string_view>

#include "rest/client.hpp"
#include "rest/response.hpp"
#include "rest/rpcService.hpp"

namespace anch::rest {

  /*!
   * Call RPC method with Connect protocol (unary call, header only: link \c anch::rest-client ):
   * <tt>POST /package.Service/Method</tt> on client base URL. The server can be an anch-rest API
   * ( \ref installRpc ) or any Connect server.
   * \snippet{trimleft} anch-rest/examples/protobuf.cpp rpc client
   *
   * \tparam Res the response message type
   * \tparam Req the request message type
   *
   * \param client the anch-rest client
   * \param service the service full name ( \c package.Service )
   * \param method the method name
   * \param request the request message
   * \param format the messages encoding ( \c application/proto or \c application/json )
   *
   * \return the response message
   *
   * \throw RpcError on error response (Connect error code and message) or invalid response message
   * \throw TransportError on network error
   *
   * \since 0.2
   */
  template<anch::codec::ProtobufMessage Res, anch::codec::ProtobufMessage Req>
  Res callRpc(anch::rest::Client& client, std::string_view service, std::string_view method, const Req& request,
	      anch::rest::RpcFormat format = anch::rest::RpcFormat::BINARY);

}

#include "rest/impl/rpcClient.hpp"
