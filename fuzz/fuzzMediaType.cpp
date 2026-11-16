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
/*
 * Media type and Accept header parsers fuzzer: an accepted media type is written and parsed again to the same
 * essence.
 */
#include "rest/mediaType.hpp"

#include <cstdint>
#include <cstdlib>
#include <optional>
#include <string_view>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {
  const std::string_view input(reinterpret_cast<const char*>(data), size);
  std::optional<anch::rest::MediaType> type = anch::rest::MediaType::parse(input);
  if(type) {
    std::optional<anch::rest::MediaType> again = anch::rest::MediaType::parse(type->toString());
    if(!again || again->essence() != type->essence()) {
      std::abort();
    }
  }
  (void) anch::rest::MediaType::parseAccept(input);
  return 0;
}
