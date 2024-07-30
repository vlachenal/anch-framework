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

#include "ut/error.hpp"

namespace anch::ut {

  inline void assertFalse(bool check, const std::string& message) {
    assertTrue(!check, message);
  }

  inline void fail(const std::string& message) {
    assertTrue(false, message);
  }

  template<typename... T>
  void assert(bool check, std::string_view message, const T&... args) {
    if(!check) {
      anch::ut::fail(std::vformat(message, std::make_format_args(args...)));
    }
  }

}
