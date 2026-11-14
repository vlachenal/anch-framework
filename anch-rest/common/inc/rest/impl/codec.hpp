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

#include <iterator>

namespace anch::rest {

  template<typename C, typename T>
  requires anch::rest::Serializer<C, T>
  void
  serializeBody(const T& value, std::ostream& out, anch::rest::Side side) {
    if constexpr (requires { C::serialize(value, out, side); }) {
      C::serialize(value, out, side);
    } else {
      C::serialize(value, out);
    }
  }

  template<typename C, typename T>
  requires anch::rest::Deserializer<C, T>
  void
  deserializeBody(T& value, std::istream& input, anch::rest::Side side) {
    if constexpr (requires { C::deserialize(value, input, side); }) {
      C::deserialize(value, input, side);
    } else {
      C::deserialize(value, input);
    }
  }

  inline std::string_view
  Text::mediaType() noexcept {
    return "text/plain";
  }

  template<typename T>
  requires requires(std::ostream& out, const T& value) { out << value; }
  void
  Text::serialize(const T& value, std::ostream& out) {
    out << value;
  }

  inline void
  Text::deserialize(std::string& value, std::istream& input) {
    value.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
  }

}
