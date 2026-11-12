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

#include <charconv>
#include <stdexcept>
#include <system_error>

namespace anch::rest {

  template<typename T>
  requires (std::integral<T> && !std::same_as<T, bool>)
  std::optional<T>
  ParamConverter<T>::convert(std::string_view value) noexcept {
    T res{};
    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), res);
    if(value.empty() || ec != std::errc() || ptr != value.data() + value.size()) {
      return std::nullopt;
    }
    return res;
  }

  template<std::floating_point T>
  std::optional<T>
  ParamConverter<T>::convert(std::string_view value) noexcept {
    T res{};
    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), res);
    if(value.empty() || ec != std::errc() || ptr != value.data() + value.size()) {
      return std::nullopt;
    }
    return res;
  }

  inline std::optional<bool>
  ParamConverter<bool>::convert(std::string_view value) noexcept {
    if(value == "true") {
      return true;
    }
    if(value == "false") {
      return false;
    }
    return std::nullopt;
  }

  inline std::optional<std::string>
  ParamConverter<std::string>::convert(std::string_view value) {
    return std::string(value);
  }

  inline std::optional<anch::UUID>
  ParamConverter<anch::UUID>::convert(std::string_view value) {
    try {
      return anch::UUID(std::string(value));
    } catch(const std::invalid_argument&) {
      return std::nullopt;
    }
  }

}
