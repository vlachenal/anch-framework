/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <cstdlib>
#include <atomic>
#include <array>
#include <map>
#include <string>
#include <stdexcept>

#include <ifaddrs.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <sys/socket.h>

#include "device/uuidProvider.hpp"
#include "uuid.hpp"

using anch::UUID;

/*!
 * Last timestamp used by version 1 and 6 UUID (100 ns intervals since 1582-10-15)
 */
static std::atomic<uint64_t> LAST_TIMESTAMP(0);

/*!
 * Get next version 1 / 6 timestamp: current time, or last timestamp + 1 when clock has not advanced (several UUID in
 * the same 100 ns interval) or has gone backward. Timestamps are strictly increasing in the process, including between
 * threads, so UUID generated with the same node are unique (RFC 9562 section 6.2).
 *
 * \return the timestamp
 */
static uint64_t
nextTimestamp() {
  uint64_t now = UUID::getUtcTimestamp();
  uint64_t previous = LAST_TIMESTAMP.load(std::memory_order_relaxed);
  uint64_t next;
  do {
    next = (now > previous) ? now : previous + 1;
  } while(!LAST_TIMESTAMP.compare_exchange_weak(previous, next, std::memory_order_relaxed));
  return next;
}

/*! Multicast bit of the node (least significant bit of the first byte) */
static const uint64_t NODE_MULTICAST_BIT = 0x010000000000ULL;

/*!
 * Select default node (RFC 9562 sections 5.1 and 6.10) from network interfaces hardware addresses (\c getifaddrs ,
 * \c AF_PACKET : every interface is listed, with or without IP address).\n
 * Loopback interfaces, addresses which are not 6 bytes long, null, broadcast and multicast addresses are ignored.
 * A universally administered address (IEEE) is preferred to a locally administered one (virtual interfaces, randomized
 * Wi-Fi addresses) ; in each category, the first interface by name is selected so that the node is stable.\n
 * When no address is available, a random node with multicast bit set is used.
 *
 * \return the node
 */
static uint64_t
selectDefaultNode() {
  std::map<std::string, uint64_t> universal;
  std::map<std::string, uint64_t> local;
  struct ifaddrs* addresses = NULL;
  if(::getifaddrs(&addresses) == 0) {
    for(struct ifaddrs* iter = addresses ; iter != NULL ; iter = iter->ifa_next) {
      if(iter->ifa_addr == NULL || iter->ifa_addr->sa_family != AF_PACKET || (iter->ifa_flags & IFF_LOOPBACK) != 0) {
	continue;
      }
      const struct sockaddr_ll* link = reinterpret_cast<const struct sockaddr_ll*>(iter->ifa_addr);
      if(link->sll_halen != 6 || (link->sll_addr[0] & 0x01) != 0) { // not a 6 bytes unicast address
	continue;
      }
      uint64_t node = 0;
      for(std::size_t i = 0 ; i < 6 ; ++i) {
	node = (node << 8) | link->sll_addr[i];
      }
      if(node == 0) {
	continue;
      }
      std::map<std::string, uint64_t>& category = ((link->sll_addr[0] & 0x02) == 0) ? universal : local;
      category.emplace(iter->ifa_name, node);
    }
    ::freeifaddrs(addresses);
  }
  if(!universal.empty()) {
    return universal.begin()->second;
  }
  if(!local.empty()) {
    return local.begin()->second;
  }
  // Random node with multicast bit set (cannot conflict with a hardware address)
  return UUID::random().getNode() | NODE_MULTICAST_BIT;
}

/*!
 * Get default node: selected once by process (see \c selectDefaultNode )
 *
 * \return the node
 */
static uint64_t
getDefaultNode() {
  static const uint64_t node = selectDefaultNode();
  return node;
}

/*!
 * Hexadecimal digit value
 *
 * \param c the character
 *
 * \return the value, \c -1 when \c c is not an hexadecimal digit
 */
