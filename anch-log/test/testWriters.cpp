#include "log/fileWriter.hpp"
#include "log/lowPriorityWriter.hpp"
#include "log/mdc.hpp"
#include "log/levels.hpp"
#include "log/fmt/messageFormatter.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "ut/unit.hpp"
#include "ut/assert.hpp"

using anch::conf::Section;
using anch::log::FileWriter;
using anch::log::LowPriorityWriter;
using anch::log::MDC;
using anch::log::fmt::MessageFormatter;

namespace fs = std::filesystem;

// Helpers +
Section
makeSection(const std::map<std::string,std::string>& values) {
  Section section;
  for(const auto& [key, value] : values) {
    section.putValue(key, value);
  }
  return section;
}

// Remove log file and its archives
void
clean(const std::string& path) {
  fs::remove(path);
  for(int i = 0 ; i < 10 ; ++i) {
    fs::remove(path + "." + std::to_string(i));
  }
}

std::vector<std::string>
readLines(const std::string& path) {
  std::vector<std::string> lines;
  std::ifstream input(path);
  std::string line;
  while(std::getline(input, line)) {
    lines.push_back(line);
  }
  return lines;
}

// Lines from first to last (included) formatted on 9 digits (10 bytes with end of line)
std::vector<std::string>
numbers(int first, int last) {
  std::vector<std::string> lines;
  for(int i = first ; i <= last ; ++i) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%09d", i);
    lines.push_back(buf);
  }
  return lines;
}

// Logger and level are set by the logger in the MDC
void
setLogContext() {
  MDC.put("logger", "test");
  MDC.put("level", "INFO");
}

void
writeNumbers(anch::log::Writer& writer, int first, int last) {
  setLogContext();
  for(const std::string& line : numbers(first, last)) {
    writer.write(line);
  }
}
// Helpers -

// L8: the default pattern did not write the message
void
testDefaultPattern() {
  clean("default-pattern.log");
  {
    FileWriter writer(makeSection({{"path", "./default-pattern.log"}}));
    setLogContext();
    writer.write("hello");
  }
  std::vector<std::string> lines = readLines("default-pattern.log");
  anch::ut::assertTrue(lines == std::vector<std::string>{"hello"}, "Default pattern does not write the message");
}

void
testFormatter() {
  MessageFormatter formatter("$c|$p|$m|price $5|$x");
  std::string res = formatter.formatMessage("cat", anch::log::Level::INFO, "msg");
  anch::ut::assertThat(res == "cat|INFO|msg|price $5|$x", "Formatted message is '{}'", res);
  MessageFormatter percent("%m");
  res = percent.formatMessage("cat", anch::log::Level::INFO, "msg");
  anch::ut::assertThat(res == "%m", "'%m' is not a constant: '{}'", res);
}

void
testMDCFormatter() {
  MDC.put("user", "alice");
  MessageFormatter formatter("$C{user}/$C{none}");
  std::string res = formatter.formatMessage("cat", anch::log::Level::INFO, "msg");
  anch::ut::assertThat(res == "alice/undefined", "Formatted message is '{}'", res);
  MDC.remove("user");
}

// L2: max-size parsing
void
testInvalidMaxSize() {
  for(const std::string size : {"2X", "", "12KB", "K", "-1", "99999999999999T"}) {
    try {
      FileWriter writer(makeSection({{"path", "./invalid.log"}, {"max-size", size}}));
      anch::ut::fail("Max size '" + size + "' has been accepted");
    } catch(const std::invalid_argument&) {
      // expected
    }
  }
  clean("invalid.log");
}

// L2: max-size without unit was ignored ("100") ; L3: archives names ; L4: number of archives
void
testRotation() {
  clean("rotation.log");
  {
    FileWriter writer(makeSection({{"path", "./rotation.log"}, {"max-size", "100"}, {"max-rotate-files", "3"}}));
    writeNumbers(writer, 0, 49); // 10 lines per file: 5 rotations
  }
  anch::ut::assertTrue(readLines("rotation.log.0") == numbers(40, 49), "Unexpected archive 0 content");
  anch::ut::assertTrue(readLines("rotation.log.1") == numbers(30, 39), "Unexpected archive 1 content");
  anch::ut::assertTrue(readLines("rotation.log.2") == numbers(20, 29), "Unexpected archive 2 content");
  anch::ut::assertFalse(fs::exists("rotation.log.3"), "More than 3 archives have been kept");
  anch::ut::assertTrue(readLines("rotation.log").empty(), "Current file is not empty");
  for(const auto& entry : fs::directory_iterator(".")) {
    std::string name = entry.path().filename().string();
    if(name.starts_with("rotation.log")) {
      anch::ut::assertThat(name == "rotation.log" || name.size() == std::string("rotation.log.0").size(), "Unexpected archive name {}", name);
    }
  }
}

