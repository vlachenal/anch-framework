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
#include "app/app.hpp"

using anch::app::Application;


Application::Application(): _handler(new Arguments()) {
  // \todo register args
}

Application::~Application() noexcept {
  if(_handler != NULL) {
    delete _handler;
  }
  after();
}

// To override methods +
void
Application::before() {
  // Nothing to do
}

void
Application::after() noexcept {
  // Nothing to do
}
// To override methods -

// Arguments registering +
Application&
Application::arg([[ maybe_unused ]] const anch::cli::Arg& arg) {
  // \todo do some checks and add to arguments
  return *this;
}

Application&
Application::args([[ maybe_unused ]] const std::vector<anch::cli::Arg>& args) {
  for(auto item : args) {
    arg(item);
  }
  return *this;
}
// Arguments registering -

// Execution +
void
Application::run([[ maybe_unused ]] int argc, [[ maybe_unused ]] char* argv[]) {
  // \todo parse arguments
  before();
  // Free memory +
  if(_handler != NULL) {
    delete _handler;
    _handler = NULL; // to avoid double free in destructor
  }
  // Free memory -
  // \todo something ???
  execute();
  // \todo something ???
}
// Execution -
