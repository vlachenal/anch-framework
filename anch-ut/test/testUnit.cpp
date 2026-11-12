/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2023 Vincent Lachenal

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
#include <cassert> // anch::ut::assertThat has to be usable with <cassert> macro (included before and after ut headers)
#include "ut/unit.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <vector>
#include <list>
#include <set>
#include "ut/assert.hpp"
#include "ut/threadErrors.hpp"
#include <cassert>

void
startup() {
  std::cout << "Initialize tests suite" << std::endl;
  if(std::getenv("ANCH_UT_EXIT_IN_INIT") != NULL) {
    std::exit(EXIT_SUCCESS);
  }
  if(std::getenv("ANCH_UT_FAIL_IN_INIT") != NULL) {
    anch::ut::fail("initialization failure");
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
  if(std::getenv("ANCH_UT_FAIL_IN_UNINIT") != NULL) {
    anch::ut::fail("uninitialization failure");
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
testAssertLocation() {
  anch::ut::assertThat(false, "located assertion {}", 42); // location checked in CMakeLists.txt
}

void
testAssertTrueLocation() {
  anch::ut::assertTrue(false, "located assertTrue"); // location checked in CMakeLists.txt
}

void
testThreadAssert() {
  anch::ut::ThreadErrors errors;
  std::vector<std::thread> threads;
  for(int i = 0 ; i < 4 ; ++i) {
    threads.emplace_back(errors.wrap([i]() {
      anch::ut::assertThat(i != 2, "assertion failed in thread {}", i);
    }));
  }
  for(std::thread& thread : threads) {
    thread.join();
  }
  anch::ut::assertTrue(errors.failed(), "Error should have been captured");
  errors.rethrow();
}

void
testThreadOK() {
  anch::ut::ThreadErrors errors;
  int sum = 0;
  std::thread thread(errors.wrap([&sum](int value) { sum = value; }), 3);
  thread.join();
  errors.rethrow();
  anch::ut::assertFalse(errors.failed(), "No error should have been captured");
  anch::ut::assertThat(sum == 3, "Sum should be 3 instead of {}", sum);
}

void
testEqualsPtr() {
  int value = 1;
  std::ostringstream out;
  anch::ut::assertTrue(anch::ut::equalsPtr<int>("p", NULL, NULL, out), "NULL pointers are equals");
  anch::ut::assertTrue(anch::ut::equalsPtr("p", &value, &value, out), "Same pointers are equals");
  anch::ut::assertTrue(out.str().empty(), "Nothing should be written when pointers are equals");
  anch::ut::assertFalse(anch::ut::equalsPtr<int>("p", &value, NULL, out), "Pointer differs from NULL");
  anch::ut::assertThat(out.str() == "p differs ; expected is not null / current is null\n",
		       "Message should be written in output stream: '{}'", out.str());
}

void
testEqualsCollections() {
  std::ostringstream out;
  anch::ut::assertTrue(anch::ut::equals("v", std::vector<int>{1, 2, 3}, std::vector<int>{1, 2, 3}, out), "Vectors are equals");
  anch::ut::assertTrue(anch::ut::equals("l", std::list<int>{1, 2, 3}, std::list<int>{1, 2, 3}, out), "Lists are equals");
  anch::ut::assertTrue(anch::ut::equals("s", std::set<int>{1, 2, 3}, std::set<int>{1, 2, 3}, out), "Sets are equals");
  anch::ut::assertTrue(out.str().empty(), "Nothing should be written when collections are equals");
  anch::ut::assertFalse(anch::ut::equals("v", std::vector<int>{1, 2, 3}, std::vector<int>{1, 4, 3}, out), "Vectors differ");
  anch::ut::assertThat(out.str() == "v[1] differs ; expected='2' / current='4'\n", "Unexpected message: '{}'", out.str());
  out.str("");
  anch::ut::assertFalse(anch::ut::equals("l", std::list<int>{1, 2}, std::list<int>{1, 2, 3}, out), "Lists differ");
  anch::ut::assertThat(out.str() == "l's size differs ; expected=2 / current=3\n", "Unexpected message: '{}'", out.str());
  out.str("");
  // long double elements: equals when they are equals as double
  anch::ut::assertTrue(anch::ut::equals("d", std::vector<long double>{1.0L, 2.0L}, std::vector<long double>{1.0L, 2.0L}, out), "Long double vectors are equals");
}

void
testOrder(const std::string& name) {
  std::cout << "Run " << name << std::endl;
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
    .add("assert-location", testAssertLocation)
    .add("assert-true-location", testAssertTrueLocation)
    .add("thread-assert", testThreadAssert)
    .add("thread-ok", testThreadOK)
    .add("equals-ptr", testEqualsPtr)
    .add("equals-collections", testEqualsCollections)
    .add("z-registered-first", []() { testOrder("z-registered-first"); })
    .add("a-registered-second", []() { testOrder("a-registered-second"); })
    ;
  if(std::getenv("ANCH_UT_DUPLICATE") != NULL) {
    tests.add("nothing", testNothing);
  }
}
