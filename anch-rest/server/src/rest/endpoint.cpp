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
#include "rest/endpoint.hpp"

using anch::rest::Method;
using anch::rest::Route;

Route::Route(Method method, std::string pattern, std::vector<std::string> parameterNames):
  _method(method),
  _pattern(std::move(pattern)),
  _parameterNames(std::move(parameterNames)) {
  // Nothing to do
}

Route::~Route() {
  // Nothing to do
}

Method
Route::getMethod() const noexcept {
  return _method;
}

const std::string&
Route::getPattern() const noexcept {
  return _pattern;
}

const std::vector<std::string>&
Route::getParameterNames() const noexcept {
  return _parameterNames;
}
