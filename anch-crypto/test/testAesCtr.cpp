#include <iostream>
#include <fstream>

#include "crypto/cipher/ctr.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/hash/sha1.hpp"

using anch::crypto::CTR;
using anch::crypto::AES128;
using anch::crypto::AES256;
using anch::crypto::SHA1;


int
main(void) {
  std::cout << "Enter in AES - CTR tests" << std::endl;

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::array<uint8_t, 16> iv = { {
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
    } };

  {
    std::cout << "Enter in CTR sequential with AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ctrOutCipher("Makefile.ctr.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CTR<AES128> ctr(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ctr.aes128.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ctr.aes128.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CTR with sequential AES128 tests" << std::endl;
  }

  std::cout << std::endl;

  {
    std::cout << "Enter in CTR with parallel AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ctrOutCipher("Makefile.ctr.aes128.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CTR<AES128> ctr(iv,2);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ctr.aes128.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ctr.aes128.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CTR with parallel AES128 tests" << std::endl;
  }

  std::cout << std::endl;

  {
    std::cout << "Enter in CTR sequential with AES256 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ctrOutCipher("Makefile.ctr.aes256.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CTR<AES256> ctr(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ctr.aes256.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ctr.aes256.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CTR with sequential AES256 tests" << std::endl;
  }

  std::cout << std::endl;

  {
    std::cout << "Enter in CTR with parallel AES256 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ctrOutCipher("Makefile.ctr.aes256.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CTR<AES256> ctr(iv,2);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.ctr.aes256.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ctr.aes256.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CTR with parallel AES256 tests" << std::endl;
  }

  std::cout << "Exit AES - CTR tests" << std::endl;
  return 0;
}
