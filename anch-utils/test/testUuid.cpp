#include "uuid/uuid.hpp"

#include <iostream>

using anch::uuid::Uuid;
using anch::uuid::Version;

using std::cout;
using std::cerr;
using std::endl;

int
main(void) {
  Uuid mac;
  Uuid::generateUuid(mac, Version::MAC_ADDRESS);
  Uuid random;
  Uuid::generateUuid(random);
  Uuid md5;
  Uuid::generateUuid(md5, Version::MD5_HASH, "Ta mère suce des bites en enfer !");
  cout << "MAC address UUID: " << mac << endl;
  cout << "Random UUID:      " << random << endl;
  cout << "MD5 hash UUID:    " << md5 << endl;
  return 0;
}
