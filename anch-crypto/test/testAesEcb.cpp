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

using anch::crypto::ECB;
using anch::crypto::AES128;
using anch::crypto::AES192;
using anch::crypto::AES256;
using anch::crypto::SHA1;
using anch::crypto::ISO7816_4Padding;
using anch::crypto::PKCS5Padding;
using anch::crypto::ZeroPadding;
using anch::crypto::ANSIX923;


int
main(int argc, char** argv) {
  std::cout << "Enter in AES - ECB tests" << std::endl;

  bool all = true;
  if(argc == 4) {
    all = false;
  }

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  if(all || (strcmp("ISO7816_4", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("128", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes128.ISO7816_4.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.ISO7816_4.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES128 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ISO7816_4", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("128", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes128.ISO7816_4.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.ISO7816_4.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES128 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("128", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes128.pkcs5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.pkcs5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES128 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("128", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes128.pkcs5.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.pkcs5.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES128 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("128", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes128.zero.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.zero.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES128 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("128", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes128.zero.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.zero.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES128 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("128", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes128.ANSIX923.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.ANSIX923.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES128 and ANSIX923 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("128", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes128.ANSIX923.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.ANSIX923.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES128 and ANSIX923 padding tests" << std::endl;
  }

  if(all || (strcmp("ISO7816_4", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("192", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes192.ISO7816_4.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes192.ISO7816_4.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES192 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ISO7816_4", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("192", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes192.ISO7816_4.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes192.ISO7816_4.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES192 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("192", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes192.pkcs5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes192.pkcs5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES192 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("192", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes192.pkcs5.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes192.pkcs5.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES192 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("192", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes192.zero.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes192.zero.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES192 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("192", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes192.zero.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes192.zero.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES192 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("192", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes192.ANSIX923.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes192.ANSIX923.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES192 and ANSIX923 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("192", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes192.ANSIX923.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes192.ANSIX923.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES192 and ANSIX923 padding tests" << std::endl;
  }

  if(all || (strcmp("ISO7816_4", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("256", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes256.ISO7816_4.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes256.ISO7816_4.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES256 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ISO7816_4", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("256", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes256.ISO7816_4.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes256.ISO7816_4.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES256 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("256", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes256.pkcs5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes256.pkcs5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES256 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("256", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes256.pkcs5.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes256.pkcs5.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES256 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("256", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes256.zero.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes256.zero.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES256 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("256", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes256.zero.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes256.zero.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES256 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0 && strcmp("256", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes256.ANSIX923.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes256.ANSIX923.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with sequential AES256 and ANSIX923 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0 && strcmp("256", argv[3]) == 0)) {
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ecb.aes256.ANSIX923.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes256.ANSIX923.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES256 and ANSIX923 padding tests" << std::endl;
  }

  std::cout << "Exit AES - ECB tests" << std::endl;
  return 0;
}
