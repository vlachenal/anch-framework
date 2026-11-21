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
 * JSON reader fuzzer: objects with every kind of field, strict and lenient options, buffer sizes from 1 to 64
 * (refill). An accepted document is written again: writing then reading it gives the same JSON (round trip).
 */
#include "json/json.hpp"

#include <cstdint>
#include <cstdlib>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {

  struct Child {
    int32_t id = 0;
    std::optional<std::string> name;
  };

  struct Document {
    std::string text;
    int64_t integer = 0;
    uint16_t small = 0;
    double real = 0.0;
    bool flag = false;
    std::optional<std::string> optional;
    std::vector<int32_t> numbers;
    std::vector<std::string> strings;
    std::map<std::string, Child> children;
    std::vector<Child> list;
    std::optional<Child> child;
  };

}

template<>
struct anch::Mapping<Child> {
  static void
  registerObject(anch::ObjectMapper<Child> auto& mapper) {
    mapper
      .registerField("id", &Child::id, anch::val::required)
      .registerField("name", &Child::name, anch::val::maxLength(16))
      ;
  }
};

template<>
struct anch::Mapping<Document> {
  static void
  registerObject(anch::ObjectMapper<Document> auto& mapper) {
    mapper
      .registerField("text", &Document::text)
      .registerField("integer", &Document::integer)
      .registerField("small", &Document::small, anch::val::minimum(1))
      .registerField("real", &Document::real)
      .registerField("flag", &Document::flag)
      .registerField("optional", &Document::optional)
      .registerField("numbers", &Document::numbers)
      .registerField("strings", &Document::strings)
      .registerField("children", &Document::children)
      .registerField("list", &Document::list)
      .registerField("child", &Document::child)
      ;
  }
};

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {
  if(size == 0) {
    return 0;
  }
  // First byte: options ; others: document
  const uint8_t control = data[0];
  anch::json::MappingOptions options = anch::json::DEFAULT_MAPPING_OPTIONS;
  options.buffer_size = 1 + (control & 0x3F);
  if(control & 0x40) {
    options.deserialize_ignore_unknown_field = true;
    options.deserialize_max_discard_char = -1;
  }
  options.deserialize_check_constraints = (control & 0x80) != 0;
  options.max_depth = 64;
  const std::string input(reinterpret_cast<const char*>(data) + 1, size - 1);
  Document document;
  try {
    std::istringstream in(input);
    anch::json::deserialize(document, in, options);
  } catch(const anch::json::MappingError&) {
    return 0; // invalid document
  }
  // Round trip: written JSON is read again and written identically
  anch::json::MappingOptions write = anch::json::DEFAULT_MAPPING_OPTIONS;
  const std::string first = anch::json::serialize(document, write);
  Document copy;
  std::istringstream again(first);
  anch::json::MappingOptions read = anch::json::DEFAULT_MAPPING_OPTIONS;
  read.deserialize_check_constraints = false;
  try {
    anch::json::deserialize(copy, again, read);
  } catch(const anch::json::MappingError&) {
    std::abort(); // written JSON has to be readable
  }
  if(anch::json::serialize(copy, write) != first) {
    std::abort(); // round trip has to keep values
  }
  return 0;
}
