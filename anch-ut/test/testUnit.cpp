#include <cassert> // anch::ut::assertThat has to be usable with <cassert> macro (included before and after ut headers)
#include "ut/unit.hpp"

#include <iostream>
#include <cstdlib>
#include "ut/assert.hpp"
#include <cassert>

void
startup() {
  std::cout << "Initialize tests suite" << std::endl;
  if(std::getenv("ANCH_UT_EXIT_IN_INIT") != NULL) {
    std::exit(EXIT_SUCCESS);
  }
}

void
before() {
  std::cout << "Before test" << std::endl;
}

void
after() {
  std::cout << "After test" << std::endl;
}

void
shutdown() {
  std::cout << "Uninitialize tests suite" << std::endl;
  if(std::getenv("ANCH_UT_EXIT_IN_UNINIT") != NULL) {
    std::exit(EXIT_SUCCESS);
  }
}

void
testNothing() {
  std::cout << "Nothing" << std::endl;
  assert(true); // <cassert> macro
  anch::ut::assertThat(true, "assertThat with <cassert> macro: {}", 1);
}

void
testException() {
  std::cout << "Exception" << std::endl;
  throw std::exception();
}

void
testAssert() {
  std::cout << "Assert" << std::endl;
  anch::ut::assertFalse(true, "ASSERT !!!");
}

void
testAssertFormat() {
  std::cout << "Assert" << std::endl;
  anch::ut::assertThat(false, "format message: int={}, float={}, str={}", 2, 3.14, "toto");
}

void
testExit() {
  std::cout << "Exit" << std::endl;
  std::exit(EXIT_SUCCESS); // has to be reported as a failure
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH unit tests")
    .description("Test AnCH unit tests library")
    .initialize(startup)
    .beforeTest(before)
    .afterTest(after)
    .uninitialize(shutdown)
    .add("nothing", testNothing)
    .add("exception", testException)
    .add("assert", testAssert)
    .add("assert-format", testAssertFormat)
    .add("exit-in-test", testExit)
    ;
}
