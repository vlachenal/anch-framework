/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2024 Vincent Lachenal

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
#include "log/loggerFactory.hpp"

#include <iostream>
#include <latch>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "ut/unit.hpp"
#include "ut/assert.hpp"


using anch::log::LoggerFactory;
using anch::log::Logger;


/*! Number of messages written by each thread in rotate test */
constexpr int ROTATE_MESSAGES = 5000;

/*! Number of messages written by each thread in rotate test once both threads have written \ref ROTATE_MESSAGES */
constexpr int ROTATE_FINAL_MESSAGES = 10;

void
testMultiThread(const Logger& log, int nbLog) {
  for(int i = 0 ; i < nbLog ; ++i) {
    log.info("Message n° ", i, " from the second thread");
  }
}

void
testRotate() {
  std::cout << "Enter in testRotate" << std::endl;

  // Previous runs archives
  std::filesystem::remove("test.log");
  for(int i = 0 ; i < 10 ; ++i) {
    std::filesystem::remove("test.log." + std::to_string(i));
  }
  anch::conf::Configuration::loader().name("anch-logger").load();
  static auto LOG = LoggerFactory::getLogger("anch::logger::Logger");
  std::cout << "Logger has been retrieved" << std::endl;
  LOG.info("anch-logger");

  // 2 x 5000 messages (about 900 KB) with max-size=64K and max-rotate-files=3: many rotations, 3 archives kept.
  // Kept files hold only the last messages: when a thread finishes first, they can all come from the other one.
  // Both threads wait for each other, then write ROTATE_FINAL_MESSAGES messages (less than 3 KB, at most one more
  // rotation), so kept files always have messages of both threads.
  std::latch done(2);
  std::thread th = std::thread([&done]() {
    testMultiThread(LOG, ROTATE_MESSAGES);
    done.arrive_and_wait();
    testMultiThread(LOG, ROTATE_FINAL_MESSAGES);
  });
  for(int i = 0 ; i < ROTATE_MESSAGES ; ++i) {
    LOG.info("Message n° ", i, " from main thread");
  }
  done.arrive_and_wait();
  for(int i = 0 ; i < ROTATE_FINAL_MESSAGES ; ++i) {
    LOG.info("Message n° ", i, " from main thread");
  }
  th.join();

  std::size_t main = 0;
  std::size_t second = 0;
  for(const char* file : {"test.log", "test.log.0", "test.log.1", "test.log.2"}) {
    anch::ut::assertThat(std::filesystem::exists(file), "{} has not been created", file);
    std::uintmax_t size = std::filesystem::file_size(file);
    anch::ut::assertThat(size <= 64 * 1024 + 200, "{} is too large: {} bytes", file, size);
    std::ifstream input(file);
    std::string line;
    while(std::getline(input, line)) {
      if(line.find("from main thread") != std::string::npos) {
	++main;
      } else if(line.find("from the second thread") != std::string::npos) {
	++second;
      }
    }
  }
  anch::ut::assertThat(!std::filesystem::exists("test.log.3"), "Only 3 archives have to be kept");
  anch::ut::assertThat(main > 0 && second > 0, "Messages of both threads have to be written ({} and {})", main, second);

  std::cout << "Exit testRotate" << std::endl;
}

void
testCustomFile() {
  std::cout << "Enter in testCustomFile" << std::endl;

  anch::conf::Configuration::loader().name("customFile").load();
  static auto LOG = LoggerFactory::getLogger("anch::logger::Logger");
  LOG.info("customFile");

  for(int i = 0 ; i < 200 ; ++i) {
    LOG.info("Message n° ", i, " from main thread");
  }

  std::cout << "Exit testCustomFile" << std::endl;
}

