/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "rest/endpointFunction.hpp"

#include "convert.hpp" // from anch-utils
#include "uuid.hpp" // from anch-uuid

using anch::rest::EndpointFunc;
using anch::rest::EndpointSimpleFunc;
using anch::rest::EndpointParamsFunc;
using anch::rest::Request;
using anch::rest::Response;


// EndpointFunc +
// Destructor +
EndpointFunc::~EndpointFunc() {
  // Nothing to do
}
// Destructor -
// EndpointFunc -


// EndpointSimpleFunc +
// Constructors +
EndpointSimpleFunc::EndpointSimpleFunc(std::function<Response(const Request&)> func): _func(func) {
  // Nothing to do
}
// Constructors -

// Destructor +
EndpointSimpleFunc::~EndpointSimpleFunc() {
  // Nothing to do
}
// Destructor -

// Methods +
Response
EndpointSimpleFunc::execute(const Request& request) {
  return _func(request);
}
// Methods -
// EndpointSimpleFunc -

// Convert path parameters +
namespace anch::rest {

  template<>
  std::string
  convertPathParam(const std::string& param) {
    return param;
  }

  template<>
  std::string_view
  convertPathParam(const std::string& param) {
    return std::string_view(param.data());
  }

  template<>
  uint16_t
  convertPathParam(const std::string& param) {
    try {
      return convert<uint16_t>(param);
    } catch(...) {
      throw StatusCode::BAD_REQUEST;
    }
  }

  template<>
  int16_t
  convertPathParam(const std::string& param) {
    try {
      return convert<int16_t>(param);
    } catch(...) {
      throw StatusCode::BAD_REQUEST;
    }
  }

  template<>
  uint32_t
  convertPathParam(const std::string& param) {
    try {
      return convert<uint32_t>(param);
    } catch(...) {
      throw StatusCode::BAD_REQUEST;
    }
  }

  template<>
  int32_t
  convertPathParam(const std::string& param) {
    try {
      return convert<int32_t>(param);
    } catch(...) {
      throw StatusCode::BAD_REQUEST;
    }
  }

  template<>
  uint64_t
  convertPathParam(const std::string& param) {
    try {
      return convert<uint64_t>(param);
    } catch(...) {
      throw StatusCode::BAD_REQUEST;
    }
  }

  template<>
  int64_t
  convertPathParam(const std::string& param) {
    try {
      return convert<int64_t>(param);
    } catch(...) {
      throw StatusCode::BAD_REQUEST;
    }
  }

  template<>
  anch::UUID
  convertPathParam(const std::string& param) {
    try {
      return UUID(param);
    } catch(...) {
      throw StatusCode::BAD_REQUEST;
    }
  }

}  // anch::rest
// Convert path parameters -
