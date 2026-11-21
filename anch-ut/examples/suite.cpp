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
#include "ut/unit.hpp"

#include <iostream>
#include "ut/assert.hpp"

void
startup() {
  std::cout << "Initialize tests suite" << std::endl;
}

void
before() {
  std::cout << "Before test" << std::endl;
}

void
after() {
  std::cout << "After test" << std::endl;
}

void
shutdown() {
  std::cout << "Uninitialize tests suite" << std::endl;
}

void
testNothing() {
  std::cout << "Nothing" << std::endl;
}

void
testException() {
  std::cout << "Exception" << std::endl;
  throw std::exception();
}

void
testAssert() {
  std::cout << "Assert" << std::endl;
  anch::ut::assertFalse(true, "ASSERT !!!");
}

void
testAssertFormat() {
  std::cout << "Assert" << std::endl;
  anch::ut::assertThat(false, "format message: int={}, float={}, str={}", 2, 3.14, "toto");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH unit tests")
    .description("Test AnCH unit tests library")
    .initialize(startup)
    .beforeTest(before)
    .afterTest(after)
    .uninitialize(shutdown)
    .add("nothing", testNothing)
    .add("exception", testException)
    .add("assert", testAssert)
    .add("assert-format", testAssertFormat)
    ;
}
//! [example]
