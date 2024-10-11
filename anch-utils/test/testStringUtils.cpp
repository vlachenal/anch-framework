#include <iostream>

#include "stringUtils.hpp"

#include "ut/unit.hpp"
#include "ut/assert.hpp"

std::string orig;

void
beforeEach() {
  orig = "PLip plOP";
  std::cout << "Original: " << orig << std::endl;
}

void
testToUpper() {
  std::cout << "Enter in toUpper test" << std::endl;
  anch::toUpper(orig);
  std::cout << "toUpper: " << orig << std::endl;
  if(orig != "PLIP PLOP") {
    std::cerr << "toUpper does not work: " << orig << std::endl;
    anch::ut::fail();
  }
  std::cout << "Exit toUpper test" << std::endl;
}

void
testToLower() {
  std::cout << "Enter in toLower test" << std::endl;
  anch::toUpper(orig);
  std::cout << "toUpper: " << orig << std::endl;
  if(orig != "PLIP PLOP") {
    std::cerr << "toLower does not work: " << orig << std::endl;
    anch::ut::fail();
  }
  std::cout << "Exit toLower test" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH string utils unit tests")
    .description("Test AnCH string utils utiliy class")
    .beforeTest(beforeEach)
    .add("to-upper", testToUpper)
    .add("to-lowwer", testToLower)
    ;
}
