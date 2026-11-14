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
#include "rest/rpcService.hpp"

#include <array>
#include <sstream>

#include "json/json.hpp"
#include "json/mappingOptions.hpp"

#include "rest/httpException.hpp"
#include "rest/request.hpp"

using anch::rest::HttpException;
using anch::rest::RpcCode;
using anch::rest::RpcContext;
using anch::rest::RpcError;
using anch::rest::RpcFormat;
using anch::rest::RpcService;
using anch::rest::Status;

namespace {

  /*! Connect error body */
  struct ConnectError {
    std::string code;
    std::optional<std::string> message;
  };

  /*! Connect codes names, by code */
  constexpr std::array<std::string_view, 17> CONNECT_CODES = {
    "ok", "canceled", "unknown", "invalid_argument", "deadline_exceeded", "not_found", "already_exists",
    "permission_denied", "resource_exhausted", "failed_precondition", "aborted", "out_of_range", "unimplemented",
    "internal", "unavailable", "data_loss", "unauthenticated"
  };

  /*! JSON string literal */
  std::string
  jsonString(std::string_view value) {
    static constexpr std::string_view HEX = "0123456789abcdef";
    std::string res = "\"";
    for(char c : value) {
      switch(c) {
      case '"': res += "\\\""; break;
      case '\\': res += "\\\\"; break;
      case '\n': res += "\\n"; break;
      case '\r': res += "\\r"; break;
      case '\t': res += "\\t"; break;
      default:
	if(static_cast<unsigned char>(c) < 0x20) {
	  res += "\\u00";
	  res += HEX[static_cast<unsigned char>(c) >> 4];
	  res += HEX[static_cast<unsigned char>(c) & 0x0F];
	} else {
	  res += c;
	}
      }
    }
    return res + '"';
  }

}

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<ConnectError>& mapper) {
  mapper
    .registerField("code", &ConnectError::code, anch::json::required)
    .registerField<std::string>("message", &ConnectError::message)
    ;
}

// RpcError +
RpcError::RpcError(RpcCode code, const std::string& message): std::runtime_error(message), _code(code) {
  // Nothing to do
}

RpcCode
RpcError::getCode() const noexcept {
  return _code;
}
// RpcError -

// RpcContext +
RpcContext::RpcContext(anch::rest::Headers request): _request(std::move(request)), _response() {
  // Nothing to do
}

const anch::rest::Headers&
RpcContext::getRequestMetadata() const noexcept {
  return _request;
}

anch::rest::Headers&
RpcContext::getResponseMetadata() noexcept {
  return _response;
}
// RpcContext -

// RpcService +
RpcService::RpcService(std::string_view name): _name(name), _methods(), _maxMessageSize(4 * 1024 * 1024) {
  if(_name.empty() || _name.find('/') != std::string::npos) {
    throw std::invalid_argument("Invalid RPC service name '" + _name + "'");
  }
}

void
RpcService::add(std::string_view method, Method function) {
  if(method.empty() || method.find('/') != std::string_view::npos) {
    throw std::invalid_argument("Invalid RPC method name '" + std::string(method) + "'");
  }
  if(!_methods.emplace(std::string(method), std::move(function)).second) {
    throw std::invalid_argument("RPC method " + _name + "/" + std::string(method) + " already exists");
  }
}

std::string
RpcService::call(std::string_view method, std::string_view request, RpcFormat format, RpcContext& context) const {
  auto found = _methods.find(method);
  if(found == _methods.end()) {
    throw RpcError(RpcCode::UNIMPLEMENTED, "Unknown method " + _name + "/" + std::string(method));
  }
  if(request.size() > _maxMessageSize) {
    throw RpcError(RpcCode::RESOURCE_EXHAUSTED, "Request message is larger than " + std::to_string(_maxMessageSize) + " bytes");
  }
  try {
    return found->second(request, format, context);
  } catch(const RpcError&) {
    throw;
  } catch(const HttpException& error) {
    throw RpcError(anch::rest::rpc::rpcCode(error.getStatus()), error.what());
  } catch(...) {
    throw RpcError(RpcCode::INTERNAL, "");
  }
}

bool
RpcService::has(std::string_view method) const {
  return _methods.contains(method);
}

std::vector<std::string>
RpcService::getMethods() const {
  std::vector<std::string> res;
  for(const auto& [name, method] : _methods) {
    res.push_back(name);
  }
  return res;
}

const std::string&
RpcService::getName() const noexcept {
  return _name;
}

std::size_t
RpcService::getMaxMessageSize() const noexcept {
  return _maxMessageSize;
}

void
RpcService::setMaxMessageSize(std::size_t size) noexcept {
  _maxMessageSize = size;
}
// RpcService -

// Protocols +
std::string_view
anch::rest::rpc::connectCode(RpcCode code) noexcept {
  const auto index = static_cast<std::size_t>(code);
  return index < CONNECT_CODES.size() ? CONNECT_CODES[index] : std::string_view("unknown");
}

std::optional<RpcCode>
anch::rest::rpc::parseConnectCode(std::string_view name) noexcept {
  for(std::size_t i = 1 ; i < CONNECT_CODES.size() ; ++i) {
    if(CONNECT_CODES[i] == name) {
      return static_cast<RpcCode>(i);
    }
  }
  return std::nullopt;
}