static int
hexDigit(char c) {
  if(c >= '0' && c <= '9') {
    return c - '0';
  }
  if(c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
  if(c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }
  return -1;
}

/*!
 * Get node from MAC address.\n
 * The MAC address has to be 6 pairs of hexadecimal digits (lower or upper case) separated by \c ':' or \c '-'
 * (the same separator everywhere), for example \c 01:23:45:67:89:ab
 *
 * \param macAddress the MAC address (default node when empty)
 *
 * \return the node
 *
 * \throw std::invalid_argument invalid MAC address
 */
static uint64_t
getNode(const std::string& macAddress) {
  if(macAddress.empty()) {
    return getDefaultNode();
  }
  if(macAddress.size() != 17 || (macAddress[2] != ':' && macAddress[2] != '-')) {
    throw std::invalid_argument("Invalid MAC address '" + macAddress + "': 6 hexadecimal pairs separated by ':' or '-' expected");
  }
  char separator = macAddress[2];
  uint64_t node = 0;
  for(std::size_t i = 0 ; i < 6 ; ++i) {
    std::size_t pos = i * 3;
    int high = hexDigit(macAddress[pos]);
    int low = hexDigit(macAddress[pos + 1]);
    if(high < 0 || low < 0 || (i < 5 && macAddress[pos + 2] != separator)) {
      throw std::invalid_argument("Invalid MAC address '" + macAddress + "': 6 hexadecimal pairs separated by ':' or '-' expected");
    }
    node = (node << 8) | static_cast<uint64_t>((high << 4) | low);
  }
  return node;
}

/*!
 * Get random clock sequence (14 bits) with RFC 9562 variant.\n
 * Random bits come from \c UUID::random() (cryptographically secure source drawn in one call, see \c anch-uuid ),
 * which is much cheaper than \c std::random_device .
 *
 * \param clockSeqLow the clock sequence low part to set
 * \param clockSeqHighRes the clock sequence high part with variant to set
 */
static void
randomClockSequence(uint16_t& clockSeqLow, uint16_t& clockSeqHighRes) {
  std::array<uint8_t,16> bytes = UUID::random().toBytes(); // byte 8 has RFC 9562 variant (10xx), byte 9 is random
  clockSeqHighRes = static_cast<uint16_t>(0x80 | (bytes[8] & 0x3F));
  clockSeqLow = bytes[9];
}

/*!
 * Generate version 1 UUID (RFC 9562 section 5.1): timestamp low, mid and high parts
 *
 * \param macAddress the MAC address (default node when empty)
 *
 * \return the UUID
 */
static UUID
generateUUIDv1(const std::string& macAddress = "") {
  uint64_t timestamp = nextTimestamp();
  uint16_t clockSeqLow;
  uint16_t clockSeqHighRes;
  randomClockSequence(clockSeqLow, clockSeqHighRes);
  return UUID(static_cast<uint32_t>(timestamp & UUID::TIME_LOW_MASK),
	      static_cast<uint16_t>((timestamp >> 32) & UUID::TIME_MID_MASK),
	      static_cast<uint16_t>((timestamp >> 48) & UUID::TIME_HIGH_MASK),
	      clockSeqLow,
	      clockSeqHighRes,
	      getNode(macAddress),
	      UUID::Version::MAC_ADDRESS);
}

/*!
 * Generate version 6 UUID (RFC 9562 section 5.6): version 1 timestamp reordered from most to least significant bits
 * (32 high bits, 16 middle bits, version and 12 low bits) so that UUID are sorted by time
 *
 * \param macAddress the MAC address (default node when empty)
 *
 * \return the UUID
 */
static UUID
generateUUIDv6(const std::string& macAddress = "") {
  uint64_t timestamp = nextTimestamp();
  uint16_t clockSeqLow;
  uint16_t clockSeqHighRes;
  randomClockSequence(clockSeqLow, clockSeqHighRes);
  return UUID(static_cast<uint32_t>((timestamp >> 28) & UUID::TIME_LOW_MASK),
	      static_cast<uint16_t>((timestamp >> 12) & UUID::TIME_MID_MASK),
	      static_cast<uint16_t>(timestamp & UUID::TIME_HIGH_MASK),
	      clockSeqLow,
	      clockSeqHighRes,
	      getNode(macAddress),
	      UUID::Version::MAC_ADDRESS_R);
}

void
anch::device::registerMACAddressUUIDProvider() {
  UUID::registerProvider(UUID::Version::MAC_ADDRESS, &generateUUIDv1);
  UUID::registerProvider(UUID::Version::MAC_ADDRESS_R, &generateUUIDv6);
}
