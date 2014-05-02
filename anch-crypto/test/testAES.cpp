#include "crypto/cipher/aes128.hpp"
#include "crypto/cipher/aes192.hpp"
#include "crypto/cipher/aes256.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>


// bool
// equals(uint8_t a[16], uint8_t b[16]) {
//   for(int i = 0 ; i < 15 ; i++) {
//     if(a[i] != b[i]) {
//       return false;
//     }
//   }
//   return true;
// }

std::string
formatData(uint8_t* data, std::size_t len) {
  std::ostringstream out;
  out << std::hex;
  for(std::size_t i = 0 ; i < len ; i++) {
    out << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(data[i]);
  }
  return out.str();
}

int
main(void) {
  std::array<uint8_t,16> input;
  input[0] = 0x00;
  input[1] = 0x11;
  input[2] = 0x22;
  input[3] = 0x33;
  input[4] = 0x44;
  input[5] = 0x55;
  input[6] = 0x66;
  input[7] = 0x77;
  input[8] = 0x88;
  input[9] = 0x99;
  input[10] = 0xAA;
  input[11] = 0xBB;
  input[12] = 0xCC;
  input[13] = 0xDD;
  input[14] = 0xEE;
  input[15] = 0xFF;
  std::array<uint8_t,32> key;
  for(uint8_t i = 0 ; i < 32 ; i++) {
    key[i] = i;
  }

  std::cout << "Begin specification vector tests" << std::endl << std::endl;

  std::cout << "AES-128 - Cipher" << std::endl;
  anch::crypto::AES128 aes128(key.data());
  std::array<uint8_t,16> expRes = {{
      0x69,0xc4,0xe0,0xd8,0x6a,0x7b,0x04,0x30,0xd8,0xcd,0xb7,0x80,0x70,0xb4,0xc5,0x5a
    }};
  std::array<uint8_t,16> output;
  std::cout << "Encrypt " << formatData(input.data(),16) << " uisng key " << formatData(key.data(),16) << std::endl;
  aes128.cipher(input, output);
  std::cout << "Found " << formatData(output.data(),16) << std::endl;
  if(expRes != output) {
    std::cerr << "Unexpected result" << std::endl;
    return 1;
  }
  std::array<uint8_t,16> decInput;
  decInput = output;
  std::cout << std::endl << "AES-128 - Decipher" << std::endl;
  std::cout << "Decrypt " << formatData(output.data(),16) << " using key " << formatData(key.data(),16) << std::endl;
  aes128.decipher(decInput, output);
  std::cout << "Found " << formatData(output.data(),16) << std::endl;
  if(input != output) {
    std::cerr << "Unexpected result" << std::endl;
    return 1;
  }

  std::cout << std::endl << "AES-192 - Cipher" << std::endl;
  anch::crypto::AES192 aes192(key.data());
  std::array<uint8_t,16> expRes2 = {{
      0xdd,0xa9,0x7c,0xa4,0x86,0x4c,0xdf,0xe0,0x6e,0xaf,0x70,0xa0,0xec,0x0d,0x71,0x91
    }};
  std::cout << "Encrypt " << formatData(input.data(),16) << " uisng key " << formatData(key.data(),24) << std::endl;
  aes192.cipher(input, output);
  std::cout << "Found " << formatData(output.data(),16) << std::endl;
  if(expRes2 != output) {
    std::cerr << "Unexpected result" << std::endl;
    return 1;
  }
  decInput = output;
  std::cout << std::endl << "AES-192 - Decipher" << std::endl;
  std::cout << "Decrypt " << formatData(output.data(),16) << " using key " << formatData(key.data(),24) << std::endl;
  aes192.decipher(decInput, output);
  std::cout << "Found " << formatData(output.data(),16) << std::endl;
  if(input != output) {
    std::cerr << "Unexpected result" << std::endl;
    return 1;
  }

  std::cout << std::endl << "AES-256 - Cipher" << std::endl;
  anch::crypto::AES256 aes256(key.data());
  std::array<uint8_t,16> expRes3 = {{
      0x8e,0xa2,0xb7,0xca,0x51,0x67,0x45,0xbf,0xea,0xfc,0x49,0x90,0x4b,0x49,0x60,0x89
    }};
  std::cout << "Encrypt " << formatData(input.data(),16) << " uisng key " << formatData(key.data(),32) << std::endl;
  aes256.cipher(input, output);
  std::cout << "Found " << formatData(output.data(),16) << std::endl;
  if(expRes3 != output) {
    std::cerr << "Unexpected result" << std::endl;
    return 1;
  }
  decInput = output;
  std::cout << std::endl << "AES-256 - Decipher" << std::endl;
  std::cout << "Decrypt " << formatData(output.data(),16) << " using key " << formatData(key.data(),32) << std::endl;
  aes256.decipher(decInput, output);
  std::cout << "Found " << formatData(output.data(),16) << std::endl;
  if(input != output) {
    std::cerr << "Unexpected result" << std::endl;
    return 1;
  }

  std::cout << "End specification vector tests" << std::endl;

  return 0;
}
