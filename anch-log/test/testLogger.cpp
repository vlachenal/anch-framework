#include "log/loggerFactory.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include "ut/unit.hpp"


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

/*void
beforeAll() {
  std::cerr << "anch-logger-test has to be launch one by one => configuration file change is not supported (and will not be supported) ; if not, segfault will occur" << std::endl;
  }*/

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH logger unit tests")
    .description("Test AnCH logger utiliy class")
    //.initialize(beforeAll)
    .add("rotate", testRotate)
    .add("custom-file", testCustomFile)
    .add("default", testFallbackOnDefault)
    .add("low-priority", testLowPriority)
    ;
}
