#include <iostream>
#include <fstream>

#include "crypto/cipher/ofb.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/cipher/aes192.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/hash/sha1.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::OFB;
using anch::crypto::AES128;
using anch::crypto::AES192;
using anch::crypto::AES256;
using anch::crypto::SHA1;

const std::array<uint8_t, 16> IV = { {
    'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
  } };

std::array<uint8_t,20> initHash;

std::array<uint8_t,20>
hashSHA1(const std::string& path) {
  std::cout << "Compute " << path << " hash using SHA1" << std::endl;
  std::ifstream input(path, std::ifstream::binary);
  SHA1 hash(input);
  return hash.digest();
}

void
beforeAll() {
  initHash = hashSHA1("Makefile");
}


void
testAesOfb128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in OFB sequential with AES128 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ofbOutCipher("Makefile.ofb.aes128.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  OFB<AES128> ofb(IV);
  start = std::chrono::high_resolution_clock::now();
  ofb.cipher(*input, ofbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ofbOutCipher.close();
  input->close();
  delete input;
  std::cout << "OFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ofb.aes128.cipher" << std::endl;
  input = new std::ifstream("Makefile.ofb.aes128.cipher", std::ifstream::binary);
  std::ofstream ofbOutDecipher("Makefile.ofb.aes128.decipher");
  start = std::chrono::high_resolution_clock::now();
  ofb.decipher(*input, ofbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ofbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "OFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ofb.aes128.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit OFB with sequential AES128 tests" << std::endl;
}

void
testAesOfb192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in OFB sequential with AES192 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ofbOutCipher("Makefile.ofb.aes192.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  OFB<AES192> ofb(IV);
  start = std::chrono::high_resolution_clock::now();
  ofb.cipher(*input, ofbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ofbOutCipher.close();
  input->close();
  delete input;
  std::cout << "OFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ofb.aes192.cipher" << std::endl;
  input = new std::ifstream("Makefile.ofb.aes192.cipher", std::ifstream::binary);
  std::ofstream ofbOutDecipher("Makefile.ofb.aes192.decipher");
  start = std::chrono::high_resolution_clock::now();
  ofb.decipher(*input, ofbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ofbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "OFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ofb.aes192.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit OFB with sequential AES192 tests" << std::endl;
}

void
testAesOfb256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in OFB sequential with AES256 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ofbOutCipher("Makefile.ofb.aes256.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  OFB<AES256> ofb(IV);
  start = std::chrono::high_resolution_clock::now();
  ofb.cipher(*input, ofbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ofbOutCipher.close();
  input->close();
  delete input;
  std::cout << "OFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ofb.aes256.cipher" << std::endl;
  input = new std::ifstream("Makefile.ofb.aes256.cipher", std::ifstream::binary);
  std::ofstream ofbOutDecipher("Makefile.ofb.aes256.decipher");
  start = std::chrono::high_resolution_clock::now();
  ofb.decipher(*input, ofbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ofbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "OFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ofb.aes256.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit OFB with sequential AES256 tests" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH AES OFB unit tests")
    .description("Test AnCH AES OFB library")
    .initialize(beforeAll)
    .add("128", testAesOfb128)
    .add("192", testAesOfb192)
    .add("256", testAesOfb256)
    ;
}
