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
#include "uuid/uuid.hpp"

#include <sstream>
#include <ctime>
#include <chrono>
#include <limits>
#include <random>
#include <functional>

#include "device/network.hpp"
#include "crypto/hash/md5.hpp"
#include "crypto/hash/sha1.hpp"


using anch::uuid::Uuid;
using anch::uuid::Version;
using anch::device::Network;
using anch::device::NetworkInterface;
using anch::crypto::MD5;
using anch::crypto::SHA1;
using std::string;
using std::array;
using std::numeric_limits;
using std::uniform_int_distribution;


// Static members initialization +
const uint64_t Uuid::UTC_OFFSET = 0x01B21DD213814000;
// Static members initialization -
uint64_t TIME_LOW_MASK = 0x00000000FFFFFFFF;
uint64_t TIME_MID_MASK = 0x000000000000FFFF;
uint64_t TIME_HIGH_MASK = 0x0000000000000FFF;
uint32_t SEQ_LOW_MASK = 0x00FF;
uint32_t SEQ_HIGH_MASK = 0x3F00;

std::mt19937 engine;
uniform_int_distribution<uint16_t> distSeq(0, SEQ_HIGH_MASK);
uniform_int_distribution<uint16_t> dist16;
uniform_int_distribution<uint32_t> dist32;
uniform_int_distribution<uint64_t> dist64(0, 0xFFFFFFFFFFFF); // Max 12 hexadecimal digits


// Constructors +
/*!
 * \ref Uuid default constructor
 */
Uuid::Uuid() {
  // Intialize some fields ?
}

/*!
 * \ref Uuid copy constructor
 *
 * \param uuid The \ref Uuid to copy
 */
Uuid::Uuid(const Uuid& uuid): _lowTime(uuid._lowTime),
			      _midTime(uuid._midTime),
			      _highTime(uuid._highTime),
			      _clockSeqLow(uuid._clockSeqLow),
			      _clockSeqHighRes(uuid._clockSeqHighRes),
			      _node(uuid._node) {
  // Nothing to do
}
// Constructors -


// Methods +
/*!
 * Generate a new UUID
 *
 * \param uuid The UUID to set
 * \param version The UUID algorithm version to use (default to MAC based algorithm)
 * \param data The data to process (used to SHA1 and MD5 algorithms)
 */
void
  Uuid::generateUuid(Uuid& uuid, Version version, const string& data) {
  // Initialize random seed +
  std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
  auto epoch = now.time_since_epoch();
  int64_t timestamp = (std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count());
  engine.seed(timestamp);
  // Initialize random seed -

  uuid._version = version;

  switch(version) {
  case Version::MAC_ADDRESS:
    generateUuidVersion1(uuid, data);
    break;
  case Version::DCE_SECURITY:
    // TODO implements this algorithm version
    break;
  case Version::MD5_HASH:
    generateUuidVersion3(uuid,data);
    break;
  case Version::RANDOM:
    generateUuidVersion4(uuid);
    break;
  case Version::SHA1_HASH:
    generateUuidVersion5(uuid,data);
    break;
  default:
    // Nothing to do
    break;
  }
}

/*!
 * Generate a new UUID with version 1 (MAC address based) algorithm
 *
 * \param uuid The UUID to set
 * \param macAddress The MAC address to use. If empty, the first MAC address
 *                   available will be choosen
 */
void
Uuid::generateUuidVersion1(Uuid& uuid, const string& macAddress) {

  // Timestamp +
  uint64_t timestamp = getUtcTimestamp();
  uuid._lowTime = timestamp & TIME_LOW_MASK;
  uuid._midTime = (timestamp >> 32) & TIME_MID_MASK;
  uuid._highTime = (timestamp >> 48) & TIME_HIGH_MASK;
  // Timestamp -

  // Sequence +
  uint16_t clockSeq = distSeq(engine);
  uuid._clockSeqLow = clockSeq & SEQ_LOW_MASK;
  uuid._clockSeqHighRes = (clockSeq & SEQ_HIGH_MASK) >> 8;
  uuid._clockSeqHighRes |= 0x80;
  // Sequence -

  // MAC address +
  string macAddr = macAddress;
  if(macAddr == "") {
    for(auto iter : Network::getInterfaces()) {
      const NetworkInterface iface = iter.second;
      if(!iface.isLocalhost()) {
	macAddr = iface.getMacAddress();
      }
    }
  }
  size_t pos = 0;
  size_t last = 0;
  string macHexStr = "";
  do {
    if(pos != 0) {
      last = pos + 1;
    }
    pos = macAddr.find(':', last);
    macHexStr += macAddr.substr(last, pos - last);
  } while(pos != string::npos);
  uuid._node = std::stoull(macHexStr,0,16);
  // MAC address -
}

