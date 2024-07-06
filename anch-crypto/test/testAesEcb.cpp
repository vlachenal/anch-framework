#include <iostream>
#include <fstream>

#include "crypto/cipher/ecb.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/cipher/aes192.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/hash/sha1.hpp"

#include "crypto/padding/zeroPadding.hpp"
#include "crypto/padding/pkcs5Padding.hpp"
#include "crypto/padding/iso7816_4Padding.hpp"
#include "crypto/padding/ansiX923.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::ECB;
using anch::crypto::AES128;
using anch::crypto::AES192;
using anch::crypto::AES256;
using anch::crypto::SHA1;
using anch::crypto::ISO7816_4Padding;
using anch::crypto::PKCS5Padding;
using anch::crypto::ZeroPadding;
using anch::crypto::ANSIX923;

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
testAesEcbISO7816_4sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES128 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes128.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES128,ISO7816_4Padding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes128.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes128.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes128.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes128.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES128 and ISO7816_4 padding tests" << std::endl;
}

void
testAesEcbISO7816_4parallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES128 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes128.ISO7816_4.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES128,ISO7816_4Padding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes128.ISO7816_4.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes128.ISO7816_4.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes128.ISO7816_4.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes128.ISO7816_4.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES128 and ISO7816_4 padding tests" << std::endl;
}

void
testPKCS7sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES128 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes128.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES128,PKCS5Padding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes128.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes128.pkcs5.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes128.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes128.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES128 and PKCS#7 padding tests" << std::endl;
}

void
testPKCS7parallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES128 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes128.pkcs5.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES128,PKCS5Padding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes128.pkcs5.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes128.pkcs5.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes128.pkcs5.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes128.pkcs5.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES128 and PKCS#7 padding tests" << std::endl;
}

void
test0sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES128 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes128.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES128,ZeroPadding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes128.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes128.zero.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes128.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes128.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES128 and zero padding tests" << std::endl;
}

void
test0parallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES128 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes128.zero.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES128,ZeroPadding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes128.zero.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes128.zero.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes128.zero.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes128.zero.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES128 and zero padding tests" << std::endl;
}

void
testANSIX923sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES128 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes128.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES128,ANSIX923> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes128.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes128.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes128.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes128.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES128 and ANSIX923 padding tests" << std::endl;
}

void
testANSIX923parallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES128 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes128.ANSIX923.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES128,ANSIX923> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes128.ANSIX923.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes128.ANSIX923.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes128.ANSIX923.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes128.ANSIX923.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES128 and ANSIX923 padding tests" << std::endl;
}

void
testAesEcbISO7816_4sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES192 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes192.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES192,ISO7816_4Padding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes192.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes192.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes192.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes192.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES192 and ISO7816_4 padding tests" << std::endl;
}

void
testAesEcbISO7816_4parallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES192 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes192.ISO7816_4.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES192,ISO7816_4Padding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes192.ISO7816_4.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes192.ISO7816_4.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes192.ISO7816_4.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes192.ISO7816_4.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES192 and ISO7816_4 padding tests" << std::endl;
}

void
testPKCS7sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES192 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes192.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES192,PKCS5Padding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes192.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes192.pkcs5.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes192.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes192.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES192 and PKCS#7 padding tests" << std::endl;
}

void
testPKCS7parallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES192 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes192.pkcs5.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES192,PKCS5Padding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes192.pkcs5.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes192.pkcs5.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes192.pkcs5.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes192.pkcs5.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES192 and PKCS#7 padding tests" << std::endl;
}

void
test0sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES192 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes192.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES192,ZeroPadding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes192.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes192.zero.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes192.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes192.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES192 and zero padding tests" << std::endl;
}

void
test0parallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES192 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes192.zero.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES192,ZeroPadding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes192.zero.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes192.zero.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes192.zero.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes192.zero.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES192 and zero padding tests" << std::endl;
}

void
testANSIX923sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES192 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes192.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES192,ANSIX923> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes192.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes192.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes192.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes192.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES192 and ANSIX923 padding tests" << std::endl;
}

void
testANSIX923parallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES192 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes192.ANSIX923.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES192,ANSIX923> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes192.ANSIX923.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes192.ANSIX923.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes192.ANSIX923.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes192.ANSIX923.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES192 and ANSIX923 padding tests" << std::endl;
}

