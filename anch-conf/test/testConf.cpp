/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2025 Vincent Lachenal

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

#include "conf/configuration.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"


using anch::conf::Configuration;


const Configuration* conf;

void
startup() {
  std::cout << "Parse file" << std::endl;
  conf = &Configuration::loader()
    .name("test")
    .load();
  std::cout << "File has been parsed" << std::endl;
}

void
getToto() {
  std::cout << "Execute getToto" << std::endl;
  std::optional<std::string> val = conf->value("toto");
  anch::ut::assertTrue(val.has_value(), "toto not found");
  std::cout << "toto=" << val.value() << std::endl;
  anch::ut::assertThat(val.value() == "tata", "Found {} instead of {}", val.value(), "tata");
}

void
getTOTOToto() {
  std::cout << "Execute getTOTOToto" << std::endl;
  std::optional<std::string> val = conf->value("TOTO.toto");
  anch::ut::assertTrue(val.has_value(), "TOTO/toto not found");
  std::cout << "TOTO.toto=" << val.value() << std::endl;
  anch::ut::assertThat(val.value() == "titi", "Found {} instead of {}", val.value(), "titi");
}

void
getTata() {
  std::cout << "Execute getTata" << std::endl;
  std::optional<std::string> val = conf->value("tata");
  anch::ut::assertTrue(val.has_value(), "tata not found");
  std::cout << "tata=" << val.value() << std::endl;
  anch::ut::assertThat(val.value() == "tutu", "Found {} instead of {}", val.value(), "tutu");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH conf unit tests")
    .description("Test AnCH conf library")
    .initialize(startup)
    .add("get-toto", getToto)
    .add("get-TOTO-toto", getTOTOToto)
    .add("get-tata", getTata)
    ;
}
