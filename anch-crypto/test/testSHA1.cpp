#include "crypto/hash/sha1.hpp"

#include <iostream>
#include <sstream>

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

  return 0;
}
