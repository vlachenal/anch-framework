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
#ifdef ANCH_UUID // \todo UUID mapper implementation in string mapper
	|| std::is_same<T, anch::UUID>::value
#endif
#ifdef ANCH_DATE // \todo UUID date implementation in string mapper
	|| std::is_same<T, anch::date::Date>::value
#endif
#ifdef ANCH_CRYPTO // \todo base64 implementation ... which is a bad idea if my mind
	|| std::is_same<T, std::istream>::value
#endif
	;
    }

    template<typename T>
    auto& JSONFactory<T>::getInstance() {
      if constexpr (isPrimitive<T>()) {
	static anch::json::JSONPrimitiveMapper<T> instance;
	return instance;
      } else if constexpr (std::is_enum<T>::value) {
      	static anch::json::JSONPrimitiveMapper<T> instance; // \todo define JSONEnumMapper
      	return instance;
      } else {
	static anch::json::ObjectMapper<T> instance;
	return instance;
      }
    }

  }
}
