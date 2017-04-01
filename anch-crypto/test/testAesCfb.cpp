#include <iostream>
#include <fstream>

#include "crypto/cipher/cfb.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/cipher/aes192.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/hash/sha1.hpp"

using anch::crypto::CFB;
using anch::crypto::AES128;
using anch::crypto::AES192;
using anch::crypto::AES256;
using anch::crypto::SHA1;


int
main(int argc, char** argv) {
  std::cout << "Enter in AES - CFB tests" << std::endl;

  bool all = true;
  if(argc == 3) {
    all = false;
  }

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::array<uint8_t, 16> iv = { {
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
    } };

  if(all || (strcmp("128", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0)) {
    std::cout << "Enter in CFB sequential with AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cfbOutCipher("Makefile.cfb.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CFB<AES128> cfb(iv);
    start = std::chrono::high_resolution_clock::now();
    cfb.cipher(*input, cfbOutCipher, "foobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutCipher.close();
    input->close();
    delete input;
    std::cout << "CFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.cfb.aes128.cipher" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes128.cipher", std::ifstream::binary);
    std::ofstream cfbOutDecipher("Makefile.cfb.aes128.decipher");
    start = std::chrono::high_resolution_clock::now();
    cfb.decipher(*input, cfbOutDecipher, "foobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutDecipher.close();
    input->close();
    delete input;
    std::cout << "CFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cfb.aes128.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes128.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CFB with sequential AES128 tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("128", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0)) {
    std::cout << "Enter in CFB with parallel AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cfbOutCipher("Makefile.cfb.aes128.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CFB<AES128> cfb(iv,2);
    start = std::chrono::high_resolution_clock::now();
    cfb.cipher(*input, cfbOutCipher, "foobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutCipher.close();
    input->close();
    delete input;
    std::cout << "CFB parallel cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.cfb.aes128.async.cipher" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes128.async.cipher", std::ifstream::binary);
    std::ofstream cfbOutDecipher("Makefile.cfb.aes128.async.decipher");
    start = std::chrono::high_resolution_clock::now();
    cfb.decipher(*input, cfbOutDecipher, "foobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutDecipher.close();
    input->close();
    delete input;
    std::cout << "CFB parallel decipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cfb.aes128.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes128.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CFB with parallel AES128 tests" << std::endl;
  }

  if(all || (strcmp("192", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0)) {
    std::cout << "Enter in CFB sequential with AES192 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cfbOutCipher("Makefile.cfb.aes192.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CFB<AES192> cfb(iv);
    start = std::chrono::high_resolution_clock::now();
    cfb.cipher(*input, cfbOutCipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutCipher.close();
    input->close();
    delete input;
    std::cout << "CFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.cfb.aes192.cipher" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes192.cipher", std::ifstream::binary);
    std::ofstream cfbOutDecipher("Makefile.cfb.aes192.decipher");
    start = std::chrono::high_resolution_clock::now();
    cfb.decipher(*input, cfbOutDecipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutDecipher.close();
    input->close();
    delete input;
    std::cout << "CFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cfb.aes192.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes192.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CFB with sequential AES192 tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("192", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0)) {
    std::cout << "Enter in CFB with parallel AES192 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cfbOutCipher("Makefile.cfb.aes192.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CFB<AES192> cfb(iv,2);
    start = std::chrono::high_resolution_clock::now();
    cfb.cipher(*input, cfbOutCipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutCipher.close();
    input->close();
    delete input;
    std::cout << "CFB parallel cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.cfb.aes192.async.cipher" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes192.async.cipher", std::ifstream::binary);
    std::ofstream cfbOutDecipher("Makefile.cfb.aes192.async.decipher");
    start = std::chrono::high_resolution_clock::now();
    cfb.decipher(*input, cfbOutDecipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutDecipher.close();
    input->close();
    delete input;
    std::cout << "CFB parallel decipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cfb.aes192.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes192.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CFB with parallel AES192 tests" << std::endl;
  }

  if(all || (strcmp("256", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0)) {
    std::cout << "Enter in CFB sequential with AES256 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cfbOutCipher("Makefile.cfb.aes256.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CFB<AES256> cfb(iv);
    start = std::chrono::high_resolution_clock::now();
    cfb.cipher(*input, cfbOutCipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutCipher.close();
    input->close();
    delete input;
    std::cout << "CFB sequential cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.cfb.aes256.cipher" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes256.cipher", std::ifstream::binary);
    std::ofstream cfbOutDecipher("Makefile.cfb.aes256.decipher");
    start = std::chrono::high_resolution_clock::now();
    cfb.decipher(*input, cfbOutDecipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutDecipher.close();
    input->close();
    delete input;
    std::cout << "CFB sequential decipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cfb.aes256.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes256.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CFB with sequential AES256 tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("256", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0)) {
    std::cout << "Enter in CFB with parallel AES256 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cfbOutCipher("Makefile.cfb.aes256.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CFB<AES256> cfb(iv,2);
    start = std::chrono::high_resolution_clock::now();
    cfb.cipher(*input, cfbOutCipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutCipher.close();
    input->close();
    delete input;
    std::cout << "CFB parallel cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.cfb.aes256.async.cipher" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes256.async.cipher", std::ifstream::binary);
    std::ofstream cfbOutDecipher("Makefile.cfb.aes256.async.decipher");
    start = std::chrono::high_resolution_clock::now();
    cfb.decipher(*input, cfbOutDecipher, "foobar    rabooffoobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cfbOutDecipher.close();
    input->close();
    delete input;
    std::cout << "CFB parallel decipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cfb.aes256.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes256.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CFB with parallel AES256 tests" << std::endl;
  }

  std::cout << "Exit AES - CFB tests" << std::endl;
  return 0;
}
