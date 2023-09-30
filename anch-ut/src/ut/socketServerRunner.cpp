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
#include "ut/socketServerRunner.hpp"

using anch::ut::SocketServerRunner;


SocketServerRunner::SocketServerRunner([[maybe_unused]] const std::string& uri, const anch::ut::UnitTests& tests): anch::ut::DefaultRunner(tests) {
  // \todo init socket
}

SocketServerRunner::~SocketServerRunner() {
  // \todo plop
}

void
SocketServerRunner::handle([[maybe_unused]] const anch::events::Event<anch::network::SocketEvent>& evt) noexcept {
  // \todo handle message
}
