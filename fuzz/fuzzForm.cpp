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
 * application/x-www-form-urlencoded reader fuzzer (server limits).
 */
#include "codec/codec.hpp"
#include "codec/mediaType.hpp"

#include "rest/form.hpp"
#include "rest/formCodec.hpp"

#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {
  static const anch::codec::MediaType type = *anch::codec::MediaType::parse("application/x-www-form-urlencoded");
  std::istringstream input(std::string(reinterpret_cast<const char*>(data), size));
  anch::rest::Form form;
  try {
    anch::rest::FormUrlEncoded::deserialize(form, input, type, anch::codec::Side::SERVER);
  } catch(const std::invalid_argument&) {
    // malformed body
  } catch(const anch::codec::ContentTooLarge&) {
    // over limits
  }
  return 0;
}
