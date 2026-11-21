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
#include <iostream>
#include <sstream>

//! [models]
#include "petstore.hpp"
#include "json/json.hpp"

void
readPet(std::istream& input) {
  petstore::model::Pet pet = anch::json::deserialize<petstore::model::Pet>(input);
  std::cout << pet.id << " " << pet.name << std::endl;
}
//! [models]

int
main() {
  std::istringstream input(R"({"id":1,"name":"Rex","status":"available"})");
  readPet(input);
  return 0;
}
