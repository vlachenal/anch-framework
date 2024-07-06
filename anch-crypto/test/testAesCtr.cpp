#include <iostream>
#include <fstream>

#include "crypto/cipher/ctr.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/cipher/aes192.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/hash/sha1.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::CTR;
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

void testAesCtrSequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CTR sequential with AES128 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ctrOutCipher("Makefile.ctr.aes128.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CTR<AES128> ctr(IV);
  start = std::chrono::high_resolution_clock::now();
  ctr.cipher(*input, ctrOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutCipher.close();
  input->close();
  delete input;
  std::cout << "CTR sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ctr.aes128.cipher" << std::endl;
  input = new std::ifstream("Makefile.ctr.aes128.cipher", std::ifstream::binary);
  std::ofstream ctrOutDecipher("Makefile.ctr.aes128.decipher");
  start = std::chrono::high_resolution_clock::now();
  ctr.decipher(*input, ctrOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CTR sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ctr.aes128.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CTR with sequential AES128 tests" << std::endl;
}

void testAesCtrParallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CTR with parallel AES128 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ctrOutCipher("Makefile.ctr.aes128.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CTR<AES128> ctr(IV,4);
  start = std::chrono::high_resolution_clock::now();
  ctr.cipher(*input, ctrOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutCipher.close();
  input->close();
  delete input;
  std::cout << "CTR parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ctr.aes128.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ctr.aes128.async.cipher", std::ifstream::binary);
  std::ofstream ctrOutDecipher("Makefile.ctr.aes128.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ctr.decipher(*input, ctrOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CTR parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ctr.aes128.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CTR with parallel AES128 tests" << std::endl;
}

void testAesCtrSequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CTR sequential with AES256 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ctrOutCipher("Makefile.ctr.aes256.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CTR<AES256> ctr(IV);
  start = std::chrono::high_resolution_clock::now();
  ctr.cipher(*input, ctrOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutCipher.close();
  input->close();
  delete input;
  std::cout << "CTR sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ctr.aes256.cipher" << std::endl;
  input = new std::ifstream("Makefile.ctr.aes256.cipher", std::ifstream::binary);
  std::ofstream ctrOutDecipher("Makefile.ctr.aes256.decipher");
  start = std::chrono::high_resolution_clock::now();
  ctr.decipher(*input, ctrOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CTR sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ctr.aes256.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CTR with sequential AES256 tests" << std::endl;
}

void testAesCtrParallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CTR with parallel AES256 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ctrOutCipher("Makefile.ctr.aes256.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CTR<AES256> ctr(IV,4);
  start = std::chrono::high_resolution_clock::now();
  ctr.cipher(*input, ctrOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutCipher.close();
  input->close();
  delete input;
  std::cout << "CTR parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ctr.aes256.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ctr.aes256.async.cipher", std::ifstream::binary);
  std::ofstream ctrOutDecipher("Makefile.ctr.aes256.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ctr.decipher(*input, ctrOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CTR parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ctr.aes256.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CTR with parallel AES256 tests" << std::endl;
}

void testAesCtrSequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CTR sequential with AES192 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ctrOutCipher("Makefile.ctr.aes192.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CTR<AES192> ctr(IV);
  start = std::chrono::high_resolution_clock::now();
  ctr.cipher(*input, ctrOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutCipher.close();
  input->close();
  delete input;
  std::cout << "CTR sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ctr.aes192.cipher" << std::endl;
  input = new std::ifstream("Makefile.ctr.aes192.cipher", std::ifstream::binary);
  std::ofstream ctrOutDecipher("Makefile.ctr.aes192.decipher");
  start = std::chrono::high_resolution_clock::now();
  ctr.decipher(*input, ctrOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CTR sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ctr.aes192.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CTR with sequential AES192 tests" << std::endl;
}

void testAesCtrParallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CTR with parallel AES192 tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ctrOutCipher("Makefile.ctr.aes192.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CTR<AES192> ctr(IV,4);
  start = std::chrono::high_resolution_clock::now();
  ctr.cipher(*input, ctrOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutCipher.close();
  input->close();
  delete input;
  std::cout << "CTR parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ctr.aes192.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ctr.aes192.async.cipher", std::ifstream::binary);
  std::ofstream ctrOutDecipher("Makefile.ctr.aes192.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ctr.decipher(*input, ctrOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ctrOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CTR parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ctr.aes192.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CTR with parallel AES192 tests" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH AES CTR unit tests")
    .description("Test AnCH CTR ECB library")
    .initialize(beforeAll)
    .add("sequential-128", testAesCtrSequential128)
    .add("parallel-128", testAesCtrParallel128)
    .add("sequential-192", testAesCtrSequential192)
    .add("parallel-192", testAesCtrParallel192)
    .add("sequential-256", testAesCtrSequential256)
    .add("parallel-256", testAesCtrParallel256)
    ;
}