void
testAesEcbISO7816_4sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES256 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes256.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES256,ISO7816_4Padding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes256.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes256.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes256.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes256.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES256 and ISO7816_4 padding tests" << std::endl;
}

void
testAesEcbISO7816_4parallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES256 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes256.ISO7816_4.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES256,ISO7816_4Padding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes256.ISO7816_4.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes256.ISO7816_4.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes256.ISO7816_4.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes256.ISO7816_4.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES256 and ISO7816_4 padding tests" << std::endl;
}

void
testPKCS7sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES256 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes256.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES256,PKCS5Padding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes256.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes256.pkcs5.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes256.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes256.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES256 and PKCS#7 padding tests" << std::endl;
}

void
testPKCS7parallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES256 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes256.pkcs5.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES256,PKCS5Padding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes256.pkcs5.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes256.pkcs5.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes256.pkcs5.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes256.pkcs5.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES256 and PKCS#7 padding tests" << std::endl;
}

void
test0sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES256 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes256.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES256,ZeroPadding> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes256.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes256.zero.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes256.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes256.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES256 and zero padding tests" << std::endl;
}

void
test0parallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES256 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes256.zero.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES256,ZeroPadding> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes256.zero.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes256.zero.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes256.zero.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes256.zero.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES256 and zero padding tests" << std::endl;
}

void
testANSIX923sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB sequential with AES256 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes256.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES256,ANSIX923> ecb;
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes256.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes256.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes256.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes256.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with sequential AES256 and ANSIX923 padding tests" << std::endl;
}

void
testANSIX923parallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in ECB with parallel AES256 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream ecbOutCipher("Makefile.ecb.aes256.ANSIX923.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  ECB<AES256,ANSIX923> ecb(2);
  start = std::chrono::high_resolution_clock::now();
  ecb.cipher(*input, ecbOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutCipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.ecb.aes256.ANSIX923.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.ecb.aes256.ANSIX923.async.cipher", std::ifstream::binary);
  std::ofstream ecbOutDecipher("Makefile.ecb.aes256.ANSIX923.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  ecb.decipher(*input, ecbOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  ecbOutDecipher.close();
  input->close();
  delete input;
  std::cout << "ECB parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.ecb.aes256.ANSIX923.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit ECB with parallel AES256 and ANSIX923 padding tests" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH AES ECB unit tests")
    .description("Test AnCH AES ECB library")
    .initialize(beforeAll)
    .add("ISO7816_4-sequential-128", testAesEcbISO7816_4sequential128)
    .add("ISO7816_4-parallel-128", testAesEcbISO7816_4parallel128)
    .add("PKCS7-sequential-128", testPKCS7sequential128)
    .add("PKCS7-parallel-128", testPKCS7parallel128)
    .add("0-sequential-128", test0sequential128)
    .add("0-parallel-128", test0parallel128)
    .add("ANSIX923-sequential-128", testANSIX923sequential128)
    .add("ANSIX923-parallel-128", testANSIX923parallel128)
    .add("ISO7816_4-sequential-192", testAesEcbISO7816_4sequential192)
    .add("ISO7816_4-parallel-192", testAesEcbISO7816_4parallel192)
    .add("PKCS7-sequential-192", testPKCS7sequential192)
    .add("PKCS7-parallel-192", testPKCS7parallel192)
    .add("0-sequential-192", test0sequential192)
    .add("0-parallel-192", test0parallel192)
    .add("ANSIX923-sequential-192", testANSIX923sequential192)
    .add("ANSIX923-parallel-192", testANSIX923parallel192)
    .add("ISO7816_4-sequential-256", testAesEcbISO7816_4sequential256)
    .add("ISO7816_4-parallel-256", testAesEcbISO7816_4parallel256)
    .add("PKCS7-sequential-256", testPKCS7sequential256)
    .add("PKCS7-parallel-256", testPKCS7parallel256)
    .add("0-sequential-256", test0sequential256)
    .add("0-parallel-256", test0parallel256)
    .add("ANSIX923-sequential-256", testANSIX923sequential256)
    .add("ANSIX923-parallel-256", testANSIX923parallel256)
    ;
}
