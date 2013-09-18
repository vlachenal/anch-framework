#include "crypto/hash/md5.hpp"
#include "crypto/hmac.hpp"
#include "crypto/hash/sha1.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

using anch::crypto::MD5;
using anch::crypto::SHA1;
using anch::crypto::HMAC;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ostringstream;


int
main(void) {
  cout << "Enter in HMAC tests" << endl;

  cout << "Enter in HMAC-MD5 test" << endl;
  ostringstream out1;
  out1 << HMAC<MD5>("key", "The quick brown fox jumps over the lazy dog");
  string res1 = out1.str();
  if(res1 != "80070713463e7749b90c2dc24911e275") {
    cerr << "Found " << res1 << " instead of 80070713463e7749b90c2dc24911e275" << endl;
    return 1;
  } else {
    cout << "Found HMAC-MD5 " << res1 << endl;
  }
  cout << "Exit HMAC-MD5 test" << endl;

  cout << "Enter in HMAC-SHA1 test" << endl;
  ostringstream out2;
  out2 << HMAC<SHA1>("key", "The quick brown fox jumps over the lazy dog");
  string res2 = out2.str();
  if(res2 != "de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9") {
    cerr << "Found " << res2 << " instead of de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9" << endl;
    return 1;
  } else {
    cout << "Found HMAC-SHA1 " << res2 << endl;
  }
  cout << "Exit HMAC-SHA1 test" << endl;

  cout << "Exit HMAC tests" << endl;
  return 0;
}
