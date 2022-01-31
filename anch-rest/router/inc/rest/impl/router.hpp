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

#include "rest/routerException.hpp"

namespace anch {
  namespace rest {

    template<typename... P>
    Router&
    Router::add(std::function<anch::rest::Response(const anch::rest::Request&, P...)> func,
		const std::string& path,
		const std::string& verb,
		const std::vector<std::string>& consumes,
		const std::vector<std::string>& produces) {
      std::vector<std::string> cons(consumes);
      if(cons.empty()) {
	cons.push_back(MediaType::NONE);
      }
      std::vector<std::string> pros(produces);
      if(pros.empty()) {
	pros.push_back(MediaType::NONE);
      }
      for(const std::string& ctype : cons) {
	for(const std::string& ptype : pros) {
	  EndPoint endpoint;
	  endpoint.pathPattern = path;
	  endpoint.verb = verb;
	  endpoint.contentType = ctype;
	  endpoint.accept = ptype;
	  endpoint.build();
	  add(endpoint, func);
	}
      }
      return *this;
    }

    template<typename... P>
    Router&
    Router::add(EndPoint& endpoint, std::function<anch::rest::Response(const anch::rest::Request&, P...)> func) {
      auto found = _routes.find(endpoint);
      if(found != _routes.end()) {
	throw anch::rest::RouterException("Endpoint already exists");
      }
      endpoint.build();
      if constexpr (sizeof...(P) == 0) {
	_routes[endpoint] = new EndpointSimpleFunc(func);
      } else {
	_routes[endpoint] = new EndpointParamsFunc<P...>(func);
      }
      return *this;
    }

  } // rest
} // anch
