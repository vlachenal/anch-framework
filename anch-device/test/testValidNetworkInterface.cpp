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
 * This test has to succeed.
 */
int
main(void) {
  string ifName = "wlan0";
  try {
    Network::initialize(ifName);
    cout << Network::getInterfaceName() << " has broadcast address "
	 << Network::getBroadcastAddress() << endl;

  } catch(const DeviceException& e) {
    cerr << e.what() << endl;
    return 1;
  }
  return 0;
}
