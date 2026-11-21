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
// anch-codec documentation examples (anch-codec.dox)
#include "codec/all.hpp"
#include "codec/format.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "json/json.hpp"

//! [codec]
// text/csv codec of a list of numbers: one serialize / deserialize form is enough
struct NumbersCsv {
  static std::string_view
  mediaType() noexcept {
    return "text/csv";
  }

  static void
  serialize(const std::vector<int>& values, std::ostream& out) {
    for(std::size_t i = 0; i < values.size(); ++i) {
      out << (i == 0 ? "" : ",") << values[i];
    }
  }

  static void
  deserialize(std::vector<int>& values, std::istream& input, anch::codec::Side side) {
    // Data which is not controlled ( SERVER ) is bounded
    std::istringstream content(anch::codec::readBody(input, side));
    for(std::string value; std::getline(content, value, ',');) {
      values.push_back(std::stoi(value));
    }
  }
};
static_assert(anch::codec::Serializer<NumbersCsv, std::vector<int>>);
//! [codec]

//! [model]
struct Pet {
  uint64_t id;
  std::string name;
};

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id)
      .registerField("name", &Pet::name, anch::val::required, anch::val::minLength(1));
  }
};
//! [model]

int
main() {
  //! [serialize]
  // Content type first (a codec can add parameters), then serialization with the trust policy of the side
  const Pet pet{1, "Rex"};
  const anch::codec::MediaType type = anch::codec::contentTypeOf<anch::codec::Json>(pet);
  std::ostringstream out;
  anch::codec::serialize<anch::codec::Json>(pet, out, type, anch::codec::Side::CLIENT);
  std::cout << type.toString() << ' ' << out.str() << std::endl; // application/json {"id":1,"name":"Rex"}
  //! [serialize]

  //! [codecs]
  // Codecs selected at run time among a list resolved at compilation
  using Codecs = anch::codec::Codecs<anch::codec::Json, NumbersCsv, anch::codec::Text>;
  std::ostringstream csv;
  std::optional<anch::codec::MediaType> selected =
    Codecs::serialize(std::vector<int>{1, 2, 3}, csv, anch::codec::MediaType::parseAccept("text/csv, */*;q=0.1"),
		      anch::codec::Side::SERVER);
  std::cout << selected->essence() << ' ' << csv.str() << std::endl; // text/csv 1,2,3

  std::vector<int> numbers;
  std::istringstream input("4,5");
  Codecs::deserialize(numbers, input, *anch::codec::MediaType::parse("text/csv; charset=utf-8"), anch::codec::Side::SERVER);
  //! [codecs]

  //! [available]
  // Codecs of the formats built with the framework (codec/all.hpp, target anch::codec-all)
  Pet read;
  std::istringstream json(R"({"id":2,"name":"Médor"})");
  if(!anch::codec::Available::deserialize(read, json, anch::codec::MediaType("application", "json"), anch::codec::Side::SERVER)) {
    std::cerr << "Unsupported media type" << std::endl;
  }
  // Formats registered by the plugins the program uses
  for(const anch::codec::Format& format : anch::codec::formats()) {
    std::cout << format.name << ':';
    for(const std::string& mediaType : format.mediaTypes) {
      std::cout << ' ' << mediaType;
    }
    std::cout << std::endl;
  }
  //! [available]
  return (read.id == 2 && numbers.size() == 2 && csv.str() == "1,2,3") ? 0 : 1;
}
