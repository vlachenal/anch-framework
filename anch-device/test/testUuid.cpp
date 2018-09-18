#include "uuid.hpp"

#include <iostream>

#include "device/uuidProvider.hpp"

int
main(void) {
  anch::device::registerMACAddressUUIDProvider();

  anch::UUID mac = anch::UUID::generateUUID(anch::UUID::Version::MAC_ADDRESS);
  std::cout << "MAC address UUID: " << mac << std::endl;
  std::string macStr = static_cast<std::string>(mac);
  std::cout << "UUID MAC address UUID conversion:   " << macStr << std::endl;
  std::cout << "UUID MAC address string conversion: " << anch::UUID(macStr) << std::endl;

  return 0;
}
