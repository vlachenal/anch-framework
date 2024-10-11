#include <iostream>
#include <cstdint>

#include "convert.hpp"

#include "ut/unit.hpp"
#include "ut/assert.hpp"

void
test32asu16() {
  std::cout << "Parse '32' as uint16_t" << std::endl;
  uint16_t val;
  try {
    val = anch::convert<uint16_t>("32");
    std::cout << "Parsed value is " << val << std::endl;
  } catch(const std::bad_cast& e) {
    std::cerr << "Unable to parse '32' as uint16_t" << std::endl;
    anch::ut::fail("Unable to parse '32' as uint16_t");
  }
}

void
testM32asu16() {
  std::cout << "Parse '-32' as uint16_t" << std::endl;
  uint16_t val;
  try {
    val = anch::convert<uint16_t>("-32");
    std::cout << "Parsed value is " << val << std::endl;
  } catch(const std::bad_cast& e) {
    std::cerr << "Unable to parse '-32' as uint16_t" << std::endl;
    anch::ut::fail("Unable to parse '-32' as uint16_t");
  }
}

void
test32dot2asu16() {
  std::cout << "Parse '32.2' as uint16_t" << std::endl;
  uint16_t val;
  try {
    val = anch::convert<uint16_t>("32.2");
    std::cerr << "Parsed value is " << val << std::endl;
    anch::ut::fail();
  } catch(const std::bad_cast& e) {
    std::cout << "Unable to parse '32.2' as uint16_t" << std::endl;
  }
}

void
test32em1asu16() {
  std::cout << "Parse '32e-1' as uint16_t" << std::endl;
  uint16_t val;
  try {
    val = anch::convert<uint16_t>("32e-1");
    std::cerr << "Parsed value is " << val << std::endl;
    anch::ut::fail();
  } catch(const std::bad_cast& e) {
    std::cout << "Unable to parse '32e-1' as uint16_t" << std::endl;
  }
}

void
test32as16() {
  std::cout << "Parse '32' as int16_t" << std::endl;
  int16_t val;
  try {
    val = anch::convert<int16_t>("32");
    std::cout << "Parsed value is " << val << std::endl;
  } catch(const std::bad_cast& e) {
    std::cerr << "Unable to parse '32' as int16_t" << std::endl;
    anch::ut::fail("Unable to parse '32' as int16_t");
  }
}

void
testM32as16() {
  std::cout << "Parse '-32' as int16_t" << std::endl;
  int16_t val;
  try {
    val = anch::convert<int16_t>("-32");
    std::cout << "Parsed value is " << val << std::endl;
  } catch(const std::bad_cast& e) {
    std::cerr << "Unable to parse '-32' as int16_t" << std::endl;
    anch::ut::fail("Unable to parse '-32' as int16_t");
  }
}

void
test32dot2asFloat() {
  std::cout << "Parse '32.2' as float" << std::endl;
  float val;
  try {
    val = anch::convert<float>("32.2");
    std::cout << "Parsed value is " << val << std::endl;
  } catch(const std::bad_cast& e) {
    std::cout << "Unable to parse '32.2' as float" << std::endl;
    anch::ut::fail("Unable to parse '32.2' as int16_t");
  }
}

void
test32em1asFloat() {
  std::cout << "Parse '32e-1' as float" << std::endl;
  float val;
  try {
    val = anch::convert<float>("32e-1");
    std::cout << "Parsed value is " << val << std::endl;
  } catch(const std::bad_cast& e) {
    std::cout << "Unable to parse '32e-1' as float" << std::endl;
    anch::ut::fail("Unable to parse '32e-1' as int16_t");
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH convert unit tests")
    .description("Test AnCH convert utiliy class")
    .add("conv-32-as-ushort", test32asu16)
    .add("conv--32-as-ushort", testM32asu16)
    .add("conv--32.2-as-ushort", test32dot2asu16)
    .add("conv-32e-1-as-ushort", test32em1asu16)
    .add("conv-32-as-short", test32as16)
    .add("conv--32-as-short", testM32as16)
    .add("conv-32.2-as-short", test32dot2asFloat)
    .add("conv-32e-1-as-short", test32em1asFloat)
    ;
}
