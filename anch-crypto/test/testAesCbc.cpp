#include <iostream>
#include <fstream>

#include "crypto/cipher/cbc.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/hash/sha1.hpp"

#include "crypto/padding/zeroPadding.hpp"
#include "crypto/padding/pkcs5Padding.hpp"
#include "crypto/padding/iso7816_4Padding.hpp"
#include "crypto/padding/ansiX923.hpp"

using anch::crypto::CBC;
using anch::crypto::AES128;
using anch::crypto::SHA1;
using anch::crypto::ISO7816_4Padding;
using anch::crypto::PKCS5Padding;
using anch::crypto::ZeroPadding;
using anch::crypto::ANSIX923;


int
main(int argc, char** argv) {
  std::cout << "Enter in AES - CBC tests" << std::endl;

  bool all = true;
  if(argc == 3) {
    all = false;
  }

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  std::array<uint8_t, 16> iv = { {
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
    } };

  if(all || (strcmp("ISO7816_4", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0)) {
    std::cout << "Enter in CBC sequential with AES128 and ISO7816_4 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.ISO7816_4.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128,ISO7816_4Padding> cbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.ISO7816_4.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.ISO7816_4.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with sequential AES128 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ISO7816_4", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0)) {
    std::cout << "Enter in CBC with parallel AES128 and ISO7816_4 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.ISO7816_4.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128,ISO7816_4Padding> cbc(iv,3);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.ISO7816_4.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.ISO7816_4.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with parallel AES128 and ISO7816_4 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0)) {
    std::cout << "Enter in CBC sequential with AES128 and PKCS#7 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.pkcs5.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128,PKCS5Padding> cbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.pkcs5.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.pkcs5.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with sequential AES128 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("PKCS#7", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0)) {
    std::cout << "Enter in CBC with parallel AES128 and PKCS#7 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.pkcs5.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128,PKCS5Padding> cbc(iv,2);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.pkcs5.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.pkcs5.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with parallel AES128 and PKCS#7 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0)) {
    std::cout << "Enter in CBC sequential with AES128 and zero padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.zero.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128,ZeroPadding> cbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.zero.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.zero.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with sequential AES128 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("zero", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0)) {
    std::cout << "Enter in CBC with parallel AES128 and zero padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.zero.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128,ZeroPadding> cbc(iv,2);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.zero.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.zero.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with parallel AES128 and zero padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0 && strcmp("sequential", argv[2]) == 0)) {
    std::cout << "Enter in CBC sequential with AES128 and ANSIX923 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.ANSIX923.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128,ANSIX923> cbc(iv);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.ANSIX923.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.ANSIX923.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with sequential AES128 and ANSIX923 padding tests" << std::endl;
  }

  std::cout << std::endl;

  if(all || (strcmp("ANSIX923", argv[1]) == 0 && strcmp("parallel", argv[2]) == 0)) {
    std::cout << "Enter in CBC with parallel AES128 and ANSIX923 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.ANSIX923.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128,ANSIX923> cbc(iv,2);
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

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.ANSIX923.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.ANSIX923.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with parallel AES128 and ANSIX923 padding tests" << std::endl;
  }

  std::cout << "Exit AES - CBC tests" << std::endl;
  return 0;
}