// L2: '1K' was read as 1 byte and '1G' as 1 byte
void
testMaxSizeUnits() {
  clean("units-k.log");
  clean("units-g.log");
  {
    FileWriter writerK(makeSection({{"path", "./units-k.log"}, {"max-size", "1K"}}));
    FileWriter writerG(makeSection({{"path", "./units-g.log"}, {"max-size", "1G"}}));
    writeNumbers(writerK, 0, 102); // 1030 bytes: one rotation after 1024 bytes
    writeNumbers(writerG, 0, 102);
  }
  anch::ut::assertTrue(readLines("units-k.log.0") == numbers(0, 102), "1K: unexpected archive content");
  anch::ut::assertFalse(fs::exists("units-g.log.0"), "1G: file has been rotated");
  anch::ut::assertThat(readLines("units-g.log").size() == 103, "1G: {} lines instead of 103", readLines("units-g.log").size());
}

void
testNoArchive() {
  clean("no-archive.log");
  {
    FileWriter writer(makeSection({{"path", "./no-archive.log"}, {"max-size", "100"}, {"max-rotate-files", "0"}}));
    writeNumbers(writer, 0, 24);
  }
  anch::ut::assertFalse(fs::exists("no-archive.log.0"), "Archive has been created");
  anch::ut::assertTrue(readLines("no-archive.log") == numbers(20, 24), "Unexpected file content");
}

void
testDefaultArchives() {
  clean("default-archive.log");
  {
    FileWriter writer(makeSection({{"path", "./default-archive.log"}, {"max-size", "100"}}));
    writeNumbers(writer, 0, 34);
  }
  anch::ut::assertTrue(readLines("default-archive.log.0") == numbers(20, 29), "Unexpected archive content");
  anch::ut::assertFalse(fs::exists("default-archive.log.1"), "More than 1 archive has been kept");
  anch::ut::assertTrue(readLines("default-archive.log") == numbers(30, 34), "Unexpected file content");
}

// L1: low priority writer never wrote and crashed on destruction
void
testLowPriority() {
  clean("low-priority.log");
  {
    LowPriorityWriter writer(makeSection({{"path", "./low-priority.log"}}));
    std::thread other([&writer]() { writeNumbers(writer, 1000, 1999); });
    writeNumbers(writer, 0, 999);
    other.join();
  } // queued messages are written on destruction
  std::vector<std::string> lines = readLines("low-priority.log");
  anch::ut::assertThat(lines.size() == 2000, "{} lines instead of 2000", lines.size());
  for(const std::string& line : lines) {
    anch::ut::assertThat(line.size() == 9, "Malformed line '{}'", line);
  }
}

// L9: add, remove, reset and copy only applied to constants
void
testMDC() {
  anch::log::MDC_CONST["app"] = "anch";
  std::thread thread([]() { // MDC is thread local: new context with constants
    MDC.add("user", "alice");
    MDC.add("user", "bob"); // add does not overwrite
    MDC.add("app", "other"); // constants can not be added
    anch::ut::assertThat(MDC.get().at("user") == "alice", "user is {}", MDC.get().at("user"));
    anch::ut::assertThat(MDC.get().at("app") == "anch", "app is {}", MDC.get().at("app"));
    MDC.remove("app"); // constants can not be removed
    anch::ut::assertTrue(MDC.get().contains("app"), "Constant has been removed");
    MDC.remove("user");
    anch::ut::assertFalse(MDC.get().contains("user"), "user has not been removed");
    MDC.put("a", "1");
    MDC.put("b", "2");
    MDC.put("c", "3");
    MDC.reset();
    anch::ut::assertThat(MDC.get().size() == 2, "{} entries after reset instead of 2", MDC.get().size());
    anch::ut::assertTrue(MDC.get().contains(anch::log::MDC_THREAD_ID) && MDC.get().contains("app"), "reset has removed thread id or constants");
    anch::log::MapDiagnosticContext other;
    other.put("x", "1");
    other.put(anch::log::MDC_THREAD_ID, "other thread");
    MDC.copy(other);
    anch::ut::assertTrue(MDC.get().at("x") == "1", "x has not been copied");
    anch::ut::assertTrue(MDC.get().at(anch::log::MDC_THREAD_ID) != "other thread", "thread id has been copied");
  });
  thread.join();
  anch::log::MDC_CONST.erase("app");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH log writers unit tests")
    .description("Test AnCH log writers, formatters and MDC")
    .add("default-pattern", testDefaultPattern)
    .add("formatter", testFormatter)
    .add("mdc-formatter", testMDCFormatter)
    .add("invalid-max-size", testInvalidMaxSize)
    .add("rotation", testRotation)
    .add("max-size-units", testMaxSizeUnits)
    .add("no-archive", testNoArchive)
    .add("default-archives", testDefaultArchives)
    .add("low-priority-writer", testLowPriority)
    .add("mdc", testMDC)
    ;
}
