#include <iostream>
#include <fstream>

#include "crypto/cipher/ecb.hpp"
#include "crypto/cipher/cbc.hpp"
#include "crypto/cipher/pcbc.hpp"
#include "crypto/cipher/cfb.hpp"
#include "crypto/cipher/ofb.hpp"
#include "crypto/cipher/ctr.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/hash/sha1.hpp"

using anch::crypto::ECB;
using anch::crypto::CBC;
using anch::crypto::PCBC;
using anch::crypto::CFB;
using anch::crypto::OFB;
using anch::crypto::CTR;
using anch::crypto::AES128;
using anch::crypto::SHA1;


int
main(void) {
  std::cout << "Enter in block cipher mode of operation tests" << std::endl;

  {
    std::cout << "Enter in ECB with AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ecbOutCipher("Makefile.ecb.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    ECB<AES128> ecb;
    ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
    ecbOutCipher.close();
    input->close();
    delete input;

    std::cout << "Decipher Makefile.ecb.aes128.cipher" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.cipher", std::ifstream::binary);
    std::ofstream ecbOutDecipher("Makefile.ecb.aes128.decipher");
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

    std::cout << "Compute Makefile.ecb.aes128.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.decipher", std::ifstream::binary);
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

  {
    std::cout << "Enter in ECB with parallel AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ecbOutCipher("Makefile.ecb.aes128.async.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    ECB<AES128> ecb(4);
    ecb.cipher(*input, ecbOutCipher, "foobar    raboof");
    ecbOutCipher.close();
    input->close();
    delete input;

    std::cout << "Decipher Makefile.ecb.aes128.async.cipher" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.async.cipher", std::ifstream::binary);
    std::ofstream ecbOutDecipher("Makefile.ecb.aes128.async.decipher");
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

    std::cout << "Compute Makefile.ecb.aes128.async.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.ecb.aes128.async.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit ECB with parallel AES128 tests" << std::endl;
  }

  {
    std::cout << "Enter in CBC with AES128 tests" << std::endl;

    std::array<uint8_t, 16> iv = { {
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
      } };

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cbcOutCipher("Makefile.cbc.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CBC<AES128> cbc(iv);
    cbc.cipher(*input, cbcOutCipher, "foobar    raboof");
    cbcOutCipher.close();
    input->close();
    delete input;

    std::cout << "Decipher Makefile.cbc.aes128.cipher" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.cipher", std::ifstream::binary);
    std::ofstream cbcOutDecipher("Makefile.cbc.aes128.decipher");
    cbc.decipher(*input, cbcOutDecipher, "foobar    raboof");
    cbcOutDecipher.close();
    input->close();
    delete input;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.cbc.aes128.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.cbc.aes128.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit CBC with AES128 tests" << std::endl;
  }

  {
    std::cout << "Enter in PCBC with AES128 tests" << std::endl;

    std::array<uint8_t, 16> iv = { {
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
      } };

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream pcbcOutCipher("Makefile.pcbc.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    PCBC<AES128> pcbc(iv);
    pcbc.cipher(*input, pcbcOutCipher, "foobar    raboof");
    pcbcOutCipher.close();
    input->close();
    delete input;

    std::cout << "Decipher Makefile.pcbc.aes128.cipher" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes128.cipher", std::ifstream::binary);
    std::ofstream pcbcOutDecipher("Makefile.pcbc.aes128.decipher");
    pcbc.decipher(*input, pcbcOutDecipher, "foobar    raboof");
    pcbcOutDecipher.close();
    input->close();
    delete input;

    std::cout << "Compute Makefile hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile", std::ifstream::binary);
    SHA1 hash(*input);
    std::array<uint8_t,20> initHash = hash.digest();
    input->close();
    delete input;

    std::cout << "Compute Makefile.pcbc.aes128.decipher hash using SHA1" << std::endl;
    input = new std::ifstream("Makefile.pcbc.aes128.decipher", std::ifstream::binary);
    SHA1 cipherHash(*input);
    std::array<uint8_t,20> resHash = cipherHash.digest();
    input->close();
    delete input;

    if(initHash != resHash) {
      std::cerr << "Makefiles are differents" << std::endl;
      return 1;
    }

    std::cout << "Exit PCBC with AES128 tests" << std::endl;
  }

  {
    std::cout << "Enter in CFB with AES128 tests" << std::endl;

    std::array<uint8_t, 16> iv = { {
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
      } };

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream cfbOutCipher("Makefile.cfb.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CFB<AES128> cfb(iv);
    cfb.cipher(*input, cfbOutCipher, "foobar    raboof");
    cfbOutCipher.close();
    input->close();
    delete input;

    std::cout << "Decipher Makefile.cfb.aes128.cipher" << std::endl;
    input = new std::ifstream("Makefile.cfb.aes128.cipher", std::ifstream::binary);
    std::ofstream cfbOutDecipher("Makefile.cfb.aes128.decipher");
    cfb.decipher(*input, cfbOutDecipher, "foobar    raboof");
    cfbOutDecipher.close();
    input->close();
    delete input;

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

    std::cout << "Exit CFB with AES128 tests" << std::endl;
  }

  {
    std::cout << "Enter in OFB with AES128 tests" << std::endl;

    std::array<uint8_t, 16> iv = { {
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
      } };

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ofbOutCipher("Makefile.ofb.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    OFB<AES128> ofb(iv);
    ofb.cipher(*input, ofbOutCipher, "foobar    raboof");
    ofbOutCipher.close();
    input->close();
    delete input;

    std::cout << "Decipher Makefile.ofb.aes128.cipher" << std::endl;
    input = new std::ifstream("Makefile.ofb.aes128.cipher", std::ifstream::binary);
    std::ofstream ofbOutDecipher("Makefile.ofb.aes128.decipher");
    ofb.decipher(*input, ofbOutDecipher, "foobar    raboof");
    ofbOutDecipher.close();
    input->close();
    delete input;

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

    std::cout << "Exit OFB with AES128 tests" << std::endl;
  }

  {
    std::cout << "Enter in CTR with AES128 tests" << std::endl;

    std::array<uint8_t, 16> iv = { {
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'
      } };

    std::ifstream* input = new std::ifstream("Makefile", std::ifstream::binary);
    std::ofstream ctrOutCipher("Makefile.ctr.aes128.cipher", std::ofstream::binary);

    std::cout << "Cipher Makefile" << std::endl;
    CTR<AES128> ctr(iv);
    ctr.cipher(*input, ctrOutCipher, "foobar    raboof");
    ctrOutCipher.close();
    input->close();
    delete input;

    std::cout << "Decipher Makefile.ctr.aes128.cipher" << std::endl;
    input = new std::ifstream("Makefile.ctr.aes128.cipher", std::ifstream::binary);
    std::ofstream ctrOutDecipher("Makefile.ctr.aes128.decipher");
    ctr.decipher(*input, ctrOutDecipher, "foobar    raboof");
    ctrOutDecipher.close();
    input->close();
    delete input;

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

    std::cout << "Exit CTR with AES128 tests" << std::endl;
  }

  std::cout << "Exit block cipher mode of operation tests" << std::endl;
  return 0;
}
