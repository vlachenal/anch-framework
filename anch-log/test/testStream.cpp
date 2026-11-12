/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "log/stream.hpp"
#include "log/writer.hpp"
#include "log/mdc.hpp"

#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "conf/section.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::log::Level;
using anch::log::LogLine;
using anch::log::Logger;
using anch::log::MDC;
using anch::log::NoopStream;
namespace lvl = anch::log::lvl;

// Helpers +
namespace {

  /*! Writer which keeps messages and their MDC level */
  class StringWriter: public anch::log::Writer {
  public:
    std::vector<std::string> messages;
    std::vector<std::string> levels;

    StringWriter(): anch::log::Writer(anch::conf::Section()) {
    }

    void
    write(const std::string& message) override {
      messages.push_back(message);
      levels.push_back(MDC.get().at("level"));
    }
  };

  /*! Writer which always fails */
  class ThrowingWriter: public anch::log::Writer {
  public:
    ThrowingWriter(): anch::log::Writer(anch::conf::Section()) {
    }

    void
    write(const std::string&) override {
      throw std::runtime_error("write error");
    }
  };

  /*! Type whose formatting fails */
  struct Unformattable {};

  std::ostream&
  operator<<(std::ostream&, const Unformattable&) {
    throw std::runtime_error("format error");
  }

  int calls = 0;

  int
  count() {
    return ++calls;
  }

}
// Helpers -

// Messages which must be written use FATAL level: it is never removed at compile time, whatever ANCH_LOG_MIN_LEVEL is

void
testRuntimeLevel() {
  StringWriter writer;
  Logger logger("stream", Level::FATAL, {&writer});
  logger << lvl::error << "filtered";
  anch::ut::assertThat(writer.messages.empty(), "ERROR message must be filtered, {} message(s) written", writer.messages.size());
  logger << lvl::fatal << "value=" << 42 << ' ' << 1.5;
  logger << lvl::fatal << "last";
  anch::ut::assertThat(writer.messages == std::vector<std::string>({"value=42 1.5", "last"}), "Unexpected messages: {}", writer.messages.size());
  anch::ut::assertThat(writer.levels == std::vector<std::string>({"FATAL", "FATAL"}), "Unexpected MDC levels");
}

void
testManipulators() {
  StringWriter writer;
  Logger logger("stream", Level::TRACE, {&writer});
  logger << lvl::fatal << std::hex << 255 << std::endl;
  logger << lvl::fatal << 255;
  anch::ut::assertThat(writer.messages == std::vector<std::string>({"ff\n", "255"}), "Unexpected messages: {}", writer.messages.size());
}

void
testFormatError() {
  StringWriter writer;
  Logger logger("stream", Level::TRACE, {&writer});
  logger << lvl::fatal << "before " << Unformattable{} << " after";
  anch::ut::assertThat(writer.messages.empty(), "Failed message must be dropped");
  logger << lvl::fatal << "next";
  anch::ut::assertThat(writer.messages == std::vector<std::string>({"next"}), "Next message must be written");
}

void
testWriterError() {
  StringWriter writer;
  ThrowingWriter throwing;
  Logger logger("stream", Level::TRACE, {&throwing, &writer});
  logger << lvl::fatal << "message";
  anch::ut::assertThat(writer.messages == std::vector<std::string>({"message"}), "A writer error must not prevent other writers to log");
}

void
testEvaluation() {
  StringWriter writer;
  Logger logger("stream", Level::FATAL, {&writer});
  calls = 0;
  logger << lvl::error << count();
  anch::ut::assertThat(writer.messages.empty(), "ERROR message must be filtered");
  anch::ut::assertThat(calls == 1, "Values are evaluated even when level is disabled: {} call(s)", calls);
}

template<Level L>
void
expectLine(std::vector<std::string>& expected, const std::string& message) {
  using Line = decltype(std::declval<const Logger&>() << anch::log::LevelTag<L>{});
  if constexpr (L < anch::log::MIN_LEVEL) {
    static_assert(std::is_same_v<Line, NoopStream>);
  } else {
    static_assert(std::is_same_v<Line, LogLine>);
    expected.push_back(message);
  }
}

void
testCompileLevel() {
  std::vector<std::string> expected;
  expectLine<Level::TRACE>(expected, "trace");
  expectLine<Level::DEBUG>(expected, "debug");
  expectLine<Level::INFO>(expected, "info");
  expectLine<Level::WARN>(expected, "warn");
  expectLine<Level::ERROR>(expected, "error");
  expectLine<Level::FATAL>(expected, "fatal");
  StringWriter writer;
  Logger logger("stream", Level::TRACE, {&writer});
  logger << lvl::trace << "trace";
  logger << lvl::debug << "debug";
  logger << lvl::info << "info";
  logger << lvl::warn << "warn";
  logger << lvl::error << "error";
  logger << lvl::fatal << "fatal";
  anch::ut::assertThat(writer.messages == expected, "Messages lower than ANCH_LOG_MIN_LEVEL must be removed: {} written, {} expected",
		       writer.messages.size(), expected.size());
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH log stream unit tests")
    .description("Test AnCH logger stream API")
    .add("runtime-level", testRuntimeLevel)
    .add("manipulators", testManipulators)
    .add("format-error", testFormatError)
    .add("writer-error", testWriterError)
    .add("evaluation", testEvaluation)
    .add("compile-level", testCompileLevel)
    ;
}