/*!
 * Generate a new UUID with version 3 (MD5 based) algorithm
 *
 * \param uuid The UUID to set
 * \param data The data to process
 */
void
Uuid::generateUuidVersion3(Uuid& uuid, const string& data) {
  MD5 hash(data);
  const array<uint8_t,16>& digest = hash.digest();

  // Timestamp +
  uuid._lowTime = digest[0]
    + (static_cast<uint32_t>(digest[1]) << 8)
    + (static_cast<uint32_t>(digest[2]) << 16)
    + (static_cast<uint32_t>(digest[3]) << 24);
  uuid._midTime = digest[4]
    + (static_cast<uint16_t>(digest[5]) << 8);
  uuid._highTime = digest[6]
    + (static_cast<uint16_t>(digest[7]) << 8);
  uuid._highTime = uuid._highTime & TIME_HIGH_MASK;
  // Timestamp -

  // Sequence +
  uuid._clockSeqLow = digest[9];
  uuid._clockSeqHighRes = digest[8];
  uuid._clockSeqHighRes |= 0x80;
  // Sequence -

  // Node +
  uuid._node = digest[10]
    + (static_cast<uint64_t>(digest[11]) << 8)
    + (static_cast<uint64_t>(digest[12]) << 16)
    + (static_cast<uint64_t>(digest[13]) << 24)
    + (static_cast<uint64_t>(digest[14]) << 32)
    + (static_cast<uint64_t>(digest[15]) << 40);
  // Node -
}

/*!
 * Generate a new UUID with version 1 (MAC address based) algorithm
 *
 * \param uuid The UUID to set
 */
void
Uuid::generateUuidVersion4(Uuid& uuid) {
  // Timestamp +
  uuid._lowTime = dist32(engine);
  uuid._midTime = dist16(engine);
  uuid._highTime = dist16(engine) & TIME_HIGH_MASK;
  // Timestamp -

  // Sequence +
  uint16_t clockSeq = distSeq(engine);
  uuid._clockSeqLow = clockSeq & SEQ_LOW_MASK;
  uuid._clockSeqHighRes = (clockSeq & SEQ_HIGH_MASK) >> 8;
  uuid._clockSeqHighRes |= 0x80;
  // Sequence -

  // Node +
  uuid._node = dist64(engine);
  // Node -
}

/*!
 * Generate a new UUID with version 3 (MD5 based) algorithm
 *
 * \param uuid The UUID to set
 * \param data The data to process
 */
void
Uuid::generateUuidVersion5(Uuid& uuid, const string& data) {
  SHA1 hash(data);
  const array<uint8_t,20>& digest = hash.digest();

  // Timestamp +
  uuid._lowTime = digest[0]
    + (static_cast<uint32_t>(digest[1]) << 8)
    + (static_cast<uint32_t>(digest[2]) << 16)
    + (static_cast<uint32_t>(digest[3]) << 24);
  uuid._midTime = digest[4]
    + (static_cast<uint16_t>(digest[5]) << 8);
  uuid._highTime = digest[6]
    + (static_cast<uint16_t>(digest[7]) << 8);
  uuid._highTime = uuid._highTime & TIME_HIGH_MASK;
  // Timestamp -

  // Sequence +
  uuid._clockSeqLow = digest[9];
  uuid._clockSeqHighRes = digest[8];
  uuid._clockSeqHighRes |= 0x80;
  // Sequence -

  // Node +
  uuid._node = digest[10]
    + (static_cast<uint64_t>(digest[11]) << 8)
    + (static_cast<uint64_t>(digest[12]) << 16)
    + (static_cast<uint64_t>(digest[13]) << 24)
    + (static_cast<uint64_t>(digest[14]) << 32)
    + (static_cast<uint64_t>(digest[15]) << 40);
  // Node -
}

/*!
 * Get UTC timestamp
 *
 * \return The UTC timestamp
 */
uint64_t
Uuid::getUtcTimestamp() {
  std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
  auto epoch = now.time_since_epoch();
  int64_t timestamp = (std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count()) / 100;
  return UTC_OFFSET + (uint64_t)timestamp;
}

/*!
 * Convert \ref Uuid to std::string
 *
 * \return The string representation of \ref Uuid
 */
std::string
Uuid::toString() const {
  std::ostringstream out;
  out << *this;
  return out.str();
}
// Methods -
