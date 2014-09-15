#include <iostream>
#include <fstream>

#include "crypto/cipher/ofb.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/hash/sha1.hpp"

using anch::crypto::OFB;
using anch::crypto::AES128;
using anch::crypto::SHA1;


int
main(void) {
  std::cout << "Enter in AES - OFB tests" << std::endl;

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::array<uint8_t, 16> iv = { {
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
    } };

  {
    std::cout << "Enter in OFB sequential with AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ofbOutCipher("Makefile.ofb.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    OFB<AES128> ofb(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ofb.aes128.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ofb.aes128.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit OFB with sequential AES128 tests" << std::endl;
  }

  std::cout << "Exit AES - OFB tests" << std::endl;
  return 0;
}
