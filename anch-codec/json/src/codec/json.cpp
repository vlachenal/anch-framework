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

#include "json/mappingError.hpp"

using anch::codec::FormatPolicy;
using anch::codec::Json;

template struct anch::codec::FormatCodec<anch::json::JsonFormat>;

namespace {

  // Registered when the library is loaded: a program which uses Json links this file (options)
  [[maybe_unused]] const bool REGISTERED = anch::codec::registerFormat(anch::codec::Format{"json", {std::string(Json::mediaType())}});

}

void
FormatPolicy<anch::json::JsonFormat>::configure(anch::json::MappingOptions& options, anch::codec::Side side) {
  if(side == anch::codec::Side::SERVER) {
    options.deserialize_check_constraints = true;
    options.serialize_check_constraints = false;
    if(options.max_payload == 0) {
      options.max_payload = 2 * 1024 * 1024; // request bodies are bounded, as application/x-www-form-urlencoded ones
    }
  } else {
    options.deserialize_check_constraints = false;
    options.serialize_check_constraints = true;
    options.deserialize_ignore_unknown_field = true; // server responses are trusted: newer fields are ignored
    options.deserialize_max_discard_char = -1;       // indented responses are accepted
  }
}

void
FormatPolicy<anch::json::JsonFormat>::translate(const std::exception& error) {
  const auto* mapping = dynamic_cast<const anch::json::MappingError*>(&error);
  if(mapping != nullptr && mapping->getErrorCode() == anch::json::ErrorCode::PAYLOAD_TOO_LARGE) {
    throw anch::codec::ContentTooLarge(mapping->what());
  }
}
