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
#include "cli/utils.hpp"
#include "cli/args.hpp"
#include "cli/formatter.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <optional>
#include <sstream>

#include <unistd.h>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::cli::Arg;
using anch::cli::ArgHandler;

// Formatter output operator has to be found from a namespace which declares its own operator<<
namespace other {
  struct Other {};
  std::ostream& operator<<(std::ostream& out, const Other&) {
    return out;
  }
  void
  print(std::ostream& out) {
    out << anch::cli::RESET << Other();
  }
}

/*!
 * Check that binding function raises std::invalid_argument
 */
template<typename T>
void
checkInvalid(T& dest, const T& initial, const std::string& value) {
  dest = initial;
  bool raised = false;
  try {
    anch::cli::bindNum(dest)(value);
  } catch(const std::invalid_argument&) {
    raised = true;
  }
  anch::ut::assertThat(raised, "'{}' should be refused", value);
  anch::ut::assertThat(dest == initial, "Value has been modified by '{}'", value);
}

void
testNumbers() {
  uint16_t port = 42;
  checkInvalid<uint16_t>(port, 42, "70000"); // out of range
  checkInvalid<uint16_t>(port, 42, "");      // empty
  checkInvalid<uint16_t>(port, 42, "-1");
  checkInvalid<uint16_t>(port, 42, "12a");
  anch::cli::bindNum(port)("65535");
  anch::ut::assertThat(port == 65535, "port is {}", port);
  int16_t i16 = 0;
  checkInvalid<int16_t>(i16, 0, "-32769");
  anch::cli::bindNum(i16)("-32768");
  anch::ut::assertThat(i16 == -32768, "i16 is {}", i16);
  double d = 0.0;
  checkInvalid<double>(d, 0.0, "1e400");
  anch::cli::bindNum(d)("3.5");
  anch::ut::assertThat(d == 3.5, "d is {}", d);
}

void
testOutputFile() {
  std::shared_ptr<std::ostream> out;
  std::shared_ptr<std::ofstream> ofs;
  bool raised = false;
  try {
    anch::cli::bindOFS(out)("does-not-exist/out.txt");
  } catch(const std::invalid_argument&) {
    raised = true;
  }
  anch::ut::assertTrue(raised && !out, "Output file in missing directory should be refused");
  raised = false;
  try {
    anch::cli::bindOFS(ofs, std::ios_base::app)("does-not-exist/out.txt");
  } catch(const std::invalid_argument&) {
    raised = true;
  }
  anch::ut::assertTrue(raised && !ofs, "Output file (with mode) in missing directory should be refused");
  anch::cli::bindOFS(out)("utils-out.txt");
  anch::ut::assertTrue(out && out->good(), "Output file should be opened");
}

void
testRepeatedFlag() {
  bool verbose = false;
  ArgHandler handler(std::vector<Arg>{{.handler = anch::cli::bindTrue(verbose), .sopt = 'v', .lopt = "verbose"}});
  char prog[] = "prog";
  char lopt[] = "--verbose";
  char sopt[] = "-v";
  char* argv[] = {prog, lopt, sopt, lopt, NULL};
  handler.parse(4, argv);
  anch::ut::assertTrue(verbose, "verbose should be set");
}

void
testRepeatedValue() {
  std::optional<std::string> value;
  ArgHandler handler(std::vector<Arg>{{.handler = anch::cli::bindStr(value), .sopt = 'p', .lopt = "plop", .value = true}});
  char prog[] = "prog";
  char lopt[] = "--plop";
  char v1[] = "a";
  char v2[] = "b";
  char* argv[] = {prog, lopt, v1, lopt, v2, NULL};
  try {
    handler.parse(5, argv);
  } catch(const std::invalid_argument& e) {
    std::string msg = e.what();
    anch::ut::assertThat(msg == "Argument -p|--plop has already been found", "Unexpected message: '{}'", msg);
    return;
  }
  anch::ut::fail("Repeated value option should be refused");
}

void
testMultiPositionalLast() {
  std::vector<std::string> multi;
  std::optional<std::string> last;
  try {
    ArgHandler handler(std::vector<Arg>{
	{.handler = anch::cli::bindCol(multi), .value = true, .name = "M", .multi = true},
	{.handler = anch::cli::bindStr(last), .value = true, .name = "LAST"}
      });
  } catch(const std::invalid_argument&) {
    return;
  }
  anch::ut::fail("Multiple positional argument which is not the last one should be refused");
}

void
testUserNoTermFmt() {
  // A user defined --no-term-fmt option was replaced by a mandatory positional argument
  bool noFmt = false;
  ArgHandler handler(std::vector<Arg>{{.handler = anch::cli::bindTrue(noFmt), .lopt = "no-term-fmt", .description = "user option"}});
  char prog[] = "prog";
  char* argv[] = {prog, NULL};
  handler.handle(1, argv); // exits with failure when an argument is missing
  std::ostringstream help;
  handler.printHelp(help);
  anch::ut::assertThat(help.str().find("Usage: prog [OPTIONS]\n") != std::string::npos, "Unexpected usage: {}", help.str());
}

void
testFormatterNoTerminal() {
  std::ostringstream out;
  other::print(out);
  if(::isatty(STDOUT_FILENO) == 0) {
    anch::ut::assertTrue(anch::cli::Formatter::DISABLED, "Formatting should be disabled when output is not a terminal");
    anch::ut::assertTrue(out.str().empty(), "Nothing should be written when formatting is disabled");
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH CLI utils unit tests")
    .description("Test AnCH CLI binding functions and argument handler")
    .add("numbers", testNumbers)
    .add("output-file", testOutputFile)
    .add("repeated-flag", testRepeatedFlag)
    .add("repeated-value", testRepeatedValue)
    .add("multi-positional-last", testMultiPositionalLast)
    .add("user-no-term-fmt", testUserNoTermFmt)
    .add("formatter-no-terminal", testFormatterNoTerminal)
    ;
}
