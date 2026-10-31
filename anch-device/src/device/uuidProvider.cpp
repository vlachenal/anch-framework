/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <cstdlib>
#include <bitset>

#include "device/uuidProvider.hpp"
#include "device/network.hpp"
#include "uuid.hpp"

using anch::UUID;
using anch::device::Network;
using anch::device::NetworkInterface;

UUID
generateUUID(uint64_t timestamp, const std::string& macAddress = "") {
  // Timestamp +
  uint32_t lowTime = timestamp & UUID::TIME_LOW_MASK;
  uint16_t midTime = (timestamp >> 32) & UUID::TIME_MID_MASK;
  uint16_t highTime = (timestamp >> 48) & UUID::TIME_HIGH_MASK;
  // Timestamp -

  // Sequence +
  uint16_t clockSeq = UUID::getDistSeq()(UUID::getRandomEngine());
  uint16_t clockSeqLow = clockSeq & UUID::SEQ_LOW_MASK;
  uint16_t clockSeqHighRes = (clockSeq & UUID::SEQ_HIGH_MASK) >> 8;
  clockSeqHighRes |= 0x80;
  // Sequence -

  // MAC address +
  std::string macAddr = macAddress;
  if(macAddr == "") {
    for(auto iter : Network::getInterfaces()) {
      const NetworkInterface iface = iter.second;
      if(!iface.isLocalhost()) {
	macAddr = iface.getMacAddress();
      }
    }
  }
  std::size_t pos = 0;
  std::size_t last = 0;
  std::string macHexStr = "";
  do {
    if(pos != 0) {
      last = pos + 1;
    }
    pos = macAddr.find(':', last);
    macHexStr += macAddr.substr(last, pos - last);
  } while(pos != std::string::npos);
  uint64_t node = std::stoull(macHexStr,0,16);
  // MAC address -

  return UUID(lowTime, midTime, highTime, clockSeqLow, clockSeqHighRes, node, UUID::Version::MAC_ADDRESS);
}

UUID
generateUUIDv1(const std::string& macAddress = "") {
  return generateUUID(UUID::getUtcTimestamp(), macAddress);
}

UUID
generateUUIDv6(const std::string& macAddress = "") {
  uint64_t timestamp = UUID::getUtcTimestamp();
  std::bitset<64> bits(timestamp);
  std::bitset<64> res;
  for(std::size_t i = 0 ; i < 64 ; ++i) {
    res.set(63 - i, bits[i]);
  }
  return generateUUID(res.to_ullong(), macAddress);
}

void
anch::device::registerMACAddressUUIDProvider() {
  UUID::registerProvider(UUID::Version::MAC_ADDRESS, &generateUUIDv1);
  UUID::registerProvider(UUID::Version::MAC_ADDRESS_R, &generateUUIDv6);
}
