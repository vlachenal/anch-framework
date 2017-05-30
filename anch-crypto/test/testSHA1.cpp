#include "crypto/hash/sha1.hpp"
#include "crypto/b2t/base64.hpp"

#include <iostream>
#include <sstream>
#include <random>
#include <iomanip>
#include <chrono>
#include <cstring>

using anch::crypto::SHA1;


int
main(void) {
  std::cout << "Enter in SHA1 test" << std::endl;

  {
    std::string msg = "abc";
    std::cout << "Hash message: " << msg << std::endl;
    SHA1 hash(msg);
    std::ostringstream out;
    out << hash;
    std::string res = out.str();

    std::string sum = "a9993e364706816aba3e25717850c26c9cd0d89d";
    if(res != sum) {
      std::cerr << "Hash are differents:" << std::endl;
      std::cerr << "AnCH result: " << res << std::endl;
      std::cerr << "Instead of:  " << sum << std::endl;
      return 1;

    } else {
      std::cout << "Found value: " << res << std::endl << std::endl;
    }
  }

  {
    std::string msg = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    std::cout << "Hash message: " << msg << std::endl;
    SHA1 hash(msg);
    std::ostringstream out;
    out << hash;
    std::string res = out.str();

    std::string sum = "84983e441c3bd26ebaae4aa1f95129e5e54670f1";
    if(res != sum) {
      std::cerr << "Hash are differents:" << std::endl;
      std::cerr << "AnCH result: " << res << std::endl;
      std::cerr << "Instead of:  " << sum << std::endl;
      return 1;

    } else {
      std::cout << "Found value: " << res << std::endl << std::endl;
    }
  }

  {
    std::ostringstream in;
    for(int i = 0 ; i < 1000000 ; i++) {
      in << 'a';
    }
    std::string msg = in.str();
    std::cout << "Hash message: 'one million of a'" << std::endl;
    SHA1 hash(msg);
    std::ostringstream out;
    out << hash;
    std::string res = out.str();

    std::string sum = "34aa973cd4c4daa4f61eeb2bdbad27316534016f";
    if(res != sum) {
      std::cerr << "Hash are differents:" << std::endl;
      std::cerr << "AnCH result: " << res << std::endl;
      std::cerr << "Instead of:  " << sum << std::endl;
      return 1;

    } else {
      std::cout << "Found value: " << res << std::endl << std::endl;
    }
  }

  {
    std::cout << "Enter in secure password test" << std::endl;
    std::string password("vfdbhjilvbgfvfdjnl"); // 18 characters
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<uint8_t> uniform_dist;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();
    std::ostringstream outSalt;
    for(std::size_t i = 0 ; i < 16 ; ++i) {
      outSalt << static_cast<uint8_t>(uniform_dist(engine));
    }
    std::string salt = outSalt.str(); // 16 characters
    std::string saltPasswd = salt + password; // 34 characters
    for(int i = 0 ; i < 1500 ; ++i) {
      saltPasswd.assign(reinterpret_cast<const char*>(SHA1(saltPasswd).digest().data()), SHA1::getBlockSize()); // 20 characters
    }
    uint8_t saltDigest[36];
    std::memcpy(saltDigest, salt.data(), 16);
    std::cout << "Last salt byte: " << static_cast<uint16_t>(salt[15]) << std::endl;
    std::cout << "Last salt digest byte: " << static_cast<uint16_t>(saltDigest[15]) << std::endl;
    std::memcpy(saltDigest + 16, saltPasswd.data(), 20);
    std::string storedPasswd = anch::crypto::Base64::encode(saltDigest, 36);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    std::cout << "Salt is " << std::hex;
    for(std::size_t i = 0 ; i < 16 ; ++i) {
      std::cout << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(static_cast<const uint8_t>(salt[i]));
    }
    std::cout << std::dec << std::endl;
    std::cout << "We will store: " << storedPasswd << std::endl;
    std::cout << "Password has been encrypted in " << duration.count() << " µs" << std::endl;
  }

  return 0;
}
