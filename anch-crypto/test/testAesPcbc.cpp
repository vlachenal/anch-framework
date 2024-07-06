#include <iostream>
#include <fstream>

#include "crypto/cipher/pcbc.hpp"
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

using anch::crypto::PCBC;
using anch::crypto::AES128;
using anch::crypto::AES192;
using anch::crypto::AES256;
using anch::crypto::SHA1;
using anch::crypto::ISO7816_4Padding;
using anch::crypto::PKCS5Padding;
using anch::crypto::ZeroPadding;
using anch::crypto::ANSIX923;

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
testISO7816_4sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES128 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES128,ISO7816_4Padding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes128.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes128.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes128.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes128.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES128 and ISO7816_4 padding tests" << std::endl;
}

void testPKCS7sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES128 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES128,PKCS5Padding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes128.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes128.pkcs5.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes128.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes128.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES128 and PKCS#7 padding tests" << std::endl;
}

void
test0sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES128 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES128,ZeroPadding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes128.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes128.zero.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes128.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes128.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES128 and zero padding tests" << std::endl;
}

void
testANSIX923sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES128 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES128,ANSIX923> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes128.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes128.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes128.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes128.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES128 and ANSIX923 padding tests" << std::endl;
}

void
testISO7816_4sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES192 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes192.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES192,ISO7816_4Padding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes192.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes192.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes192.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes192.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES192 and ISO7816_4 padding tests" << std::endl;
}

void
testPKCS7sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES192 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes192.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES192,PKCS5Padding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes192.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes192.pkcs5.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes192.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes192.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES192 and PKCS#7 padding tests" << std::endl;
}

void
test0sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES192 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes192.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES192,ZeroPadding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes192.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes192.zero.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes192.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes192.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES192 and zero padding tests" << std::endl;
}

void
testANSIX923sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES192 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes192.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES192,ANSIX923> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes192.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes192.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes192.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes192.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES192 and ANSIX923 padding tests" << std::endl;
}

void
testISO7816_4sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES256 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes256.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES256,ISO7816_4Padding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes256.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes256.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes256.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes256.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES256 and ISO7816_4 padding tests" << std::endl;
}

void
testPKCS7sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES256 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes256.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES256,PKCS5Padding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes256.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes256.pkcs5.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes256.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes256.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES256 and PKCS#7 padding tests" << std::endl;
}

void
test0sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES256 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes256.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES256,ZeroPadding> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes256.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes256.zero.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes256.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes256.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES256 and zero padding tests" << std::endl;
}

void
testANSIX923sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in PCBC sequential with AES256 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream pcbcOutCipher("Makefile.pcbc.aes256.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  PCBC<AES256,ANSIX923> pcbc(IV);
  start = std::chrono::high_resolution_clock::now();
  pcbc.cipher(*input, pcbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.pcbc.aes256.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.pcbc.aes256.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream pcbcOutDecipher("Makefile.pcbc.aes256.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  pcbc.decipher(*input, pcbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  pcbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "PCBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.pcbc.aes256.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit PCBC with sequential AES256 and ANSIX923 padding tests" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH AES ECB unit tests")
    .description("Test AnCH AES ECB library")
    .initialize(beforeAll)
    .add("ISO7816_4-128", testISO7816_4sequential128)
    .add("PKCS7-128", testPKCS7sequential128)
    .add("0-128", test0sequential128)
    .add("ANSIX923-128", testANSIX923sequential128)
    .add("ISO7816_4-192", testISO7816_4sequential192)
    .add("PKCS7-192", testPKCS7sequential192)
    .add("0-192", test0sequential192)
    .add("ANSIX923-192", testANSIX923sequential192)
    .add("ISO7816_4-256", testISO7816_4sequential256)
    .add("PKCS7-256", testPKCS7sequential256)
    .add("0-256", test0sequential256)
    .add("ANSIX923-256", testANSIX923sequential256)
    ;
}
