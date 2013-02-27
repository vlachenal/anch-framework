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
  cout << "MAC address UUID: " << mac.toString() << endl;
  Uuid random;
  Uuid::generateUuid(random);
  cout << "Random UUID: " << random.toString() << endl;
  return 0;
}
