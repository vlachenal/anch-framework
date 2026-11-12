/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

namespace anch::rest {

  template<typename F>
  anch::rest::Endpoint<std::decay_t<F>>&
  Api::route(anch::rest::Method method, std::string_view pattern, F&& function) {
    std::vector<std::string> names = parsePattern(pattern);
    auto endpoint = std::make_unique<anch::rest::Endpoint<std::decay_t<F>>>(method,
									       std::string(pattern),
									       std::move(names),
									       std::forward<F>(function));
    anch::rest::Endpoint<std::decay_t<F>>& res = *endpoint;
    addRoute(std::move(endpoint));
    return res;
  }

  template<typename F>
  anch::rest::Endpoint<std::decay_t<F>>&
  Api::get(std::string_view pattern, F&& function) {
    return route(anch::rest::Method::GET, pattern, std::forward<F>(function));
  }

  template<typename F>
  anch::rest::Endpoint<std::decay_t<F>>&
  Api::post(std::string_view pattern, F&& function) {
    return route(anch::rest::Method::POST, pattern, std::forward<F>(function));
  }

  template<typename F>
  anch::rest::Endpoint<std::decay_t<F>>&
  Api::put(std::string_view pattern, F&& function) {
    return route(anch::rest::Method::PUT, pattern, std::forward<F>(function));
  }

  template<typename F>
  anch::rest::Endpoint<std::decay_t<F>>&
  Api::patch(std::string_view pattern, F&& function) {
    return route(anch::rest::Method::PATCH, pattern, std::forward<F>(function));
  }

  template<typename F>
  anch::rest::Endpoint<std::decay_t<F>>&
  Api::del(std::string_view pattern, F&& function) {
    return route(anch::rest::Method::DELETE, pattern, std::forward<F>(function));
  }

}
