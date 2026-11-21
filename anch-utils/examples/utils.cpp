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
//! [example]
#include "convert.hpp"
#include "threadPool.hpp"

#include <atomic>
#include <chrono>
#include <iostream>

int
main() {
  // Strict number conversion (std::bad_cast on error ; anch::tryConvert returns a std::expected)
  int value = anch::convert<int>("42");
  double ratio = anch::convert<double>("0.5");

  // Thread pool: tasks are executed by persistent workers
  std::atomic<int> sum = 0;
  anch::ThreadPool pool(4);
  pool.start();
  for(int i = 1 ; i <= 10 ; ++i) {
    pool.add([&sum](int n) { sum += n; }, i);
  }
  pool.awaitTermination(std::chrono::seconds(0));  // waits for queued tasks (0: no timeout), then stops
  std::cout << value * ratio << " " << sum << std::endl;
  return 0;
}
//! [example]
