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
  std::string name;
  int32_t age;
  std::vector<std::string> tags;
  std::map<std::string,int64_t> scores;
  std::optional<std::string> color;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Pet>& mapper) {
  mapper
    .registerField("name", &Pet::name, anch::json::required, anch::json::minLength(1), anch::json::maxLength(32))
    .registerField("age", &Pet::age, anch::json::minimum(0), anch::json::maximum(50))
    .registerField<std::string>("tags", &Pet::tags, anch::json::maxItems(5), anch::json::uniqueItems,
                                anch::json::items(anch::json::minLength(1)))      // checked on every tag
    .registerField<int64_t>("scores", &Pet::scores, anch::json::items(anch::json::multipleOf(5)))
    .registerField<std::string>("color", &Pet::color, anch::json::allowed({"black", "white"})); // checked when not null
}

int
main() {
  for(const char* json : {R"({"name":"Rex","age":3,"tags":["dog",""]})",
                          R"({"name":"Rex","age":3,"scores":{"agility":12}})",
                          R"({"name":"Rex","age":-1})",
                          R"({"name":"Rex","color":"brown"})"}) {
    try {
      std::istringstream input(json);
      [[maybe_unused]] Pet pet = anch::json::deserialize<Pet>(input);
    } catch(const anch::json::MappingError& error) {
      std::cout << error.what() << std::endl;
    }
  }
  return 0;
}
//! [example]
