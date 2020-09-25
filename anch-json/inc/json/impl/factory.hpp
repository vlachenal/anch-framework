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

#include "json/mapper.hpp"
#include "json/primitiveMapper.hpp"

namespace anch {
  namespace json {

    /*!
     * Check if type is a primitive JSON type
     *
     * \tparam T the type to check
     *
     * \return \c true if type is a JSON primitive type, \c false otherwise
     */
    template<typename T>
    constexpr bool isPrimitive() {
      return std::is_same<T, std::string>::value
	|| std::is_same<T, std::string_view>::value
	|| std::is_same<T, int64_t>::value
	|| std::is_same<T, uint64_t>::value
	|| std::is_same<T, int32_t>::value
	|| std::is_same<T, uint32_t>::value
	|| std::is_same<T, int16_t>::value
	|| std::is_same<T, uint16_t>::value
	|| std::is_same<T, int8_t>::value
	|| std::is_same<T, uint8_t>::value
	|| std::is_same<T, bool>::value
	|| std::is_same<T, float>::value
	|| std::is_same<T, double>::value
	|| std::is_same<T, long double>::value
	;
    }

    template<typename T>
    auto& JSONFactory<T>::getInstance() {
      if constexpr (isPrimitive<T>()) {
	static anch::json::JSONPrimitiveMapper<T> instance;
	return instance;
      } else {
	static anch::json::JSONMapper<T> instance;
	return instance;
      }
    }

  }
}
