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
#include "singleton.hpp"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <latch>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

#include "ut/unit.hpp"
#include "ut/assert.hpp"


class Test: public anch::Singleton<Test> {
  friend anch::Singleton<Test>;
public:
  static std::atomic<int> nbBuilt;
private:
  Test() {
    ++nbBuilt;
  }
  ~Test() = default;
};

std::atomic<int> Test::nbBuilt = 0;

class SlowTest: public anch::Singleton<SlowTest> {
  friend anch::Singleton<SlowTest>;
public:
  static std::atomic<int> nbBuilt;
private:
  SlowTest() {
    ++nbBuilt;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  ~SlowTest() = default;
};

std::atomic<int> SlowTest::nbBuilt = 0;

// Constructor fails on first call
class FlakyTest: public anch::Singleton<FlakyTest> {
  friend anch::Singleton<FlakyTest>;
public:
  static int nbAttempts;
private:
  FlakyTest() {
    if(++nbAttempts == 1) {
      throw std::runtime_error("first attempt fails");
    }
  }
  ~FlakyTest() = default;
};

int FlakyTest::nbAttempts = 0;

// Copy and move have to be prohibited by anch::Singleton, even with public constructor and destructor
class PublicTest: public anch::Singleton<PublicTest> {
public:
  PublicTest() = default;
  ~PublicTest() = default;
};

static_assert(!std::is_copy_constructible_v<PublicTest>, "Singleton must not be copyable");
static_assert(!std::is_move_constructible_v<PublicTest>, "Singleton must not be movable");
static_assert(!std::is_copy_assignable_v<PublicTest>, "Singleton must not be copy assignable");
static_assert(!std::is_move_assignable_v<PublicTest>, "Singleton must not be move assignable");

// Destruction order: Dependent uses Dependency in its destructor
class Dependency: public anch::Singleton<Dependency> {
  friend anch::Singleton<Dependency>;
public:
  static bool alive;
private:
  Dependency() {
    alive = true;
  }
  ~Dependency() {
    alive = false;
  }
};

bool Dependency::alive = false;

class Dependent: public anch::Singleton<Dependent> {
  friend anch::Singleton<Dependent>;
private:
  Dependent() {
    Dependency::getInstance(); // Dependency is built first, so it will be destroyed after
  }
  ~Dependent() {
    if(!Dependency::alive) {
      std::cerr << "Dependency has been destroyed before Dependent" << std::endl;
      std::_Exit(EXIT_FAILURE); // test result is the process exit code
    }
  }
};

void
testSeqNoWait() {
  Test& test1 = Test::getInstance();
  Test& test2 = Test::getInstance();
  anch::ut::assertTrue(&test1 == &test2, "getInstance returns different instances");
  anch::ut::assertThat(Test::nbBuilt == 1, "Instance has been built {} times", Test::nbBuilt.load());
}

void
testParallelWait() {
  constexpr std::ptrdiff_t NB_THREADS = 20;
  std::latch start(NB_THREADS);
  std::vector<SlowTest*> instances(NB_THREADS, nullptr);
  std::vector<std::thread> threads;
  for(std::ptrdiff_t i = 0 ; i < NB_THREADS ; ++i) {
    threads.emplace_back([&start, &instances, i]() {
      start.arrive_and_wait(); // every thread calls getInstance at the same time
      instances[static_cast<std::size_t>(i)] = &SlowTest::getInstance();
    });
  }
  for(std::thread& thread : threads) {
    thread.join();
  }
  for(SlowTest* instance : instances) {
    anch::ut::assertTrue(instance == instances[0], "getInstance returns different instances");
  }
  anch::ut::assertThat(SlowTest::nbBuilt == 1, "Instance has been built {} times", SlowTest::nbBuilt.load());
}

void
testRetryOnError() {
  try {
    FlakyTest::getInstance();
    anch::ut::fail("Constructor exception has not been raised");
  } catch(const std::runtime_error&) {
    // expected
  }
  FlakyTest& test = FlakyTest::getInstance();
  anch::ut::assertTrue(&test == &FlakyTest::getInstance(), "getInstance returns different instances");
  anch::ut::assertThat(FlakyTest::nbAttempts == 2, "{} construction attempts instead of 2", FlakyTest::nbAttempts);
}

void
testDestructionOrder() {
  Dependent::getInstance();
  // Checked by Dependent destructor at program exit
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH singleton unit tests")
    .description("Test AnCH singleton utiliy class")
    .add("seq-no-wait", testSeqNoWait)
    .add("parallel-wait", testParallelWait)
    .add("retry-on-error", testRetryOnError)
    .add("destruction-order", testDestructionOrder)
    ;
}
