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
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "json/json.hpp"
#include "val/validation.hpp"

//! [example]
struct Pet {
  std::string name;
  int32_t age = 0;
  std::vector<std::string> tags;
};

struct Shelter {
  std::vector<Pet> pets;
};

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("name", &Pet::name, anch::val::required, anch::val::minLength(1))
      .registerField("age", &Pet::age, anch::val::minimum(0))
      .registerField("tags", &Pet::tags, anch::val::items(anch::val::minLength(1)));
  }
};

template<>
struct anch::Mapping<Shelter> {
  static void
  registerObject(anch::ObjectMapper<Shelter> auto& mapper) {
    mapper
      .registerField("pets", &Shelter::pets, anch::val::maxItems(10));
  }
};

int
main() {
  Shelter shelter{{Pet{"Rex", 3, {"dog"}}, Pet{"", -1, {"cat", ""}}}};
  for(const anch::val::Violation& violation : anch::val::validate(shelter)) { // every violation
    std::cout << violation.path << ": " << violation.message << std::endl;
  }
  return 0;
}
//! [example]
