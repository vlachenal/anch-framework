/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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
//! [example]
#include "json/json.hpp"

#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

struct Pet {
  uint64_t id;
  std::string name;
  std::vector<std::string> tags;
  std::optional<std::string> owner;
};

// Declare serialized fields (once per type)
template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id)
      .registerField("name", &Pet::name)
      .registerField("tags", &Pet::tags) // container: element type is given
      .registerField("owner", &Pet::owner); // std::optional: value type is given too
  }
};

int
main() {
  Pet pet{1, "Rex", {"dog", "good boy"}, std::nullopt};
  std::string json = anch::json::serialize(pet);
  std::cout << json << std::endl;

  std::istringstream input(R"({"id":2,"name":"Médor","tags":["cat"],"owner":"Vincent"})");
  Pet parsed = anch::json::deserialize<Pet>(input);     // anch::json::MappingError when invalid
  std::cout << parsed.name << " " << parsed.owner.value_or("-") << std::endl;
  return 0;
}
//! [example]
