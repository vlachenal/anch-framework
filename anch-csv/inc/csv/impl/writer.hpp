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

#include <array>
#include <charconv>

namespace anch::csv::detail {

  template<typename V>
  requires std::convertible_to<const V&, std::string_view>
  Text::Text(const V& value): _view(value), _buffer(), _length(0) {
    // Nothing to do
  }

  inline
  Text::Text(char value): _view(), _buffer{value}, _length(1) {
    // Nothing to do
  }

  inline
  Text::Text(bool value): _view(value ? "true" : "false"), _buffer(), _length(0) {
    // Nothing to do
  }

  template<typename V>
  requires (std::is_arithmetic_v<V> && !std::same_as<V, bool> && !std::same_as<V, char>)
  Text::Text(V value): _view(), _buffer(), _length(0) {
    const std::to_chars_result res = std::to_chars(_buffer, _buffer + sizeof(_buffer), value);
    _length = static_cast<std::size_t>(res.ptr - _buffer);
  }

  inline std::string_view
  Text::value() const noexcept {
    return (_length == 0) ? _view : std::string_view(_buffer, _length);
  }

}

namespace anch::csv {

  template<typename... V>
  void
  Writer::writeRow(const V&... values) {
    const std::array<anch::csv::detail::Text, sizeof...(V)> texts{values...};
    std::array<std::string_view, sizeof...(V)> fields;
    for(std::size_t i = 0; i < texts.size(); ++i) {
      fields[i] = texts[i].value();
    }
    write(std::span<const std::string_view>(fields));
  }

}
