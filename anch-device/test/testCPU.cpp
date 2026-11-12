/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2015 Vincent Lachenal

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
#include "device/cpu.hpp"

#include <iostream>

int
main(void) {
  std::cout << "AES support: " << (anch::device::CPU::getInstance().isAES() ? "true" : "false") << std::endl;
  std::cout << "MMX support: " << (anch::device::CPU::getInstance().isMMX() ? "true" : "false") << std::endl;
  std::cout << "SSE support: " << (anch::device::CPU::getInstance().isSSE() ? "true" : "false") << std::endl;
  std::cout << "SSE2 support: " << (anch::device::CPU::getInstance().isSSE2() ? "true" : "false") << std::endl;
  std::cout << "SSE3 support: " << (anch::device::CPU::getInstance().isSSE3() ? "true" : "false") << std::endl;
  std::cout << "SSSE3 support: " << (anch::device::CPU::getInstance().isSSSE3() ? "true" : "false") << std::endl;
  std::cout << "SSE4 support: " << (anch::device::CPU::getInstance().isSSE4() ? "true" : "false") << std::endl;
  std::cout << "SSE4.1 support: " << (anch::device::CPU::getInstance().isSSE4_1() ? "true" : "false") << std::endl;
  std::cout << "SSE4.2 support: " << (anch::device::CPU::getInstance().isSSE4_2() ? "true" : "false") << std::endl;
  return 0;
}
