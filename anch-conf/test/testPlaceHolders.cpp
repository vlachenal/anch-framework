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
#include <cstdlib>
#include <iostream>

#include "conf/configuration.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"


using anch::conf::Configuration;


const Configuration* conf;

void
startup() {
  ::setenv("ANCH_CONF_TEST_USER", "anch", 1);
  ::unsetenv("ANCH_CONF_TEST_UNSET");
  conf = &Configuration::loader()
    .name("test-placeholders")
    .load();
}

void
check(const std::string& path, const std::string& expected) {
  std::optional<std::string> val = conf->value(path);
  anch::ut::assertThat(val.has_value(), "{} not found", path);
  anch::ut::assertThat(val.value() == expected, "{}: found '{}' instead of '{}'", path, val.value(), expected);
}

void
parseSingle() {
  check("TOTO.toto", "anch");
  check("TOTO.tata", "http://localhost/anch");
  check("TOTO.titi", "http://localhost/anch/toto/tata");
}

void
parseAlternatives() {
  check("TATA.toto", "vlachenal");
  check("TATA.tata", "http://localhost/vlachenal");
  check("TATA.titi", "http://localhost/anch/toto/tata");
  check("TATA.url", "http://localhost/anch/toto/tata/api");
}

void
loaded() {
  anch::ut::assertThat(&Configuration::inst() == conf, "Configuration has to be loaded after placeholders resolution");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH conf placeholders unit tests")
    .description("Test AnCH conf placeholders")
    .initialize(startup)
    .add("single", parseSingle)
    .add("alternatives", parseAlternatives)
    .add("loaded", loaded)
    ;
}