Status
anch::rest::rpc::httpStatus(RpcCode code) noexcept {
  switch(code) {
  case RpcCode::OK: return Status::OK;
  case RpcCode::CANCELLED: return static_cast<Status>(499);
  case RpcCode::INVALID_ARGUMENT: return Status::BAD_REQUEST;
  case RpcCode::DEADLINE_EXCEEDED: return Status::GATEWAY_TIMEOUT;
  case RpcCode::NOT_FOUND: return Status::NOT_FOUND;
  case RpcCode::ALREADY_EXISTS: return Status::CONFLICT;
  case RpcCode::PERMISSION_DENIED: return Status::FORBIDDEN;
  case RpcCode::RESOURCE_EXHAUSTED: return Status::TOO_MANY_REQUESTS;
  case RpcCode::FAILED_PRECONDITION: return Status::BAD_REQUEST;
  case RpcCode::ABORTED: return Status::CONFLICT;
  case RpcCode::OUT_OF_RANGE: return Status::BAD_REQUEST;
  case RpcCode::UNIMPLEMENTED: return Status::NOT_IMPLEMENTED;
  case RpcCode::UNAVAILABLE: return Status::SERVICE_UNAVAILABLE;
  case RpcCode::UNAUTHENTICATED: return Status::UNAUTHORIZED;
  case RpcCode::UNKNOWN:
  case RpcCode::INTERNAL:
  case RpcCode::DATA_LOSS:
    break;
  }
  return Status::INTERNAL_SERVER_ERROR;
}

RpcCode
anch::rest::rpc::rpcCode(Status status) noexcept {
  switch(anch::rest::code(status)) {
  case 400: return RpcCode::INVALID_ARGUMENT;
  case 401: return RpcCode::UNAUTHENTICATED;
  case 403: return RpcCode::PERMISSION_DENIED;
  case 404: return RpcCode::NOT_FOUND;
  case 409: return RpcCode::ALREADY_EXISTS;
  case 413: return RpcCode::RESOURCE_EXHAUSTED;
  case 429: return RpcCode::RESOURCE_EXHAUSTED;
  case 499: return RpcCode::CANCELLED;
  case 501: return RpcCode::UNIMPLEMENTED;
  case 502:
  case 503: return RpcCode::UNAVAILABLE;
  case 504: return RpcCode::DEADLINE_EXCEEDED;
  default: return RpcCode::UNKNOWN;
  }
}

std::string
anch::rest::rpc::connectError(const RpcError& error) {
  std::string res = "{\"code\":" + jsonString(connectCode(error.getCode()));
  if(*error.what() != '\0') {
    res += ",\"message\":" + jsonString(error.what());
  }
  return res + '}';
}

RpcError
anch::rest::rpc::parseConnectError(Status status, std::string_view body) {
  try {
    anch::json::MappingOptions options = anch::json::DEFAULT_MAPPING_OPTIONS;
    options.deserialize_ignore_unknown_field = true; // details
    options.deserialize_max_discard_char = -1;
    std::istringstream input{std::string(body)};
    ConnectError error;
    anch::json::deserialize(error, input, options);
    if(std::optional<RpcCode> code = parseConnectCode(error.code)) {
      return RpcError(*code, error.message.value_or(""));
    }
  } catch(...) {
    // Not a Connect error
  }
  return RpcError(rpcCode(status), std::string(body));
}

std::string
anch::rest::rpc::grpcWebFrame(std::string_view data, bool trailers) {
  const auto size = static_cast<uint32_t>(data.size());
  std::string res;
  res.reserve(5 + data.size());
  res += static_cast<char>(trailers ? 0x80 : 0x00);
  res += static_cast<char>((size >> 24) & 0xFF);
  res += static_cast<char>((size >> 16) & 0xFF);
  res += static_cast<char>((size >> 8) & 0xFF);
  res += static_cast<char>(size & 0xFF);
  res += data;
  return res;
}

std::string
anch::rest::rpc::grpcWebMessage(std::string_view body) {
  if(body.size() < 5) {
    throw RpcError(RpcCode::INVALID_ARGUMENT, "Invalid gRPC-Web message frame");
  }
  if(body[0] != 0) {
    throw RpcError(body[0] == 1 ? RpcCode::UNIMPLEMENTED : RpcCode::INVALID_ARGUMENT,
		   body[0] == 1 ? "Compressed gRPC-Web messages are not supported" : "Invalid gRPC-Web message frame");
  }
  const uint32_t size = (static_cast<uint32_t>(static_cast<unsigned char>(body[1])) << 24)
    | (static_cast<uint32_t>(static_cast<unsigned char>(body[2])) << 16)
    | (static_cast<uint32_t>(static_cast<unsigned char>(body[3])) << 8)
    | static_cast<uint32_t>(static_cast<unsigned char>(body[4]));
  if(body.size() != 5 + static_cast<std::size_t>(size)) {
    throw RpcError(RpcCode::INVALID_ARGUMENT, "Invalid gRPC-Web message frame");
  }
  return std::string(body.substr(5));
}

std::string
anch::rest::rpc::grpcWebTrailers(RpcCode code, std::string_view message) {
  std::string res = "grpc-status:" + std::to_string(static_cast<int>(code)) + "\r\n";
  if(!message.empty()) {
    res += "grpc-message:" + anch::rest::percentEncode(message) + "\r\n";
  }
  return res;
}
// Protocols -
