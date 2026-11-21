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
#include <optional>
#include <string>
#include <vector>

#include "json/json.hpp"

struct Pet {
  uint64_t id;
  std::optional<std::string> owner;
  std::vector<std::string> tags;
};

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    //! [required]
    mapper
      .registerField("id", &Pet::id, anch::val::required)
      .registerField("owner", &Pet::owner, anch::val::required); // std::optional: present, can be null
    //! [required]
    //! [items]
    mapper.registerField("tags", &Pet::tags, anch::val::maxItems(10), anch::val::items(anch::val::minLength(1)));
    //! [items]
  }
};

int
main() {
  //! [client options]
  anch::json::MappingOptions client;
  client.serialize_check_constraints = true;    // invalid requests are not sent
  client.deserialize_check_constraints = false; // responses are trusted
  //! [client options]
  std::cout << anch::json::serialize(Pet{1, std::nullopt, {"dog"}}, client) << std::endl;
  Pet pet{2, "Vincent", {"cat", ""}};
  std::size_t nb = 0;
  //! [validate]
  for(const anch::val::Violation& violation : anch::json::validate(pet)) {
    std::cout << violation.path << ": " << violation.message << std::endl;
  }
  //! [validate]
  for([[maybe_unused]] const anch::val::Violation& violation : anch::json::validate(pet)) {
    ++nb;
  }
  return nb == 1 ? 0 : 1; // empty tag
}
