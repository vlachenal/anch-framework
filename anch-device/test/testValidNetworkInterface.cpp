#include <iostream>

#include "device/network.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using anch::device::Network;
using anch::device::NetworkInterface;
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
    const NetworkInterface* interface = Network::getInterface(ifName);
    if(interface == NULL) {
      cerr << "Interface not found" << endl;
      return 2;
    }
    cout << interface->getName() << " has broadcast address "
	 << interface->getBroadcastAddress() << endl;

  } catch(const DeviceException& e) {
    cerr << e.what() << endl;
    return 1;
  }
  return 0;
}
