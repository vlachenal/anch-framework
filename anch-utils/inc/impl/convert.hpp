/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <charconv>
#include <system_error>

namespace anch {

  template<anch::Convertible N>
  inline std::expected<N, std::errc>
  tryConvert(std::string_view str) noexcept {
    N num{};
    const char* end = str.data() + str.size();
    auto [ptr, ec] = std::from_chars(str.data(), end, num);
    // On overflow, std::from_chars consumes every digit: error code is checked first
    if(ec != std::errc()) {
      return std::unexpected(ec);
    }
    if(ptr != end) {
      return std::unexpected(std::errc::invalid_argument);
    }
    return num;
  }

  template<anch::Convertible N>
  inline N
  convert(std::string_view str) {
    std::expected<N, std::errc> num = anch::tryConvert<N>(str);
    if(!num) {
      throw std::bad_cast();
    }
    return *num;
  }

}
