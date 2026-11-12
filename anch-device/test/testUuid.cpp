/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2018 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "uuid.hpp"

#include <iostream>

#include "device/uuidProvider.hpp"

int
main(void) {
  anch::device::registerMACAddressUUIDProvider();

  {
    anch::UUID mac = anch::UUID::generateUUID(anch::UUID::Version::MAC_ADDRESS);
    std::cout << "MAC address UUID: " << mac << std::endl;
    std::string macStr = static_cast<std::string>(mac);
    std::cout << "UUID MAC address UUID conversion:   " << macStr << std::endl;
    std::cout << "UUID MAC address string conversion: " << anch::UUID(macStr) << std::endl;
  }

  {
    anch::UUID mac = anch::UUID::generateUUID(anch::UUID::Version::MAC_ADDRESS_R);
    std::cout << "MAC address UUID: " << mac << std::endl;
    std::string macStr = static_cast<std::string>(mac);
    std::cout << "UUID MAC address UUID conversion:   " << macStr << std::endl;
    std::cout << "UUID MAC address string conversion: " << anch::UUID(macStr) << std::endl;
  }

  return 0;
}
