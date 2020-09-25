#include <iostream>

#include "convert.hpp"


int
main(void) {

  {
    std::cout << "Parse '32' as uint16_t" << std::endl;
    uint16_t val;
    try {
      val = anch::convert<uint16_t>("32");
    } catch(const std::bad_cast& e) {
      std::cerr << "Unable to parse '32' as uint16_t" << std::endl;
      return 1;
    }
    std::cout << "Parsed value is " << val << std::endl;
  }

  {
    std::cout << "Parse '-32' as uint16_t" << std::endl;
    uint16_t val;
    try {
      val = anch::convert<uint16_t>("-32");
    } catch(const std::bad_cast& e) {
      std::cerr << "Unable to parse '-32' as uint16_t" << std::endl;
      return 1;
    }
    std::cout << "Parsed value is " << val << std::endl;
  }

  /*{
    std::cout << "Parse '0xFF' as uint16_t" << std::endl;
    uint16_t val;
    try {
      val = anch::convert<uint16_t>("0xFF");
    } catch(const std::bad_cast& e) {
      std::cerr << "Unable to parse '0xFF' as uint16_t" << std::endl;
      return 1;
    }
    std::cout << "Parsed value is " << val << std::endl;
    }*/

  {
    std::cout << "Parse '32.2' as uint16_t" << std::endl;
    uint16_t val;
    try {
      val = anch::convert<uint16_t>("32.2");
      std::cerr << "Parsed value is " << val << std::endl;
      return 1;
    } catch(const std::bad_cast& e) {
      std::cout << "Unable to parse '32.2' as uint16_t" << std::endl;
    }
  }

  {
    std::cout << "Parse '32e-1' as uint16_t" << std::endl;
    uint16_t val;
    try {
      val = anch::convert<uint16_t>("32e-1");
      std::cerr << "Parsed value is " << val << std::endl;
      return 1;
    } catch(const std::bad_cast& e) {
      std::cout << "Unable to parse '32e-1' as uint16_t" << std::endl;
    }
  }

  {
    std::cout << "Parse '32' as int16_t" << std::endl;
    int16_t val;
    try {
      val = anch::convert<int16_t>("32");
    } catch(const std::bad_cast& e) {
      std::cerr << "Unable to parse '32' as int16_t" << std::endl;
      return 1;
    }
    std::cout << "Parsed value is " << val << std::endl;
  }

  {
    std::cout << "Parse '-32' as int16_t" << std::endl;
    int16_t val;
    try {
      val = anch::convert<int16_t>("-32");
    } catch(const std::bad_cast& e) {
      std::cerr << "Unable to parse '-32' as int16_t" << std::endl;
      return 1;
    }
    std::cout << "Parsed value is " << val << std::endl;
  }

  {
    std::cout << "Parse '32.2' as float" << std::endl;
    float val;
    try {
      val = anch::convert<float>("32.2");
    } catch(const std::bad_cast& e) {
      std::cout << "Unable to parse '32.2' as float" << std::endl;
      return 1;
    }
    std::cout << "Parsed value is " << val << std::endl;
  }

  {
    std::cout << "Parse '32e-1' as float" << std::endl;
    float val;
    try {
      val = anch::convert<float>("32e-1");
    } catch(const std::bad_cast& e) {
      std::cout << "Unable to parse '32e-1' as float" << std::endl;
      return 1;
    }
    std::cout << "Parsed value is " << val << std::endl;
  }

  return 0;
}
