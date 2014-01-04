#include <iostream>
#include <fstream>

#include "crypto/cipher/ecb.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/hash/sha1.hpp"

using anch::crypto::ECB;
using anch::crypto::AES128;
using anch::crypto::SHA1;


int
main(void) {
  std::cout << "Enter in block cipher mode of operation tests" << std::endl;

  {
    std::cout << "Enter in ECB with AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ecbOutCipher("Makefile.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    ECB<AES128> ecb;
    ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
    ecbOutCipher.close();
    input->close();
    delete input;

    std::cout << "Decipher Makefile.aes128.cipher" << std::endl;
    input = new std::ifstream("Makefile.aes128.cipher", std::ifstream::binary);
    std::ofstream ecbOutDecipher("Makefile.aes128.decipher");
    ecb.decipher(*input, ecbOutDecipher, "foobar    raboof");
    ecbOutDecipher.close();
    input->close();
    delete input;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.aes128.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.aes128.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with AES128 tests" << std::endl;
  }

  std::cout << "Exit block cipher mode of operation tests" << std::endl;
  return 0;
}
