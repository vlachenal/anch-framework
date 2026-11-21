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

#include <istream>
#include <string>
#include <string_view>

#include "codec/codec.hpp"

#include "rest/api.hpp"
#include "rest/httpException.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"
#include "rest/rpcService.hpp"

namespace anch::rest {

  /*!
   * Install RPC service methods in API (header only: link \c anch::rest-server ): one
   * <tt>POST [prefix]/package.Service/Method</tt> route per method, which serves:
   * - Connect unary calls: \c application/proto (or \c application/x-protobuf ) and \c application/json
   *   bodies, \c 200 with the response message or Connect error ( <tt>{"code":"not_found","message":"..."}</tt>
   *   with the HTTP status of the code) ;
   * - gRPC-Web unary calls: \c application/grpc-web , \c application/grpc-web+proto and
   *   \c application/grpc-web+json bodies (message frame), \c 200 with the response message frame and
   *   the trailers frame ( \c grpc-status , \c grpc-message ).
   * Other content types are refused ( \c 415 ). Response metadata are HTTP headers. API filters and
   * CORS apply (gRPC-Web browser clients need \c grpc-status and \c grpc-message exposed headers).
   *
   * \param api the API
   * \param service the service (it has to outlive the API)
   * \param prefix the routes prefix ( \c /rpc , empty by default)
   *
   * \since 0.2
   */
  void installRpc(anch::rest::Api& api, const anch::rest::RpcService& service, std::string_view prefix = std::string_view());

}

#include "rest/impl/rpcApi.hpp"
