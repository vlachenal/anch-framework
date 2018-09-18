#include "uuid.hpp"

#include <iostream>

using anch::UUID;

int
main(void) {
  UUID random = UUID::random();
  std::cout << "Random UUID:      " << random << std::endl;
  std::string randomStr = static_cast<std::string>(random);
  std::cout << "UUID random conversion:   " << randomStr << std::endl;
  std::cout << "UUID random string conversion: " << UUID(randomStr) << std::endl;
  return 0;
}
