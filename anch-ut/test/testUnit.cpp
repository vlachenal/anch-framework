#include "ut/unit.hpp"

#include <iostream>
#include "ut/assert.hpp"

void
startup() {
  std::cout << "Initialize tests suite" << std::endl;
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
}

void
testNothing() {
  std::cout << "Nothing" << std::endl;
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
  anch::ut::assert(false, "format message: int={}, float={}, str={}", 2, 3.14, "toto");
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
    ;
}
