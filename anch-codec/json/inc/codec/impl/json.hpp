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

#include <string>
#include <type_traits>

namespace anch::codec {

  template<typename T>
  requires (!anch::codec::ExternalSchema<T>::value)
  void
  Json::serialize(const T& value, std::ostream& out) {
    Json::serialize(value, out, anch::json::DEFAULT_MAPPING_OPTIONS);
  }

  template<typename T>
  requires (!anch::codec::ExternalSchema<T>::value)
  void
  Json::serialize(const T& value, std::ostream& out, anch::codec::Side side) {
    Json::serialize(value, out, Json::options(side));
  }

  template<typename T>
  void
  Json::serialize(const T& value, std::ostream& out, const anch::json::MappingOptions& options) {
    if constexpr (std::is_same_v<T, std::string>) {
      // std::string would match anch-json container overload (C<T> with T = char)
      anch::json::serialize<std::string>(value, out, options);
    } else {
      anch::json::serialize(value, out, options);
    }
  }

  template<typename T>
  requires (!anch::codec::ExternalSchema<T>::value)
  void
  Json::deserialize(T& value, std::istream& input) {
    Json::deserialize(value, input, anch::json::DEFAULT_MAPPING_OPTIONS);
  }

  template<typename T>
  requires (!anch::codec::ExternalSchema<T>::value)
  void
  Json::deserialize(T& value, std::istream& input, anch::codec::Side side) {
    Json::deserialize(value, input, Json::options(side));
  }

  template<typename T>
  void
  Json::deserialize(T& value, std::istream& input, const anch::json::MappingOptions& options) {
    try {
      if constexpr (std::is_same_v<T, std::string>) {
	// std::string would match anch-json container overload (C<T> with T = char)
	anch::json::deserialize<std::string>(value, input, options);
      } else {
	anch::json::deserialize(value, input, options);
      }
    } catch(const anch::json::MappingError& error) {
      if(error.getErrorCode() == anch::json::ErrorCode::PAYLOAD_TOO_LARGE) {
	throw anch::codec::ContentTooLarge(error.what());
      }
      throw;
    }
  }

}
