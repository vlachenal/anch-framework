#include "crypto/b2t/base64.hpp"

#include <fstream>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;

using anch::crypto::Base64;

int
main(void) {
  cout << "Enter in base 64 test cases" << endl;

  cout << "Encode empty string" << endl;
  string res = Base64::encode(string(""));
  if(res != "") {
    cerr << "Found " << res << " instead of empty string" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Encode f" << endl;
  res = Base64::encode(string("f"));
  if(res != "Zg==") {
    cerr << "Found " << res << " instead of Zg==" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Encode fo" << endl;
  res = Base64::encode(string("fo"));
  if(res != "Zm8=") {
    cerr << "Found " << res << " instead of Zm8=" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Encode foo" << endl;
  res = Base64::encode(string("foo"));
  if(res != "Zm9v") {
    cerr << "Found " << res << " instead of Zm9v" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Encode foob" << endl;
  res = Base64::encode(string("foob"));
  if(res != "Zm9vYg==") {
    cerr << "Found " << res << " instead of Zm9vYg==" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Encode fooba" << endl;
  res = Base64::encode(string("fooba"));
  if(res != "Zm9vYmE=") {
    cerr << "Found " << res << " instead of Zm9vYmE=" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Encode foobar" << endl;
  res = Base64::encode(string("foobar"));
  if(res != "Zm9vYmFy") {
    cerr << "Found " << res << " instead of Zm9vYmFy" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  {
    cout << "Encode Makefile" << endl;
    ifstream file("Makefile");
    Base64::encode(file, cout);
    cout << endl;
    cout << "You can check result manually using base64 command" << endl;
    file.close();
  }

  cout << "Decode empty string" << endl;
  res = Base64::decode(string(""));
  if(res != "") {
    cerr << "Found " << res << " instead of empty string" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Decode Zg==" << endl;
  res = Base64::decode(string("Zg=="));
  if(res != "f") {
    cerr << "Found " << res << " instead of f" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Decode Zm8=" << endl;
  res = Base64::decode(string("Zm8="));
  if(res != "fo") {
    cerr << "Found " << res << " instead of fo" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Decode Zm9v" << endl;
  res = Base64::decode(string("Zm9v"));
  if(res != "foo") {
    cerr << "Found " << res << " instead of foo" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Decode Zm9vYg==" << endl;
  res = Base64::decode(string("Zm9vYg=="));
  if(res != "foob") {
    cerr << "Found " << res << " instead of foob" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Decode Zm9vYmE=" << endl;
  res = Base64::decode(string("Zm9vYmE="));
  if(res != "fooba") {
    cerr << "Found " << res << " instead of fooba" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  cout << "Decode Zm9vYmFy" << endl;
  std::ostringstream out;
  Base64::decode(string("Zm9vYmFy"), out);
  res = out.str();
  if(res != "foobar") {
    cerr << "Found " << res << " instead of foobar" << endl;
    return 1;
  }
  cout << "Found " << res << endl;

  return 0;
}
