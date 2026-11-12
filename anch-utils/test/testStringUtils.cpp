/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2022 Vincent Lachenal

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

#include "stringUtils.hpp"

#include "ut/unit.hpp"
#include "ut/assert.hpp"

std::string orig;

void
beforeEach() {
  orig = "PLip plOP";
  std::cout << "Original: " << orig << std::endl;
}

void
testToUpper() {
  std::cout << "Enter in toUpper test" << std::endl;
  anch::toUpper(orig);
  std::cout << "toUpper: " << orig << std::endl;
  if(orig != "PLIP PLOP") {
    std::cerr << "toUpper does not work: " << orig << std::endl;
    anch::ut::fail();
  }
  std::cout << "Exit toUpper test" << std::endl;
}

void
testToLower() {
  std::cout << "Enter in toLower test" << std::endl;
  anch::toUpper(orig);
  std::cout << "toUpper: " << orig << std::endl;
  if(orig != "PLIP PLOP") {
    std::cerr << "toLower does not work: " << orig << std::endl;
    anch::ut::fail();
  }
  std::cout << "Exit toLower test" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH string utils unit tests")
    .description("Test AnCH string utils utiliy class")
    .beforeTest(beforeEach)
    .add("to-upper", testToUpper)
    .add("to-lower", testToLower)
    ;
}
