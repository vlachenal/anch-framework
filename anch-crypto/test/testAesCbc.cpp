#include <iostream>
#include <fstream>

#include "crypto/cipher/cbc.hpp"
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

using anch::crypto::CBC;
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

  std::cout << "Enter in CBC sequential with AES128 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes128.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES128,ISO7816_4Padding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes128.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes128.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes128.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes128.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES128 and ISO7816_4 padding tests" << std::endl;
}

void
testISO7816_4parallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES128 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes128.ISO7816_4.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES128,ISO7816_4Padding> cbc(IV,3);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes128.ISO7816_4.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes128.ISO7816_4.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes128.ISO7816_4.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes128.ISO7816_4.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES128 and ISO7816_4 padding tests" << std::endl;
}

void testPKCS7sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES128 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes128.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES128,PKCS5Padding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes128.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes128.pkcs5.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes128.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes128.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES128 and PKCS#7 padding tests" << std::endl;
}

void testPKCS7parallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES128 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes128.pkcs5.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES128,PKCS5Padding> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes128.pkcs5.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes128.pkcs5.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes128.pkcs5.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes128.pkcs5.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES128 and PKCS#7 padding tests" << std::endl;
}

void test0sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES128 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes128.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES128,ZeroPadding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes128.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes128.zero.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes128.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes128.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES128 and zero padding tests" << std::endl;
}

void test0parallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES128 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes128.zero.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES128,ZeroPadding> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes128.zero.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes128.zero.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes128.zero.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes128.zero.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES128 and zero padding tests" << std::endl;
}

void testANSIX923sequential128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES128 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes128.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES128,ANSIX923> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes128.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes128.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes128.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes128.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES128 and ANSIX923 padding tests" << std::endl;
}

void testANSIX923parallel128() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES128 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes128.ANSIX923.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES128,ANSIX923> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes128.ANSIX923.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes128.ANSIX923.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes128.ANSIX923.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes128.ANSIX923.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES128 and ANSIX923 padding tests" << std::endl;
}

void testISO7816_4sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES192 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes192.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES192,ISO7816_4Padding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes192.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes192.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes192.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes192.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES192 and ISO7816_4 padding tests" << std::endl;
}

void testISO7816_4parallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES192 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes192.ISO7816_4.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES192,ISO7816_4Padding> cbc(IV,3);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes192.ISO7816_4.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes192.ISO7816_4.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes192.ISO7816_4.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes192.ISO7816_4.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES192 and ISO7816_4 padding tests" << std::endl;
}

void testPKCS7sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES192 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes192.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES192,PKCS5Padding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes192.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes192.pkcs5.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes192.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes192.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES192 and PKCS#7 padding tests" << std::endl;
}

void testPKCS7parallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES192 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes192.pkcs5.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES192,PKCS5Padding> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes192.pkcs5.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes192.pkcs5.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes192.pkcs5.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes192.pkcs5.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES192 and PKCS#7 padding tests" << std::endl;
}

void test0sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES192 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes192.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES192,ZeroPadding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes192.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes192.zero.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes192.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes192.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES192 and zero padding tests" << std::endl;
}

void test0parallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES192 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes192.zero.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES192,ZeroPadding> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes192.zero.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes192.zero.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes192.zero.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes192.zero.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES192 and zero padding tests" << std::endl;
}

void testANSIX923sequential192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES192 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes192.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES192,ANSIX923> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes192.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes192.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes192.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes192.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES192 and ANSIX923 padding tests" << std::endl;
}

void testANSIX923parallel192() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES192 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes192.ANSIX923.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES192,ANSIX923> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes192.ANSIX923.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes192.ANSIX923.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes192.ANSIX923.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes192.ANSIX923.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES192 and ANSIX923 padding tests" << std::endl;
}

void testISO7816_4sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES256 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes256.ISO7816_4.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES256,ISO7816_4Padding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes256.ISO7816_4.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes256.ISO7816_4.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes256.ISO7816_4.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes256.ISO7816_4.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES256 and ISO7816_4 padding tests" << std::endl;
}

void testISO7816_4parallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES256 and ISO7816_4 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes256.ISO7816_4.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES256,ISO7816_4Padding> cbc(IV,3);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes256.ISO7816_4.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes256.ISO7816_4.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes256.ISO7816_4.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes256.ISO7816_4.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES256 and ISO7816_4 padding tests" << std::endl;
}

