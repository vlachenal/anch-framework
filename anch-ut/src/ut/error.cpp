/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "ut/error.hpp"

using anch::ut::AssertException;

AssertException::AssertException(const std::string& msg): _msg(msg), _location() {
  // Nothing to do
}

AssertException::AssertException(const std::string& msg, const std::source_location& location): _msg(msg),
												  _location(location) {
  // Nothing to do
}

AssertException::~AssertException() {
  // Nothing to do
}

const char*
AssertException::what() const noexcept {
  return _msg.data();
}

const std::source_location&
AssertException::location() const noexcept {
  return _location;
}
