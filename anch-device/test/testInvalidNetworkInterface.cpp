#include <iostream>

#include "device/network.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using anch::device::Network;
using anch::device::DeviceException;


/**
 * Test valid network device.<br>
 *
 * This test has to fail.
 */
int
main(void) {
  string ifName = "toto";
  try {
    Network::initialize(ifName);
    cerr << Network::getInterfaceName() << " has broadcast address "
	 << Network::getBroadcastAddress() << endl;

  } catch(const DeviceException& e) {
    cout << e.what() << endl;
    return 0;
  }
  return 1;
}