void testPKCS7sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES256 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes256.pkcs5.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES256,PKCS5Padding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes256.pkcs5.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes256.pkcs5.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes256.pkcs5.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes256.pkcs5.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES256 and PKCS#7 padding tests" << std::endl;
}

void testPKCS7parallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES256 and PKCS#7 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes256.pkcs5.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES256,PKCS5Padding> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes256.pkcs5.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes256.pkcs5.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes256.pkcs5.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes256.pkcs5.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES256 and PKCS#7 padding tests" << std::endl;
}

void test0sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES256 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes256.zero.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES256,ZeroPadding> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes256.zero.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes256.zero.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes256.zero.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes256.zero.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES256 and zero padding tests" << std::endl;
}

void test0parallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES256 and zero padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes256.zero.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES256,ZeroPadding> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes256.zero.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes256.zero.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes256.zero.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes256.zero.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES256 and zero padding tests" << std::endl;
}

void testANSIX923sequential256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC sequential with AES256 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes256.ANSIX923.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES256,ANSIX923> cbc(IV);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes256.ANSIX923.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes256.ANSIX923.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes256.ANSIX923.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC sequential decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes256.ANSIX923.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with sequential AES256 and ANSIX923 padding tests" << std::endl;
}

void testANSIX923parallel256() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::cout << "Enter in CBC with parallel AES256 and ANSIX923 padding tests" << std::endl;

  std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
  std::ofstream cbcOutCipher("Makefile.cbc.aes256.ANSIX923.async.cipher", std::ofstream::binary);

  std::cout << "Cipher Makefile" << std::endl;
  CBC<AES256,ANSIX923> cbc(IV,2);
  start = std::chrono::high_resolution_clock::now();
  cbc.cipher(*input, cbcOutCipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutCipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel cipher duration: " << duration.count() << " µs" << std::endl;

  std::cout << "Decipher Makefile.cbc.aes256.ANSIX923.async.cipher" << std::endl;
  input = new std::ifstream("Makefile.cbc.aes256.ANSIX923.async.cipher", std::ifstream::binary);
  std::ofstream cbcOutDecipher("Makefile.cbc.aes256.ANSIX923.async.decipher");
  start = std::chrono::high_resolution_clock::now();
  cbc.decipher(*input, cbcOutDecipher, "foobar    rabooffoobar    raboof");
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
  cbcOutDecipher.close();
  input->close();
  delete input;
  std::cout << "CBC parallel decipher duration: " << duration.count() << " µs" << std::endl;

  std::array<uint8_t,20> resHash = hashSHA1("Makefile.cbc.aes256.ANSIX923.async.decipher");
  anch::ut::assertTrue(initHash == resHash);

  std::cout << "Exit CBC with parallel AES256 and ANSIX923 padding tests" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH AES CBC unit tests")
    .description("Test AnCH CBC ECB library")
    .initialize(beforeAll)
    .add("ISO7816_4-sequential-128", testISO7816_4sequential128)
    .add("ISO7816_4-parallel-128", testISO7816_4parallel128)
    .add("PKCS7-sequential-128", testPKCS7sequential128)
    .add("PKCS7-parallel-128", testPKCS7parallel128)
    .add("0-sequential-128", test0sequential128)
    .add("0-parallel-128", test0parallel128)
    .add("ANSIX923-sequential-128", testANSIX923sequential128)
    .add("ANSIX923-parallel-128", testANSIX923parallel128)
    .add("ISO7816_4-sequential-192", testISO7816_4sequential192)
    .add("ISO7816_4-parallel-192", testISO7816_4parallel192)
    .add("PKCS7-sequential-192", testPKCS7sequential192)
    .add("PKCS7-parallel-192", testPKCS7parallel192)
    .add("0-sequential-192", test0sequential192)
    .add("0-parallel-192", test0parallel192)
    .add("ANSIX923-sequential-192", testANSIX923sequential192)
    .add("ANSIX923-parallel-192", testANSIX923parallel192)
    .add("ISO7816_4-sequential-256", testISO7816_4sequential256)
    .add("ISO7816_4-parallel-256", testISO7816_4parallel256)
    .add("PKCS7-sequential-256", testPKCS7sequential256)
    .add("PKCS7-parallel-256", testPKCS7parallel256)
    .add("0-sequential-256", test0sequential256)
    .add("0-parallel-256", test0parallel256)
    .add("ANSIX923-sequential-256", testANSIX923sequential256)
    .add("ANSIX923-parallel-256", testANSIX923parallel256)
    ;
}
