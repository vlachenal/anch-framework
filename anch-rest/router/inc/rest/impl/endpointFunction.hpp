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
#pragma once

namespace anch::rest {

  template<typename P, typename... Q>
  std::function<anch::rest::Response()>
  bindParameters(std::function<anch::rest::Response(P, Q...)> func,
		 std::vector<std::string>::const_iterator iter,
		 std::vector<std::string>::const_iterator end) {
    if(iter == end) {
      throw StatusCode::INTERNAL_SERVER_ERROR;
    }
    auto val = convertPathParam<P>(std::cref(*iter));
    return bindParameters<Q...>(std::function<anch::rest::Response(Q...)>(std::bind_front(func, val)), ++iter, end);
  }

  template<typename P>
  std::function<anch::rest::Response()>
  bindParameters(std::function<anch::rest::Response(P)> func,
		 std::vector<std::string>::const_iterator iter,
		 std::vector<std::string>::const_iterator end) {
    if(iter == end) {
      throw StatusCode::INTERNAL_SERVER_ERROR;
    }
    auto val = convertPathParam<P>(std::cref(*iter));
    if(++iter != end) {
      throw StatusCode::INTERNAL_SERVER_ERROR;
    }
    return std::bind_front(func, val);
  }

  template<typename... P>
  std::function<anch::rest::Response()>
  bindRequest(std::function<anch::rest::Response(P...)> func,
	      const anch::rest::Request& request) {
    if(request.getParameters().size() == sizeof...(P)) {
      return bindParameters<P...>(func, request.getParameters().cbegin(), request.getParameters().cend());
    }
    throw StatusCode::INTERNAL_SERVER_ERROR;
  }

  template<typename... T>
  EndpointParamsFunc<T...>::EndpointParamsFunc(std::function<anch::rest::Response(const anch::rest::Request&, T...)> func): _func(func) {
    // Nothing to do
  }

  template<typename... T>
  EndpointParamsFunc<T...>::~EndpointParamsFunc() {
    // Nothing to do
  }

  template<typename... T>
  anch::rest::Response
  EndpointParamsFunc<T...>::execute(const anch::rest::Request& request) {
    std::function<Response(T...)> forBind = std::bind_front(_func, std::cref(request));
    std::function<anch::rest::Response()> call = bindRequest<T...>(forBind, request);
    return call();
  }

} // anch::rest
