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
 * UUID parser fuzzer: an accepted UUID is written in lower case and parsed again to the same value.
 */
#include "uuid.hpp"

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {
  const std::string input(reinterpret_cast<const char*>(data), size);
  try {
    const anch::UUID uuid(input);
    const std::string str = uuid.toString();
    if(str.size() != 36 || anch::UUID(str) != uuid) {
      std::abort();
    }
    for(std::size_t i = 0 ; i < str.size() ; ++i) {
      if(std::tolower(static_cast<unsigned char>(input[i])) != str[i]) {
	std::abort(); // string is the input in lower case
      }
    }
  } catch(const std::invalid_argument&) {
    // invalid UUID
  }
  return 0;
}
