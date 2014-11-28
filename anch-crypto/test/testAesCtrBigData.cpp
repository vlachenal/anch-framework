#include <iostream>
#include <fstream>

#include "crypto/cipher/ctr.hpp"
#include "crypto/cipher/aes128.hpp"
#include "crypto/hash/sha1.hpp"

using anch::crypto::CTR;
using anch::crypto::AES128;
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

    std::ifstream* input = new std::ifstream("/home/winz/toto.avi", std::ifstream::binary);
    std::ofstream ctrOutCipher("Makefile.ctr.aes128.bigdata.cipher", std::ofstream::binary);

    std::cout << "Cipher big data" << std::endl;
    CTR<AES128> ctr(iv);
    start = std::chrono::high_resolution_clock::now();
    ctr.cipher(*input, ctrOutCipher, "foobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    ctrOutCipher.close();
    input->close();
    delete input;
    std::cout << "CTR sequential cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Exit CTR with sequential AES128 tests" << std::endl;
  }

  std::cout << std::endl;

  {
    std::cout << "Enter in CTR with parallel AES128 tests" << std::endl;

    std::ifstream* input = new std::ifstream("/home/winz/toto.avi", std::ifstream::binary);
    std::ofstream ctrOutCipher("Makefile.ctr.aes128.bigdata.async.cipher", std::ofstream::binary);

    std::cout << "Cipher bigdata" << std::endl;
    CTR<AES128> ctr(iv,2);
    start = std::chrono::high_resolution_clock::now();
    ctr.cipher(*input, ctrOutCipher, "foobar    raboof");
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    ctrOutCipher.close();
    input->close();
    delete input;
    std::cout << "CTR parallel cipher duration: " << duration.count() << " µs" << std::endl;

    std::cout << "Exit CTR with parallel AES128 tests" << std::endl;
  }

  std::cout << "Exit AES - CTR tests" << std::endl;
  return 0;
}
