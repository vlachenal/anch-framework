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
#include "device/cpu.hpp"
#include "device/network.hpp"

#include <iostream>

int
main() {
  for(const auto& [name, interface] : anch::device::Network::getInterfaces()) {
    std::cout << name << " " << interface.getIpAddress() << " " << interface.getMacAddress()
              << (interface.isUp() ? " up" : " down") << std::endl;
  }
  const anch::device::CPU& cpu = anch::device::CPU::getInstance();
  std::cout << "AES-NI: " << cpu.isAES() << std::endl;
  return 0;
}
//! [example]
