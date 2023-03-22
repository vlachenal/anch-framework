/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ut/defaultRunner.hpp"

#include <iostream>

#include "ut/error.hpp"

using anch::ut::DefaultRunner;


DefaultRunner::DefaultRunner(const UnitTests& tests): _tests(tests) {
  // Nothing to do
}

bool
DefaultRunner::startup() {
  bool ok = false;
  try {
    _tests.initialize();
    ok = true;
  } catch(const anch::ut::AssertException& e) {
    std::cout << "Unit tests initialization failed on assertion: " << e.what() << std::endl;
  } catch(const std::exception& e) {
    std::cout << "Unit tests initialization failed: " << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unit tests initialization failed" << std::endl;
  }
  return ok;
}

bool
DefaultRunner::shutdown() {
  bool ok = false;
  try {
    _tests.uninitialize();
    ok = true;
  } catch(const anch::ut::AssertException& e) {
    std::cout << "Unit tests uninitialization failed on assertion: " << e.what() << std::endl;
  } catch(const std::exception& e) {
    std::cout << "Unit tests uninitialization failed: " << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unit tests uninitialization failed" << std::endl;
  }
  return ok;
}

uint16_t
DefaultRunner::test(const std::string& test) {
  auto iter = _tests.getTests().find(test);
  if(iter == _tests.getTests().end()) {
    std::cout << test << " has not been registered" << std::endl;
    return 1;
  }

  // Before test function +
  try {
    _tests.before();
  } catch(const anch::ut::AssertException& e) {
    std::cout << "Unit test " << iter->first << " initialization failed on assertion: " << e.what() << std::endl;
    return 2;
  } catch(const std::exception& e) {
    std::cout << "Unit test " << iter->first << " initialization failed: " << e.what() << std::endl;
    return 2;
  } catch(...) {
    std::cout << "Unit test " << iter->first << " initialization failed" << std::endl;
    return 2;
  }
  // Before test function -

  bool passed = false;
  // Call test function +
  try {
    std::invoke(iter->second);
    passed = true;
  } catch(const anch::ut::AssertException& e) {
    std::cout << "Unit test " << iter->first << " execution failed on assertion: " << e.what() << std::endl;
  } catch(const std::exception& e) {
    std::cout << "Unit test " << iter->first << " execution failed: " << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unit test " << iter->first << " execution failed" << std::endl;
  }
  // Call test function -

  // After test function +
  bool after = false;
  try {
    _tests.after();
    after = true;
  } catch(const anch::ut::AssertException& e) {
    std::cout << "Unit test " << iter->first << " uninitialization failed on assertion: " << e.what() << std::endl;
  } catch(const std::exception& e) {
    std::cout << "Unit test " << iter->first << " uninitialization failed: " << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unit test " << iter->first << " uninitialization failed" << std::endl;
  }
  // After test function -

  if(!passed) {
    return 3;
  }
  if(!after) {
    return 4;
  }
  return 0;
}