void
testFallbackOnDefault() {
  std::cout << "Enter in testFallbackOnDefault" << std::endl;

  //LoggerFactory::CONF_FILE = "invalid.conf";
  try {
    anch::conf::Configuration::loader().name("invalid").load();
  } catch(...) {
    // OK
  }
  auto LOG = LoggerFactory::getLogger("anch::logger::Logger");
  LOG.info("Message with default configuration");

  std::cout << "Exit testFallbackOnDefault" << std::endl;
}

void
testLowPriority() {
  std::cout << "Enter in testLowPriority" << std::endl;

  anch::conf::Configuration::loader().name("low-priority-logger").load();
  static auto LOG = LoggerFactory::getLogger("anch::logger::Logger");
  LOG.info("low-priority-logger.conf");

  std::thread th = std::thread(testMultiThread, LOG, 2000);
  for(int i = 0 ; i < 2000 ; ++i) {
    LOG.info("Message n° ", i, " from main thread");
  }

  th.join();

  std::cout << "Exit testLowPriority" << std::endl;
}

void
testBadWriters() {
  std::cout << "Enter in testBadWriters" << std::endl;

  anch::conf::Configuration::loader().name("anch-logger-bad-writer").load();
  static auto LOG = LoggerFactory::getLogger("anch::logger::Logger");
  LOG.info("anch-logger-bad-writer.conf");

  std::cout << "Exit testBadWriters" << std::endl;
}

void
testBadLoggers() {
  std::cout << "Enter in testBadLoggers" << std::endl;

  anch::conf::Configuration::loader().name("anch-logger-bad-logger").load();
  static auto LOG = LoggerFactory::getLogger("anch::logger::Logger");
  LOG.info("anch-logger-bad-logger.conf");

  std::cout << "Exit testBadLoggers" << std::endl;
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

// L11: spaces in writers list ; L12: loggers with writers used every writer, loggers without writers none
void
testWritersPerLogger() {
  std::filesystem::remove("writers-a.log");
  std::filesystem::remove("writers-b.log");
  anch::conf::Configuration::loader().name("anch-logger-writers").load();
  LoggerFactory::getLogger("only::a").info("1");
  LoggerFactory::getLogger("no::writers").info("2");
  LoggerFactory::getLogger("other").info("3");
  std::vector<std::string> a = readLines("writers-a.log");
  std::vector<std::string> b = readLines("writers-b.log");
  anch::ut::assertTrue(a == std::vector<std::string>{"only::a 1", "no::writers 2", "other 3"}, "Unexpected messages in writer a");
  anch::ut::assertTrue(b == std::vector<std::string>{"no::writers 2", "other 3"}, "Unexpected messages in writer b");
}

// L10: getLogger was not thread safe
void
testConcurrentGetLogger() {
  anch::conf::Configuration::loader().name("anch-logger-writers").load();
  std::vector<std::thread> threads;
  std::vector<const Logger*> loggers(8 * 50, nullptr);
  for(std::size_t t = 0 ; t < 8 ; ++t) {
    threads.emplace_back([&loggers, t]() {
      for(std::size_t i = 0 ; i < 50 ; ++i) {
	loggers[t * 50 + i] = &LoggerFactory::getLogger("concurrent::logger" + std::to_string(i));
      }
    });
  }
  for(std::thread& thread : threads) {
    thread.join();
  }
  for(std::size_t t = 1 ; t < 8 ; ++t) {
    for(std::size_t i = 0 ; i < 50 ; ++i) {
      anch::ut::assertTrue(loggers[t * 50 + i] == loggers[i], "Different instances for the same logger");
    }
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH logger unit tests")
    .description("Test AnCH logger utiliy class")
    .add("rotate", testRotate)
    .add("custom-file", testCustomFile)
    .add("default", testFallbackOnDefault)
    .add("low-priority", testLowPriority)
    .add("bad-writers", testBadWriters)
    .add("bad-loggers", testBadLoggers)
    .add("writers-per-logger", testWritersPerLogger)
    .add("concurrent-get-logger", testConcurrentGetLogger)
    ;
}
