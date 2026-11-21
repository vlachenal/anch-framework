/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <thread>

#include "ut/assert.hpp"
#include "ut/threadErrors.hpp"
#include "ut/unit.hpp"

bool
check() {
  return true;
}

void
testThread() {
  //! [thread]
  anch::ut::ThreadErrors errors;
  std::thread thread(errors.wrap([]() {
    anch::ut::assertThat(check(), "check failed in thread");
  }));
  thread.join();
  errors.rethrow(); // the failed assertion is reported by the runner
  //! [thread]
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("ThreadErrors example")
    .add("thread", testThread);
}
