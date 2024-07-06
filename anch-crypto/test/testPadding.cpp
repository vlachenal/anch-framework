#include "crypto/padding/ansiX923.hpp"
#include "crypto/padding/pkcs5Padding.hpp"
#include "crypto/padding/iso7816_4Padding.hpp"
#include "crypto/padding/zeroPadding.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <string.h>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::ANSIX923;
using anch::crypto::PKCS5Padding;
using anch::crypto::ISO7816_4Padding;
using anch::crypto::ZeroPadding;


const std::array<uint8_t,16> INIT_DATA = {{
    0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0x01,0x01,0x01,0x01,
    0x01,0x01
  }};

std::array<uint8_t,16> data;

void
before() {
  data = INIT_DATA;
}

void
testANSIX923() {
  std::cout << "ANSI X.923" << std::endl;
  ANSIX923::pad(data.data(),10,16);
  std::cout << "Result: " << std::hex;
  for(std::size_t i = 0 ; i < 16 ; i++) {
    std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(data[i]) << ',';
  }
  std::cout << std::endl;
  for(std::size_t i = 0 ; i < 10 ; i++) {
    anch::ut::assertTrue(data[i] == INIT_DATA[i]);
  }
  std::size_t len = ANSIX923::length(data.data(),16);
  std::cout << "Data length: " << std::dec << len << std::endl;
  anch::ut::assertTrue(len == 10);
}

void
testPKCS7() {
  std::cout << "PKCS#7" << std::endl;
  PKCS5Padding::pad(data.data(),10,16);
  std::cout << "Result: " << std::hex;
  for(std::size_t i = 0 ; i < 16 ; i++) {
    std::cout << "0x" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(data[i]) << ',';
  }
  std::cout << std::endl;
  for(std::size_t i = 0 ; i < 10 ; i++) {
    anch::ut::assertTrue(data[i] == INIT_DATA[i]);
  }
  std::size_t len = PKCS5Padding::length(data.data(),16);
  std::cout << "Data length: " << std::dec << len << std::endl;
  anch::ut::assertTrue(len == 10);
}

void
testISO7816_4() {
  std::cout << "ISO/IEC 7816-4" << std::endl;
  ISO7816_4Padding::pad(data.data(),10,16);
  std::cout << "Result: " << std::hex;
  for(std::size_t i = 0 ; i < 16 ; i++) {
    std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(data[i]) << ',';
  }
  std::cout << std::endl;
  for(std::size_t i = 0 ; i < 10 ; i++) {
    anch::ut::assertTrue(data[i] == INIT_DATA[i]);
  }
  std::size_t len = ISO7816_4Padding::length(data.data(),16);
  std::cout << "Data length: " << std::dec << len << std::endl;
  anch::ut::assertTrue(len == 10);
}

void
testZero() {
  std::cout << "Zero padding" << std::endl;
  ZeroPadding::pad(data.data(),10,16);
  std::cout << "Result: " << std::hex;
  for(std::size_t i = 0 ; i < 16 ; i++) {
    std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(data[i]) << ',';
  }
  std::cout << std::endl;
  for(std::size_t i = 0 ; i < 10 ; i++) {
    anch::ut::assertTrue(data[i] == INIT_DATA[i]);
  }
  std::size_t len = ZeroPadding::length(data.data(),16);
  std::cout << "Data length: " << std::dec << len << std::endl;
  anch::ut::assertTrue(len == 10);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH padding unit tests")
    .description("Test AnCH padding library")
    .beforeTest(before)
    .add("ANSIX923", testANSIX923)
    .add("PKCS7", testPKCS7)
    .add("ISO7816_4", testISO7816_4)
    .add("zero", testZero)
    ;
}
