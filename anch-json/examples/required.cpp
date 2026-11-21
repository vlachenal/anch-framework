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

//! [example]
struct Pet {
  uint64_t id;
  std::string name;
  std::optional<std::string> owner;
};

struct Shelter {
  std::vector<Pet> pets;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Pet>& mapper) {
  mapper
    .registerField("id", &Pet::id, anch::json::required)
    .registerField("name", &Pet::name, anch::json::required)
    .registerField<std::string>("owner", &Pet::owner, anch::json::required); // has to be present, can be null
}

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Shelter>& mapper) {
  mapper
    .registerField<Pet>("pets", &Shelter::pets, anch::json::required);
}

int
main() {
  for(const char* json : {R"({"pets":[{"id":1,"name":"Rex","owner":null},{"id":2,"owner":"Vincent"}]})",
                          R"({"pets":[{"id":1,"name":null,"owner":null}]})"}) {
    try {
      std::istringstream input(json);
      [[maybe_unused]] Shelter shelter = anch::json::deserialize<Shelter>(input);
    } catch(const anch::json::MappingError& error) {
      std::cout << error.getPath() << " -> " << error.what() << std::endl;
    }
  }
  return 0;
}
//! [example]
