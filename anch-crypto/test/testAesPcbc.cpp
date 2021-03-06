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

using anch::crypto::PCBC;
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
  std::cout << "Enter in AES - PCBC tests" << std::endl;

  bool all = true;
  if(argc == 3) {
    all = false;
  }

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::array<uint8_t, 16> iv = { {
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
    } };

  if(all || (strcmp("ISO7816_4", argv[1]) == 0  && strcmp("128", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES128 and ISO7816_4 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.ISO7816_4.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES128,ISO7816_4Padding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes128.ISO7816_4.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes128.ISO7816_4.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES128 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0  && strcmp("128", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES128 and PKCS#7 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.pkcs5.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES128,PKCS5Padding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes128.pkcs5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes128.pkcs5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES128 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0  && strcmp("128", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES128 and zero padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.zero.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES128,ZeroPadding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes128.zero.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes128.zero.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES128 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0  && strcmp("128", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES128 and ANSIX923 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.ANSIX923.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES128,ANSIX923> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes128.ANSIX923.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes128.ANSIX923.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES128 and ANSIX923 padding tests" << std::endl;
  }

  if(all || (strcmp("ISO7816_4", argv[1]) == 0  && strcmp("192", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES192 and ISO7816_4 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes192.ISO7816_4.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES192,ISO7816_4Padding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes192.ISO7816_4.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes192.ISO7816_4.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES192 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0  && strcmp("192", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES192 and PKCS#7 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes192.pkcs5.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES192,PKCS5Padding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes192.pkcs5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes192.pkcs5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES192 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0  && strcmp("192", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES192 and zero padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes192.zero.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES192,ZeroPadding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes192.zero.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes192.zero.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES192 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0  && strcmp("192", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES192 and ANSIX923 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes192.ANSIX923.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES192,ANSIX923> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes192.ANSIX923.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes192.ANSIX923.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES192 and ANSIX923 padding tests" << std::endl;
  }

  if(all || (strcmp("ISO7816_4", argv[1]) == 0  && strcmp("256", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES256 and ISO7816_4 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes256.ISO7816_4.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES256,ISO7816_4Padding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes256.ISO7816_4.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes256.ISO7816_4.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES256 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0  && strcmp("256", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES256 and PKCS#7 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes256.pkcs5.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES256,PKCS5Padding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes256.pkcs5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes256.pkcs5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES256 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0  && strcmp("256", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES256 and zero padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes256.zero.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES256,ZeroPadding> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes256.zero.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes256.zero.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES256 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0  && strcmp("256", argv[2]) == 0)) {
    std::cout << "Enter in PCBC sequential with AES256 and ANSIX923 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes256.ANSIX923.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES256,ANSIX923> pcbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes256.ANSIX923.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes256.ANSIX923.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with sequential AES256 and ANSIX923 padding tests" << std::endl;
  }

  std::cout << "Exit AES - PCBC tests" << std::endl;
  return 0;
}
