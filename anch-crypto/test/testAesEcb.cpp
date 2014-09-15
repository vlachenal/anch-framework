#include <iostream>
#include <fstream>

#include "crypto/cipher/ecb.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/hash/sha1.hpp"

#include "crypto/padding/zeroPadding.hpp"
#include "crypto/padding/pkcs7Padding.hpp"
#include "crypto/padding/iso7816_4Padding.hpp"
#include "crypto/padding/ansiX923.hpp"

using anch::crypto::ECB;
using anch::crypto::AES128;
using anch::crypto::SHA1;
using anch::crypto::ISO7816_4Padding;
using anch::crypto::PKCS7Padding;
using anch::crypto::ZeroPadding;
using anch::crypto::ANSIX923;


int
main(void) {
  std::cout << "Enter in AES - ECB tests" << std::endl;

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  {
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

  {
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

  {
    std::cout << "Enter in ECB sequential with AES128 and PKCS#7 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ecbOutCipher("Makefile.ecb.aes128.pkcs7.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    ECB<AES128,PKCS7Padding> ecb;
    start = std::chrono::high_resolution_clock::now();
    ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    ecbOutCipher.close();
    input->close();
    delete input;
    std::cout << "ECB sequential cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.ecb.aes128.pkcs7.cipher" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.pkcs7.cipher", std::ifstream::binary);
    std::ofstream ecbOutDecipher("Makefile.ecb.aes128.pkcs7.decipher");
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

    std::cout << "Compute Makefile.ecb.aes128.pkcs7.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.pkcs7.decipher", std::ifstream::binary);
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

  {
    std::cout << "Enter in ECB with parallel AES128 and PKCS#7 padding tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ecbOutCipher("Makefile.ecb.aes128.pkcs7.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    ECB<AES128,PKCS7Padding> ecb(2);
    start = std::chrono::high_resolution_clock::now();
    ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    ecbOutCipher.close();
    input->close();
    delete input;
    std::cout << "ECB parallel cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Decipher Makefile.ecb.aes128.pkcs7.async.cipher" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.pkcs7.async.cipher", std::ifstream::binary);
    std::ofstream ecbOutDecipher("Makefile.ecb.aes128.pkcs7.async.decipher");
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

    std::cout << "Compute Makefile.ecb.aes128.pkcs7.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.pkcs7.async.decipher", std::ifstream::binary);
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

  {
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

  {
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

  {
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

  {
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

  std::cout << "Exit AES - ECB tests" << std::endl;
  return 0;
}
