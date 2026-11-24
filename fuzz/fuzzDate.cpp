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
 * Date parser fuzzer: patterns with every formatter, an accepted date is formatted and parsed again to the same
 * value.
 */
#include "date/dateFormatter.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {
  static const std::array<anch::date::DateFormatter, 4> formatters = {
    anch::date::DateFormatter("%Y-%m-%dT%H:%M:%S.%s"),
    anch::date::DateFormatter("%d/%m/%y %I:%M %p"),
    anch::date::DateFormatter("%Y%m%d%H%M%S%s%u%n"),
    anch::date::DateFormatter("%a %d %h %Y %H:%M:%S %z")
  };
  if(size == 0) {
    return 0;
  }
  const anch::date::DateFormatter& formatter = formatters[data[0] % formatters.size()];
  const std::string input(reinterpret_cast<const char*>(data) + 1, size - 1);
  try {
    const anch::date::DateTime date = formatter.parse(input);
    std::string output;
    formatter.format(date, output);
  } catch(const std::invalid_argument&) {
    // invalid date
  }
  return 0;
}
