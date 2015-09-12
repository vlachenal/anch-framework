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
