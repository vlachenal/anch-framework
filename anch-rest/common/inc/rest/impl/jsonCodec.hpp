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

namespace anch::rest {

  inline std::string_view
  Json::mediaType() noexcept {
    return "application/json";
  }

  inline anch::json::MappingOptions&
  Json::options(anch::rest::Side side) {
    static anch::json::MappingOptions server = []() {
      anch::json::MappingOptions options = anch::json::DEFAULT_MAPPING_OPTIONS;
      options.deserialize_check_constraints = true;
      options.serialize_check_constraints = false;
      if(options.max_payload == 0) {
	options.max_payload = 2 * 1024 * 1024; // request bodies are bounded, as application/x-www-form-urlencoded ones
      }
      return options;
    }();
    static anch::json::MappingOptions client = []() {
      anch::json::MappingOptions options = anch::json::DEFAULT_MAPPING_OPTIONS;
      options.deserialize_check_constraints = false;
      options.serialize_check_constraints = true;
      options.deserialize_ignore_unknown_field = true; // server responses are trusted: newer fields are ignored
      options.deserialize_max_discard_char = -1;       // indented responses are accepted
      return options;
    }();
    return (side == anch::rest::Side::SERVER) ? server : client;
  }

  template<typename T>
  void
  Json::serialize(const T& value, std::ostream& out) {
    Json::serialize(value, out, anch::json::DEFAULT_MAPPING_OPTIONS);
  }

  template<typename T>
  void
  Json::serialize(const T& value, std::ostream& out, anch::rest::Side side) {
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
  void
  Json::deserialize(T& value, std::istream& input) {
    Json::deserialize(value, input, anch::json::DEFAULT_MAPPING_OPTIONS);
  }

  template<typename T>
  void
  Json::deserialize(T& value, std::istream& input, anch::rest::Side side) {
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
	throw anch::rest::ContentTooLarge(error.what());
      }
      throw;
    }
  }

}
