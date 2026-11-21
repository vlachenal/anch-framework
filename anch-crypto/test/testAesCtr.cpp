/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2014 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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

std::string
fromHex(const std::string& hex) {
  std::string res;
  for(std::size_t i = 0 ; i + 1 < hex.size() ; i += 2) {
    res += static_cast<char>(std::stoi(hex.substr(i, 2), nullptr, 16));
  }
  return res;
}

std::array<uint8_t,16>
blockFromHex(const std::string& hex) {
  std::string bytes = fromHex(hex);
  std::array<uint8_t,16> block;
  for(std::size_t i = 0 ; i < block.size() ; ++i) {
    block[i] = static_cast<uint8_t>(bytes[i]);
  }
  return block;
}

template<typename Cipher>
std::string
ctrCipher(const std::array<uint8_t,16>& nonce, const std::string& key, const std::string& data, unsigned int nbThread) {
  CTR<Cipher> ctr(nonce, nbThread);
  std::istringstream input(data);
  std::ostringstream output;
  ctr.cipher(input, output, key);
  return output.str();
}

template<typename Cipher>
std::string
ctrDecipher(const std::array<uint8_t,16>& nonce, const std::string& key, const std::string& data, unsigned int nbThread) {
  CTR<Cipher> ctr(nonce, nbThread);
  std::istringstream input(data);
  std::ostringstream output;
  ctr.decipher(input, output, key);
  return output.str();
}

// NIST SP 800-38A F.5 vectors: the initial counter f0f1...feff carries over several bytes
const std::string NIST_COUNTER = "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";
const std::string NIST_PLAIN = "6bc1bee22e409f96e93d7e117393172a"
  "ae2d8a571e03ac9c9eb76fac45af8e51"
  "30c81c46a35ce411e5fbc1191a0a52ef"
  "f69f2445df4f9b17ad2b417be66c3710";

template<typename Cipher>
void
testNist(const std::string& key, const std::string& expected) {
  std::array<uint8_t,16> nonce = blockFromHex(NIST_COUNTER);
  std::string plain = fromHex(NIST_PLAIN);
  std::string cipher = fromHex(expected);
  for(unsigned int nbThread : {1u, 3u}) {
    anch::ut::assertThat(ctrCipher<Cipher>(nonce, fromHex(key), plain, nbThread) == cipher,
			 "Wrong ciphered data with {} thread(s)", nbThread);
    anch::ut::assertThat(ctrDecipher<Cipher>(nonce, fromHex(key), cipher, nbThread) == plain,
			 "Wrong deciphered data with {} thread(s)", nbThread);
  }
  // Partial last block
  anch::ut::assertTrue(ctrCipher<Cipher>(nonce, fromHex(key), plain.substr(0, 40), 1) == cipher.substr(0, 40),
		       "Wrong ciphered partial block");
}

void
testNist128() {
  testNist<AES128>("2b7e151628aed2a6abf7158809cf4f3c",
		   "874d6191b620e3261bef6864990db6ce"
		   "9806f66b7970fdff8617187bb9fffdff"
		   "5ae4df3edbd5d35e5b4f09020db03eab"
		   "1e031dda2fbe03d1792170a0f3009cee");
}

void
testNist256() {
  testNist<AES256>("603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
		   "601ec313775789a5b7a7f504bbf3d228"
		   "f443e3ca4d62b59aca84e990cacaf5c5"
		   "2b0930daa23de94ce87017ba2d84988d"
		   "dfc9c58db67aada613c2dd08457941a6");
}

void
testCounterWrap() {
  // ff...ff + 1 wraps to 00...00: the second block uses the key stream of the first block with a null nonce
  std::string key = "foobar    raboof";
  std::string zeros(32, '\0');
  std::array<uint8_t,16> full;
  full.fill(0xff);
  std::array<uint8_t,16> null;
  null.fill(0);
  std::string wrapped = ctrCipher<AES128>(full, key, zeros, 1);
  std::string fromNull = ctrCipher<AES128>(null, key, zeros, 1);
  anch::ut::assertTrue(wrapped.substr(16, 16) == fromNull.substr(0, 16), "Counter does not wrap to 0");
}

void
testNoKeyStreamReuse() {
  // Former implementation kept 16 bits of the block index: block 65536 reused the key stream of block 0
  std::string zeros(16 * 65537, '\0');
  std::string cipher = ctrCipher<AES128>(IV, "foobar    raboof", zeros, 1);
  anch::ut::assertTrue(cipher.size() == zeros.size(), "Wrong ciphered data size");
  anch::ut::assertTrue(cipher.substr(0, 16) != cipher.substr(16 * 65536, 16), "Key stream is reused after 65536 blocks");
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
    .add("nist-128", testNist128)
    .add("nist-256", testNist256)
    .add("counter-wrap", testCounterWrap)
    .add("no-key-stream-reuse", testNoKeyStreamReuse)
    ;
}
