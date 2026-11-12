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
#include "conf/configuration.hpp"

#include <cstdlib>
#include <string>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::conf::Configuration;

const Configuration* conf;

void
startup() {
  ::unsetenv("ANCH_PROFILES"); // default profiles are used
  conf = &Configuration::loader().name("cascade").load();
}

void
check(const std::string& path, const std::string& expected) {
  std::optional<std::string> val = conf->value(path);
  anch::ut::assertThat(val.value_or("<none>") == expected, "{}: found '{}' instead of '{}'", path, val.value_or("<none>"), expected);
}

void
testIncludes() {
  check("name", "main");
  check("inc", "yes");
  check("db", "yes");
}

void
testProfiles() {
  check("p1", "yes");
  check("server.port", "8080"); // included file with profile
  // main < includes < main-p1 < includes-p1 < main-p2
  check("over", "p2");
}

void
testValues() {
  check("spaced", "ma valeur");
  check("url", "http://main-host:8080");
}

void
testSection() {
  anch::ut::assertThat(conf->section("a.b.c") != NULL && conf->section("a.b.c")->getValue<std::string>("x") == "1", "a.b.c section expected");
  anch::ut::assertThat(conf->section("a.b.d") == NULL && conf->section("server") != NULL && conf->section(".") != NULL, "Unexpected sections");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH conf cascade unit tests")
    .description("Test includes, default profiles and profiles order")
    .initialize(startup)
    .add("includes", testIncludes)
    .add("profiles", testProfiles)
    .add("values", testValues)
    .add("section", testSection)
    ;
}
