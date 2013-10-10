#include "crypto/hash/md5.hpp"
#include "crypto/hmac.hpp"
#include "crypto/hash/sha1.hpp"
#include "crypto/hash/sha224_256.hpp"
#include "crypto/hash/sha384_512.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

using anch::crypto::MD5;
using anch::crypto::SHA1;
using anch::crypto::SHA224;
using anch::crypto::SHA256;
using anch::crypto::SHA384;
using anch::crypto::SHA512;
using anch::crypto::HMAC;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ostringstream;


int
main(void) {
  cout << "Enter in HMAC tests" << endl;

  {
    cout << "Enter in HMAC-MD5 test" << endl;
    ostringstream out;
    out << HMAC<MD5>("key", "The quick brown fox jumps over the lazy dog");
    string res = out.str();
    if(res != "80070713463e7749b90c2dc24911e275") {
      cerr << "Found      " << res << endl;
      cerr << "Instead of 80070713463e7749b90c2dc24911e275" << endl;
      return 1;
    } else {
      cout << "Found HMAC-MD5 " << res << endl;
    }
    cout << "Exit HMAC-MD5 test" << endl;
  }

  {
    cout << "Enter in HMAC-SHA1 test" << endl;
    ostringstream out;
    out << HMAC<SHA1>("key", "The quick brown fox jumps over the lazy dog");
    string res = out.str();
    if(res != "de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9") {
      cerr << "Found      " << res << endl;
      cerr << "Instead of de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9" << endl;
      return 1;
    } else {
      cout << "Found HMAC-SHA1 " << res << endl;
    }
    cout << "Exit HMAC-SHA1 test" << endl;
  }

  {
    cout << "Enter in HMAC-SHA224 test" << endl;
    ostringstream out;
    out << HMAC<SHA224>("key", "The quick brown fox jumps over the lazy dog");
    string res = out.str();
    if(res != "88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69") {
      cerr << "Found      " << res << endl;
      cerr << "Instead of 88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69" << endl;
      return 1;
    } else {
      cout << "Found HMAC-SHA224 " << res << endl;
    }
    cout << "Exit HMAC-SHA224 test" << endl;
  }

  {
    cout << "Enter in HMAC-SHA256 test" << endl;
    ostringstream out;
    out << HMAC<SHA256>("key", "The quick brown fox jumps over the lazy dog");
    string res = out.str();
    if(res != "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8") {
      cerr << "Found      " << res << endl;
      cerr << "Instead of f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8" << endl;
      return 1;
    } else {
      cout << "Found HMAC-SHA256 " << res << endl;
    }
    cout << "Exit HMAC-SHA256 test" << endl;
  }

  {
    cout << "Enter in HMAC-SHA384 test" << endl;
    ostringstream out;
    out << HMAC<SHA384>("key", "The quick brown fox jumps over the lazy dog");
    string res = out.str();
    if(res != "d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237") {
      cerr << "Found      " << res << endl;
      cerr << "Instead of d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237" << endl;
      return 1;
    } else {
      cout << "Found HMAC-SHA384 " << res << endl;
    }
    cout << "Exit HMAC-SHA384 test" << endl;
  }

  {
    cout << "Enter in HMAC-SHA512 test" << endl;
    ostringstream out;
    out << HMAC<SHA512>("key", "The quick brown fox jumps over the lazy dog");
    string res = out.str();
    if(res != "b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a") {
      cerr << "Found      " << res << endl;
      cerr << "Instead of b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a" << endl;
      return 1;
    } else {
      cout << "Found HMAC-SHA512 " << res << endl;
    }
    cout << "Exit HMAC-SHA512 test" << endl;
  }

  cout << "Exit HMAC tests" << endl;
  return 0;
}
