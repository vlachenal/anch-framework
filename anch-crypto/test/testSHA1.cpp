#include "crypto/hash/sha1.hpp"
#include "crypto/b2t/base64.hpp"

#include <iostream>
#include <sstream>
#include <random>
#include <iomanip>
#include <chrono>
#include <cstring>

using anch::crypto::SHA1;
using std::cout;
using std::cerr;
using std::endl;
using std::string;


int
main(void) {
  cout << "Enter in SHA1 test" << endl;

  {
    string msg = "abc";
    cout << "Hash message: " << msg << endl;
    SHA1 hash(msg);
    std::ostringstream out;
    out << hash;
    string res = out.str();

    string sum = "a9993e364706816aba3e25717850c26c9cd0d89d";
    if(res != sum) {
      cerr << "Hash are differents:" << endl;
      cerr << "AnCH result: " << res << endl;
      cerr << "Instead of:  " << sum << endl;
      return 1;

    } else {
      cout << "Found value: " << res << endl << endl;
    }
  }

  {
    string msg = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    cout << "Hash message: " << msg << endl;
    SHA1 hash(msg);
    std::ostringstream out;
    out << hash;
    string res = out.str();

    string sum = "84983e441c3bd26ebaae4aa1f95129e5e54670f1";
    if(res != sum) {
      cerr << "Hash are differents:" << endl;
      cerr << "AnCH result: " << res << endl;
      cerr << "Instead of:  " << sum << endl;
      return 1;

    } else {
      cout << "Found value: " << res << endl << endl;
    }
  }

  {
    std::ostringstream in;
    for(int i = 0 ; i < 1000000 ; i++) {
      in << 'a';
    }
    string msg = in.str();
    cout << "Hash message: 'one million of a'" << endl;
    SHA1 hash(msg);
    std::ostringstream out;
    out << hash;
    string res = out.str();

    string sum = "34aa973cd4c4daa4f61eeb2bdbad27316534016f";
    if(res != sum) {
      cerr << "Hash are differents:" << endl;
      cerr << "AnCH result: " << res << endl;
      cerr << "Instead of:  " << sum << endl;
      return 1;

    } else {
      cout << "Found value: " << res << endl << endl;
    }
  }

  {
    cout << "Enter in secure password test" << endl;
    string password("vfdbhjilvbgfvfdjnl");
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<uint8_t> uniform_dist;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();
    std::ostringstream outSalt;
    //uint8_t salt[16];
    for(int i = 0 ; i < 16 ; ++i) {
      //salt[i] = uniform_dist(engine);
      outSalt << static_cast<uint8_t>(uniform_dist(engine));
    }
    std::string salt = outSalt.str();
    // std::size_t len = password.size() + 16;
    // uint8_t* saltPasswd = new uint8_t[len];
    // std::memcpy(saltPasswd, salt, 16);
    // std::memcpy(saltPasswd + 16, password.data(), password.size());
    std::string saltPasswd = salt + password;
    //SHA1 digester;
    for(int i = 0 ; i < 1500 ; ++i) {
      //saltPasswd = SHA1(saltPasswd).digest().data();
      saltPasswd.assign(reinterpret_cast<const char*>(SHA1(saltPasswd).digest().data()), SHA1::getBlockSize());
      //std::memcpy(saltPasswd, SHA1(saltPasswd, len).digest().data(), 20);
      //std::memcpy(saltPasswd, digester.digest(saltPasswd, len).data(), 20);
      //len = 20;
    }
    uint8_t saltDigest[36];
    std::memcpy(saltDigest, salt.data(), 16);
    std::memcpy(saltDigest + 16, saltPasswd.data(), 20);
    string storedPasswd = anch::crypto::Base64::encode(saltDigest, 36);
    //delete[] saltPasswd;
    end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    cout << "Salt is " << std::hex;
    for(int i = 0 ; i < 16 ; ++i) {
      cout << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(static_cast<const uint8_t>(salt[i]));
    }
    cout << std::dec << endl;
    cout << "We will store: " << storedPasswd << endl;
    cout << "Password has been encrypted in " << duration.count() << " µs" << endl;
  }

  return 0;
}
