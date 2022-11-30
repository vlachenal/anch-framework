#include <iostream>

#include "stringUtils.hpp"

int
main(void) {
  std::string str("plip plop");
  std::cout << "Original: " << str << std::endl;
  anch::toUpper(str);
  std::cout << "toUpper: " << str << std::endl;
  if(str != "PLIP PLOP") {
    std::cerr << "toUpper does not work: " << str << std::endl;
    return 1;
  }
  anch::toLower(str);
  std::cout << "toLower: " << str << std::endl;
  if(str != "plip plop") {
    std::cerr << "toLower does not work: " << str << std::endl;
    return 1;
  }
  return 0;
}
