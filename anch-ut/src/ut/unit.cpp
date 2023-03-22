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
#include "ut/unit.hpp"

#include <stdexcept>
#include <sstream>

using anch::ut::UnitTests;

// default function which does nothing
void doNothing() {
  // Nothing to do
}


UnitTests::UnitTests(): _name(),
			_description(),
			_initFunc(doNothing),
			_uninitFunc(doNothing),
			_beforeFunc(doNothing),
			_afterFunc(doNothing),
			_tests() {
  // Nothing to do
}

UnitTests::~UnitTests() {
  // Nothing to do
}

UnitTests&
UnitTests::name(const std::string& name) {
  _name = name;
  return *this;
}

UnitTests&
UnitTests::description(const std::string& description) {
  _description = description;
  return *this;
}

UnitTests&
UnitTests::initialize(std::function<void(void)> initFunc) {
  _initFunc = initFunc;
  return *this;
}

UnitTests&
UnitTests::uninitialize(std::function<void(void)> uninitFunc) {
  _uninitFunc = uninitFunc;
  return *this;
}

UnitTests&
UnitTests::beforeTest(std::function<void(void)> beforeFunc) {
  _beforeFunc = beforeFunc;
  return *this;
}

UnitTests&
UnitTests::afterTest(std::function<void(void)> afterFunc) {
  _afterFunc = afterFunc;
  return *this;
}

UnitTests&
UnitTests::add(const std::string& name, std::function<void(void)> testFunc) {
  if(name == "all") {
    throw std::invalid_argument("'all' is a reserved keyword to run all tests");
  }
  if(_tests.contains(name)) {
    std::ostringstream oss;
    oss << "Test " << name << " has already been registered";
    throw std::invalid_argument(oss.str());
  }
  _tests[name] = testFunc;
  return *this;
}

void
UnitTests::initialize() const {
  std::invoke(_initFunc);
}

void
UnitTests::uninitialize() const {
  std::invoke(_uninitFunc);
}

void
UnitTests::before() const {
  std::invoke(_beforeFunc);
}

void
UnitTests::after() const {
  std::invoke(_afterFunc);
}
