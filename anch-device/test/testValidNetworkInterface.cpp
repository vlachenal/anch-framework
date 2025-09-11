#include <iostream>

#include "device/network.hpp"

using anch::device::Network;
using anch::device::NetworkInterface;
using anch::device::DeviceException;


/*!
 * Test valid network device.\n
 *
 * This test has to succeed.
 */
int
main(void) {
  try {
    auto ifaces = Network::getInterfaces();
    NetworkInterface* interface = NULL;
    for(auto iter = ifaces.begin() ; iter != ifaces.end() ; ++iter) {
      if(iter->second.isLocalhost()) {
	continue;
      }
      if(iter->second.getIpAddress() == "") {
	std::cout << iter->second.getName() << " is not connected" << std::endl;
	continue;
      }
      interface = &iter->second;
      break;
    }
    if(interface == NULL) {
      std::cerr << "Interface not found" << std::endl;
      return 2;
    }
    std::cout << interface->getName() << " has broadcast address "
	      << interface->getBroadcastAddress() << std::endl;

  } catch(const DeviceException& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}
