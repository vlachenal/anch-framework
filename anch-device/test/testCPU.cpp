#include "device/cpu.hpp"

#include <iostream>

int
main(void) {
  std::cout << "AES support: " << (anch::device::CPU::getInstance().isAES() ? "true" : "false") << std::endl;
  return 0;
}
