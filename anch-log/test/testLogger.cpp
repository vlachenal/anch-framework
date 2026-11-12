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


void
testMultiThread(const Logger& log, int nbLog) {
  for(int i = 0 ; i < nbLog ; ++i) {
    log.info("Message n° ", i, " from the second thread");
  }
}

void
testRotate() {
  std::cout << "Enter in testRotate" << std::endl;

  anch::conf::Configuration::loader().name("anch-logger").load();
  static auto LOG = LoggerFactory::getLogger("anch::logger::Logger");
  std::cout << "Logger has been retrieved" << std::endl;
  LOG.info("anch-logger");

  std::thread th = std::thread(testMultiThread, LOG, 200000);
  for(int i = 0 ; i < 200000 ; ++i) {
    LOG.info("Message n° ", i, " from main thread");
  }
  th.join();

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
