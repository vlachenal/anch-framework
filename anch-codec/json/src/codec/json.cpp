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
#include "codec/json.hpp"

#include "codec/format.hpp"

using anch::codec::Json;

namespace {

  // Registered when the library is loaded: a program which uses Json links this file (mediaType, options)
  [[maybe_unused]] const bool REGISTERED = anch::codec::registerFormat(anch::codec::Format{"json", {std::string(Json::mediaType())}});

}

std::string_view
Json::mediaType() noexcept {
  return "application/json";
}

anch::json::MappingOptions&
Json::options(anch::codec::Side side) {
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
  return (side == anch::codec::Side::SERVER) ? server : client;
}
