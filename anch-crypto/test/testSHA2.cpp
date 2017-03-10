#include "crypto/hash/sha224.hpp"
#include "crypto/hash/sha256.hpp"
#include "crypto/hash/sha384.hpp"
#include "crypto/hash/sha512.hpp"

#include <iostream>
#include <sstream>

using anch::crypto::SHA224;
using anch::crypto::SHA256;
using anch::crypto::SHA384;
using anch::crypto::SHA512;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

int
main(int argc, char** argv) {

  bool all = true;
  if(argc == 2) {
    all = false;
  }

  cout << "Enter in SHA2 tests" << endl;

  if(all || strcmp("224", argv[1]) == 0) {
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

  if(all || strcmp("256", argv[1]) == 0) {
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

  if(all || strcmp("384", argv[1]) == 0) {
    string msg = "The quick brown fox jumps over the lazy dog";
    cout << "Hash SHA384 message: " << msg << endl;
    SHA384 hash(msg);
    std::ostringstream out;
    out << hash;
    string res = out.str();

    string sum = "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1";
    if(res != sum) {
      cerr << "Hash are differents:" << endl;
      cerr << "AnCH result: " << res << endl;
      cerr << "Instead of:  " << sum << endl;
      return 1;

    } else {
      cout << "Found value: " << res << endl << endl;
    }
  }

  if(all || strcmp("512", argv[1]) == 0) {
    string msg = "The quick brown fox jumps over the lazy dog";
    cout << "Hash SHA512 message: " << msg << endl;
    SHA512 hash(msg);
    std::ostringstream out;
    out << hash;
    string res = out.str();

    string sum = "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6";
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
