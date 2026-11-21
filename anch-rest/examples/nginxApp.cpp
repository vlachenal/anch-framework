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
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "json/json.hpp"
#include "rest/api.hpp"
#include "rest/jsonCodec.hpp"
#include "rest/nginxApplication.hpp"

struct Pet {
  uint64_t id = 0;
  std::string name;
};

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Pet>& mapper) {
  mapper
    .registerField("id", &Pet::id)
    .registerField("name", &Pet::name);
}

std::optional<Pet>
findPet(uint64_t id) {
  if(id != 1) {
    return std::nullopt;
  }
  return Pet{1, "Rex"};
}

// 'config' is named as in documentation without being used
#pragma GCC diagnostic ignored "-Wunused-parameter"

//! [nginx application]
std::unique_ptr<anch::rest::Api>
createApi(std::string_view config) { // config: second anch_rest_app argument
  auto api = std::make_unique<anch::rest::Api>();
  api->get("/api/pets/{id}", findPet).produces<anch::rest::Json>(); // routes see the whole URI
  return api;
}
ANCH_REST_APPLICATION(createApi)
//! [nginx application]
