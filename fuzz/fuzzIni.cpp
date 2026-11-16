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
 * INI parser fuzzer: input is written in a temporary file (anch-ini reads files), every value is read as string,
 * number and boolean.
 */
#include "ini/parser.hpp"
#include "ini/parserError.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

namespace {

  void
  readValues(const anch::ini::Section& section) {
    for(const auto& [key, value] : section.getValues()) {
      (void) section.getValue<std::string>(key);
      try {
	(void) section.getValue<int64_t>(key);
      } catch(const anch::ini::ParserError&) {
	// not a number
      }
      try {
	(void) section.getValue<bool>(key);
      } catch(const anch::ini::ParserError&) {
	// not a boolean
      }
    }
    for(const auto& [name, sub] : section.getSections()) {
      readValues(sub);
    }
  }

}

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {
  static const std::filesystem::path path = std::filesystem::temp_directory_path()
    / ("anch-fuzz-ini-" + std::to_string(::getpid()) + ".ini");
  {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
  }
  try {
    readValues(anch::ini::parse(path));
  } catch(const anch::ini::ParserError&) {
    // invalid file
  }
  return 0;
}
