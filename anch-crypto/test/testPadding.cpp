#include "crypto/padding/ansiX923.hpp"
#include "crypto/padding/pkcs5Padding.hpp"
#include "crypto/padding/iso7816_4Padding.hpp"
#include "crypto/padding/zeroPadding.hpp"

#include <iostream>
#include <iomanip>
#include <array>

using anch::crypto::ANSIX923;
using anch::crypto::PKCS5Padding;
using anch::crypto::ISO7816_4Padding;
using anch::crypto::ZeroPadding;


int
main(void) {
  std::array<uint8_t,16> initData = {{
      0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0x01,0x01,0x01,0x01,
      0x01,0x01
    }};
  std::array<uint8_t,16> data = initData;

  std::cout << "ANSI X.923" << std::endl;
  ANSIX923::pad(data.data(),10,16);
  std::cout << "Result: " << std::hex;
  for(std::size_t i = 0 ; i < 16 ; i++) {
    std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(data[i]) << ',';
  }
  std::cout << std::endl;
  for(std::size_t i = 0 ; i < 10 ; i++) {
    if(data[i] != initData[i]) {
      std::cerr << "Invalid found at index " << i << std::endl;
      return 1;
    }
  }
  std::size_t len = ANSIX923::length(data.data(),16);
  std::cout << "Data length: " << std::dec << len << std::endl;
  if(len != 10) {
    std::cerr << "Invalid size (10 expected)" << std::endl;
  }

  data = initData;
  std::cout << "PKCS#7" << std::endl;
  PKCS5Padding::pad(data.data(),10,16);
  std::cout << "Result: " << std::hex;
  for(std::size_t i = 0 ; i < 16 ; i++) {
    std::cout << "0x" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(data[i]) << ',';
  }
  std::cout << std::endl;
  for(std::size_t i = 0 ; i < 10 ; i++) {
    if(data[i] != initData[i]) {
      std::cerr << "Invalid found at index " << i << std::endl;
      return 1;
    }
  }
  len = PKCS5Padding::length(data.data(),16);
  std::cout << "Data length: " << std::dec << len << std::endl;
  if(len != 10) {
    std::cerr << "Invalid size (10 expected)" << std::endl;
  }

  data = initData;
  std::cout << "ISO/IEC 7816-4" << std::endl;
  ISO7816_4Padding::pad(data.data(),10,16);
  std::cout << "Result: " << std::hex;
  for(std::size_t i = 0 ; i < 16 ; i++) {
    std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(data[i]) << ',';
  }
  std::cout << std::endl;
  for(std::size_t i = 0 ; i < 10 ; i++) {
    if(data[i] != initData[i]) {
      std::cerr << "Invalid found at index " << i << std::endl;
      return 1;
    }
  }
  len = ISO7816_4Padding::length(data.data(),16);
  std::cout << "Data length: " << std::dec << len << std::endl;
  if(len != 10) {
    std::cerr << "Invalid size (10 expected)" << std::endl;
  }

  data = initData;
  std::cout << "Zero padding" << std::endl;
  ZeroPadding::pad(data.data(),10,16);
  std::cout << "Result: " << std::hex;
  for(std::size_t i = 0 ; i < 16 ; i++) {
    std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(data[i]) << ',';
  }
  std::cout << std::endl;
  for(std::size_t i = 0 ; i < 10 ; i++) {
    if(data[i] != initData[i]) {
      std::cerr << "Invalid found at index " << i << std::endl;
      return 1;
    }
  }
  len = ZeroPadding::length(data.data(),16);
  std::cout << "Data length: " << std::dec << len << std::endl;
  if(len != 10) {
    std::cerr << "Invalid size (10 expected)" << std::endl;
  }

  return 0;
}
