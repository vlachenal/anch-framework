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
// anch-utils documentation examples (anch-utils.dox): objects description
#include "mapping.hpp"

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

//! [mapping]
struct Pet {
  uint64_t id;
  std::string name;
  std::string nickname() const { return "little " + name; }
};

// One description of Pet for every format
template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id)
      .registerField("name", &Pet::name)
      .registerField("nickname", std::function<std::string(const Pet&)>(&Pet::nickname)) // written only
      ;
  }
};

// Types of other libraries can be described too
template<>
struct anch::Mapping<std::pair<std::string, int>> {
  static void
  registerObject(anch::ObjectMapper<std::pair<std::string, int>> auto& mapper) {
    mapper
      .registerField("key", &std::pair<std::string, int>::first)
      .registerField("value", &std::pair<std::string, int>::second)
      ;
  }
};
//! [mapping]

//! [mapper]
// Mapper of a format: here, it lists the fields of a description
template<typename T>
class FieldsPrinter {
public:
  template<typename P, typename... C>
  FieldsPrinter& registerField(const std::string& name, P T::*, C&&...) {
    std::cout << name << " (member)" << std::endl;
    return *this;
  }

  template<typename P, typename... C>
  FieldsPrinter& registerField(const std::string& name, std::function<P(const T&)>, C&&...) {
    std::cout << name << " (getter)" << std::endl;
    return *this;
  }

  template<typename P, typename... C>
  FieldsPrinter& registerField(const std::string& name, std::function<void(T&, const P&)>, C&&...) {
    std::cout << name << " (setter)" << std::endl;
    return *this;
  }

  template<typename P, typename... C>
  FieldsPrinter& registerField(const std::string& name, std::function<P(const T&)>, std::function<void(T&, const P&)>, C&&...) {
    std::cout << name << " (getter and setter)" << std::endl;
    return *this;
  }
};
static_assert(anch::ObjectMapper<FieldsPrinter<Pet>, Pet>);
//! [mapper]

int
main() {
  //! [describe]
  static_assert(anch::Described<Pet> && !anch::Described<int>);
  FieldsPrinter<Pet> printer;
  anch::describe<Pet>(printer); // id (member), name (member), nickname (getter)
  //! [describe]
  FieldsPrinter<std::pair<std::string, int>> pairPrinter;
  anch::describe<std::pair<std::string, int>>(pairPrinter);
  return 0;
}
