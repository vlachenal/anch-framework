#include <iostream>
#include <fstream>

#include "crypto/cipher/cfb.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/cipher/aes192.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/hash/sha1.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::CFB;
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


void testSequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CFB sequential with AES128 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cfbOutCipher("Makefile.cfb.aes128.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CFB<AES128> cfb(IV);
  start = std::chrono::high_resolution_clock::now();
  cfb.cipher(*input, cfbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutCipher.close();
  input->close();
  delete input;
  std::cout << "CFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cfb.aes128.cipher" << std::endl;
  input = new std::ifstream("Makefile.cfb.aes128.cipher", std::ifstream::binary);
  std::ofstream cfbOutDecipher("Makefile.cfb.aes128.decipher");
  start = std::chrono::high_resolution_clock::now();
  cfb.decipher(*input, cfbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cfb.aes128.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CFB with sequential AES128 tests" << std::endl;
}

void testParallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CFB with parallel AES128 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cfbOutCipher("Makefile.cfb.aes128.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CFB<AES128> cfb(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cfb.cipher(*input, cfbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutCipher.close();
  input->close();
  delete input;
  std::cout << "CFB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cfb.aes128.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cfb.aes128.async.cipher", std::ifstream::binary);
  std::ofstream cfbOutDecipher("Makefile.cfb.aes128.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cfb.decipher(*input, cfbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CFB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cfb.aes128.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CFB with parallel AES128 tests" << std::endl;
}

void testSequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CFB sequential with AES192 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cfbOutCipher("Makefile.cfb.aes192.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CFB<AES192> cfb(IV);
  start = std::chrono::high_resolution_clock::now();
  cfb.cipher(*input, cfbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutCipher.close();
  input->close();
  delete input;
  std::cout << "CFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cfb.aes192.cipher" << std::endl;
  input = new std::ifstream("Makefile.cfb.aes192.cipher", std::ifstream::binary);
  std::ofstream cfbOutDecipher("Makefile.cfb.aes192.decipher");
  start = std::chrono::high_resolution_clock::now();
  cfb.decipher(*input, cfbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cfb.aes192.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CFB with sequential AES192 tests" << std::endl;
}

void testParallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CFB with parallel AES192 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cfbOutCipher("Makefile.cfb.aes192.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CFB<AES192> cfb(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cfb.cipher(*input, cfbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutCipher.close();
  input->close();
  delete input;
  std::cout << "CFB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cfb.aes192.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cfb.aes192.async.cipher", std::ifstream::binary);
  std::ofstream cfbOutDecipher("Makefile.cfb.aes192.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cfb.decipher(*input, cfbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CFB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cfb.aes192.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CFB with parallel AES192 tests" << std::endl;
}

void testSequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CFB sequential with AES256 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cfbOutCipher("Makefile.cfb.aes256.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CFB<AES256> cfb(IV);
  start = std::chrono::high_resolution_clock::now();
  cfb.cipher(*input, cfbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutCipher.close();
  input->close();
  delete input;
  std::cout << "CFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cfb.aes256.cipher" << std::endl;
  input = new std::ifstream("Makefile.cfb.aes256.cipher", std::ifstream::binary);
  std::ofstream cfbOutDecipher("Makefile.cfb.aes256.decipher");
  start = std::chrono::high_resolution_clock::now();
  cfb.decipher(*input, cfbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cfb.aes256.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CFB with sequential AES256 tests" << std::endl;
}

void testParallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CFB with parallel AES256 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cfbOutCipher("Makefile.cfb.aes256.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CFB<AES256> cfb(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cfb.cipher(*input, cfbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutCipher.close();
  input->close();
  delete input;
  std::cout << "CFB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cfb.aes256.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cfb.aes256.async.cipher", std::ifstream::binary);
  std::ofstream cfbOutDecipher("Makefile.cfb.aes256.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cfb.decipher(*input, cfbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cfbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CFB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cfb.aes256.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CFB with parallel AES256 tests" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH AES CFB unit tests")
    .description("Test AnCH AES CFB library")
    .initialize(beforeAll)
    .add("128-sequential", testSequential128)
    .add("192-sequential", testSequential192)
    .add("256-sequential", testSequential256)
    .add("128-parallel", testParallel128)
    .add("192-parallel", testParallel192)
    .add("256-parallel", testParallel256)
    ;
}
