#include "crypto/hash/sha224_256.hpp"

#include <iostream>
#include <sstream>

using anch::crypto::SHA224;
using anch::crypto::SHA256;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

int
main(void) {
  cout << "Enter in SHA2 tests" << endl;

  {
    string msg = "The quick brown fox jumps over the lazy dog";
    cout << "Hash SHA224 message: " << msg << endl;
    SHA224 hash(msg);
    std::ostringstream out;
    out << hash;
    string res = out.str();

    string sum = "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525";
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
    string msg = "The quick brown fox jumps over the lazy dog";
    cout << "Hash SHA256 message: " << msg << endl;
    SHA256 hash(msg);
    std::ostringstream out;
    out << hash;
    string res = out.str();

    string sum = "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592";
    if(res != sum) {
      cerr << "Hash are differents:" << endl;
      cerr << "AnCH result: " << res << endl;
      cerr << "Instead of:  " << sum << endl;
      return 1;

    } else {
      cout << "Found value: " << res << endl << endl;
    }
  }

  cout << "Exit SHA2 tests" << endl;
  return 0;
}
