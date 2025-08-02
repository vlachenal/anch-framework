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

#include <type_traits>
#include <cstdint>
#include <string>
#include <string_view>

#include "json/mapper.hpp"
#include "json/primitiveMapper.hpp"

#ifdef ANCH_UUID
#include "uuid.hpp"
#endif

#ifdef ANCH_DATE
#include "date/date.hpp"
#endif

#ifdef ANCH_CRYPTO
#include <istream>
#endif

namespace anch::json {

  // isPrimitive default implemetation (default false) +
  template<typename T>
  constexpr bool isPrimitive() {
    return false;
  }
  // isPrimitive default implemetation (default false) -

  template<typename T>
  auto& Factory<T>::getInstance() {
    if constexpr (isPrimitive<T>()) {
      static anch::json::PrimitiveMapper<T> instance;
      return instance;
    } else if constexpr (std::is_enum<T>::value) {
      static anch::json::PrimitiveMapper<T> instance; // \todo define EnumMapper
      return instance;
    } else {
      static anch::json::ObjectMapper<T> instance;
      return instance;
    }
  }

  // isPrimitive specialization +
  // isPrimitive string specialization +
  template<>
  constexpr bool isPrimitive<std::string>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<std::string_view>() {
    return true;
  }
  // isPrimitive string specialization -

  // isPrimitive integer specialization +
  template<>
  constexpr bool isPrimitive<int64_t>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<uint64_t>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<int32_t>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<uint32_t>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<int16_t>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<uint16_t>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<int8_t>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<uint8_t>() {
    return true;
  }
  // isPrimitive integer specialization -

  // isPrimitive boolean specialization +
  template<>
  constexpr bool isPrimitive<bool>() {
    return true;
  }
  // isPrimitive boolean specialization -

  // isPrimitive float specialization +
  template<>
  constexpr bool isPrimitive<float>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<double>() {
    return true;
  }

  template<>
  constexpr bool isPrimitive<long double>() {
    return true;
  }
  // isPrimitive float specialization -
  // isPrimitive specialization -

}
