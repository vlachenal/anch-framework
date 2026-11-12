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
#include "conf/placeholders.hpp"
#include "conf/resolvers.hpp"

#include <cstdlib>
#include <functional>
#include <string>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::conf::ConfError;
using anch::conf::Placeholders;
using anch::conf::Resolvers;
using anch::conf::Section;

// Helpers +
namespace {

  std::string
  get(const Section& root, const std::string& path) {
    return Section::getValue(path, root).value_or("<none>");
  }

  /*! Resolve and expect an error code, return the error message */
  std::string
  expectError(Section& root, ConfError::ErrorCode code, std::size_t maxDepth = Placeholders::DEFAULT_MAX_DEPTH) {
    try {
      Placeholders(root, maxDepth).resolve();
    } catch(const ConfError& e) {
      anch::ut::assertThat(e.getErrorCode() == code, "Unexpected error code {} ({})", static_cast<int>(e.getErrorCode()), e.what());
      return e.what();
    }
    anch::ut::assertThat(false, "ConfError expected");
    return std::string();
  }

}
// Helpers -

void
startup() {
  ::setenv("ANCH_CONF_TEST_VAR", "value", 1);
  ::setenv("ANCH_CONF_TEST_PLACEHOLDER", "${conf=a}", 1);
  ::unsetenv("ANCH_CONF_TEST_UNSET");
  Resolvers::getInstance().registerResolver([](const std::string& name, const std::optional<std::string>&) -> std::optional<std::string> {
    std::string res(name);
    for(char& c : res) {
      c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return res;
  }, "upper");
}

void
testAlternatives() {
  Section root;
  root.putValue("env", "${env=ANCH_CONF_TEST_VAR}");
  root.putValue("default", "${env=ANCH_CONF_TEST_UNSET| fallback value }");
  root.putValue("first", "${env=ANCH_CONF_TEST_VAR|fallback}");
  root.putValue("custom", "${upper=abc}");
  root.putValue("plain", "no placeholder");
  std::size_t count = Placeholders(root).resolve();
  anch::ut::assertThat(count == 4, "Expected 4 resolved values, found {}", count);
  anch::ut::assertThat(get(root, "env") == "value", "Unexpected env '{}'", get(root, "env"));
  anch::ut::assertThat(get(root, "default") == "fallback value", "Unexpected default '{}'", get(root, "default"));
  anch::ut::assertThat(get(root, "first") == "value", "First resolved alternative has to be used");
  anch::ut::assertThat(get(root, "custom") == "ABC", "Unexpected custom resolver value '{}'", get(root, "custom"));
  anch::ut::assertThat(get(root, "plain") == "no placeholder", "Plain value has to be kept");
}

void
testPropagation() {
  Section root;
  // Declared before their dependencies: resolved by propagation
  root.putValue("a", "${conf=b}/a");
  root.section("srv").putValue("url", "http://${conf=srv.host}:${conf=srv.port}/${conf=a}");
  root.putValue("b", "${conf=c}/b");
  root.putValue("c", "${env=ANCH_CONF_TEST_VAR}");
  root.section("srv").putValue("host", "${env=ANCH_CONF_TEST_UNSET|localhost}");
  root.section("srv").putValue("port", "8080");
  Placeholders(root).resolve();
  anch::ut::assertThat(get(root, "a") == "value/b/a", "Unexpected a '{}'", get(root, "a"));
  anch::ut::assertThat(get(root, "srv.url") == "http://localhost:8080/value/b/a", "Unexpected url '{}'", get(root, "srv.url"));
}

void
testMissing() {
  Section root;
  root.putValue("a", "${conf=missing|default}");
  Placeholders(root).resolve();
  anch::ut::assertThat(get(root, "a") == "default", "Missing key has to use next alternative");
  Section error;
  error.putValue("a", "x${conf=missing}");
  std::string message = expectError(error, ConfError::ErrorCode::NOT_RES);
  anch::ut::assertThat(message.find("${conf=missing} in a") != std::string::npos, "Unexpected message '{}'", message);
}

void
testCycle() {
  Section root;
  root.putValue("tail", "${conf=a}");
  root.putValue("a", "${conf=b}");
  root.putValue("b", "x${conf=a}");
  std::string message = expectError(root, ConfError::ErrorCode::CYCLE);
  anch::ut::assertThat(message.ends_with("a -> b -> a"), "Unexpected message '{}'", message);
  Section self;
  self.putValue("a", "${conf=a|default}");
  message = expectError(self, ConfError::ErrorCode::CYCLE);
  anch::ut::assertThat(message.ends_with("a -> a"), "Unexpected message '{}'", message);
}

void
testWatchdog() {
  Section root;
  for(int i = 0 ; i < 10 ; ++i) {
    root.putValue("k" + std::to_string(i), "${conf=k" + std::to_string(i + 1) + "}");
  }
  root.putValue("k10", "end");
  Section copy = root;
  std::string message = expectError(root, ConfError::ErrorCode::CYCLE, 5);
  anch::ut::assertThat(message.find("longer than 5") != std::string::npos, "Unexpected message '{}'", message);
  Placeholders(copy, 10).resolve();
  anch::ut::assertThat(get(copy, "k0") == "end", "Chain of 10 has to be resolved with maximum depth 10");
}

void
testLiteral() {
  Section root;
  root.putValue("a", "A");
  root.putValue("b", "${env=ANCH_CONF_TEST_PLACEHOLDER}");
  Placeholders(root).resolve();
  anch::ut::assertThat(get(root, "b") == "${conf=a}", "Resolved values must not be interpreted again: '{}'", get(root, "b"));
}

void
testSyntax() {
  Section unknown;
  unknown.putValue("a", "${vault=secret}");
  expectError(unknown, ConfError::ErrorCode::RES_NOT_FOUND);
  Section nested;
  nested.putValue("a", "${conf=${env=X}}");
  expectError(nested, ConfError::ErrorCode::PARSING_ERROR);
  Section unclosed;
  unclosed.putValue("a", "${env=X");
  expectError(unclosed, ConfError::ErrorCode::PARSING_ERROR);
  Section empty;
  empty.putValue("a", "${env=X||default}");
  expectError(empty, ConfError::ErrorCode::PARSING_ERROR);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH conf placeholders resolution unit tests")
    .description("Test event driven placeholders resolution and watchdog")
    .initialize(startup)
    .add("alternatives", testAlternatives)
    .add("propagation", testPropagation)
    .add("missing", testMissing)
    .add("cycle", testCycle)
    .add("watchdog", testWatchdog)
    .add("literal", testLiteral)
    .add("syntax", testSyntax)
    ;
}
