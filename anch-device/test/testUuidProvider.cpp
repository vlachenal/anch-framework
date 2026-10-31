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
#include "uuid.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <set>
#include <stdexcept>
#include <thread>
#include <vector>

#include <map>

#include <ifaddrs.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <sys/socket.h>

#include "device/uuidProvider.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::UUID;

/*! MAC address used by tests (independent from network interfaces) */
static const std::string MAC = "01:23:45:67:89:ab";

/*! 100 ns intervals between 1582-10-15 and 1970-01-01 */
static const uint64_t GREGORIAN_OFFSET = 0x01B21DD213814000ULL;

static int64_t
nowMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/*!
 * Decode version 6 timestamp (RFC 9562 section 5.6) as Unix time in milliseconds
 */
static int64_t
v6UnixMs(const UUID& uuid) {
  std::array<uint8_t,16> bytes = uuid.toBytes();
  uint64_t timestamp = 0;
  for(std::size_t i = 0 ; i < 6 ; ++i) {
    timestamp = (timestamp << 8) | bytes[i];
  }
  timestamp = (timestamp << 12) | (static_cast<uint64_t>(bytes[6] & 0x0F) << 8) | bytes[7];
  return static_cast<int64_t>((timestamp - GREGORIAN_OFFSET) / 10000);
}

/*!
 * Decode version 1 timestamp (RFC 9562 section 5.1) as Unix time in milliseconds
 */
static int64_t
v1UnixMs(const UUID& uuid) {
  uint64_t timestamp = (static_cast<uint64_t>(uuid.getHighTime()) << 48) | (static_cast<uint64_t>(uuid.getMidTime()) << 32) | uuid.getLowTime();
  return static_cast<int64_t>((timestamp - GREGORIAN_OFFSET) / 10000);
}

static void
startup() {
  anch::device::registerMACAddressUUIDProvider();
}

void
testV6Format() {
  for(int i = 0 ; i < 1000 ; ++i) {
    int64_t before = nowMs();
    UUID uuid = UUID::generateUUID(UUID::Version::MAC_ADDRESS_R, MAC);
    int64_t after = nowMs();
    std::string str = uuid.toString();
    anch::ut::assertThat(str[14] == '6', "Version 6 expected: {}", str);
    anch::ut::assertThat(str[19] == '8' || str[19] == '9' || str[19] == 'a' || str[19] == 'b', "RFC 9562 variant expected: {}", str);
    anch::ut::assertThat(uuid.getNode() == 0x0123456789abULL, "Node should be MAC address: {}", str);
    int64_t ms = v6UnixMs(uuid);
    anch::ut::assertThat(ms >= before && ms <= after, "Timestamp {} should be between {} and {}: {}", ms, before, after, str);
  }
}

void
testV1Format() {
  for(int i = 0 ; i < 1000 ; ++i) {
    int64_t before = nowMs();
    UUID uuid = UUID::generateUUID(UUID::Version::MAC_ADDRESS, MAC);
    int64_t after = nowMs();
    std::string str = uuid.toString();
    anch::ut::assertThat(str[14] == '1', "Version 1 expected: {}", str);
    anch::ut::assertThat(str[19] == '8' || str[19] == '9' || str[19] == 'a' || str[19] == 'b', "RFC 9562 variant expected: {}", str);
    anch::ut::assertThat(uuid.getNode() == 0x0123456789abULL, "Node should be MAC address: {}", str);
    int64_t ms = v1UnixMs(uuid);
    anch::ut::assertThat(ms >= before && ms <= after, "Timestamp {} should be between {} and {}: {}", ms, before, after, str);
  }
}

void
testV6Order() {
  // Strictly increasing (binary and string order) and unique
  UUID previous = UUID::generateUUID(UUID::Version::MAC_ADDRESS_R, MAC);
  std::set<UUID> all;
  for(int i = 0 ; i < 200000 ; ++i) {
    UUID current = UUID::generateUUID(UUID::Version::MAC_ADDRESS_R, MAC);
    anch::ut::assertThat(previous < current, "{} should be lower than {}", previous.toString(), current.toString());
    anch::ut::assertThat(previous.toString() < current.toString(), "{} should be lower than {} (string)", previous.toString(), current.toString());
    all.insert(current);
    previous = current;
  }
  anch::ut::assertThat(all.size() == 200000, "UUID should be unique ; found {} / 200000", all.size());
}

void
testUniqueThreads() {
  // v1 and v6 from 4 threads with the same node: timestamps are unique in the process
  const int nbThreads = 4;
  const int nb = 50000;
  std::vector<std::vector<UUID>> results(nbThreads);
  std::vector<std::thread> threads;
  for(int t = 0 ; t < nbThreads ; ++t) {
    threads.emplace_back([&results, t, nb]() {
      for(int i = 0 ; i < nb ; ++i) {
	UUID::Version version = (i % 2 == 0) ? UUID::Version::MAC_ADDRESS_R : UUID::Version::MAC_ADDRESS;
	results[static_cast<std::size_t>(t)].push_back(UUID::generateUUID(version, MAC));
      }
    });
  }
  for(std::thread& thread : threads) {
    thread.join();
  }
  std::set<UUID> all;
  for(const std::vector<UUID>& uuids : results) {
    all.insert(uuids.begin(), uuids.end());
  }
  anch::ut::assertThat(all.size() == static_cast<std::size_t>(nbThreads * nb), "UUID should be unique ; found {} / {}", all.size(), nbThreads * nb);
}

void
testDefaultNode() {
  // Expected node: first universally administered address by interface name, then first locally administered one
  std::map<std::string, uint64_t> universal;
  std::map<std::string, uint64_t> local;
  struct ifaddrs* addresses = NULL;
  anch::ut::assertThat(::getifaddrs(&addresses) == 0, "getifaddrs failed");
  for(struct ifaddrs* iter = addresses ; iter != NULL ; iter = iter->ifa_next) {
    if(iter->ifa_addr == NULL || iter->ifa_addr->sa_family != AF_PACKET || (iter->ifa_flags & IFF_LOOPBACK) != 0) {
      continue;
    }
    const struct sockaddr_ll* link = reinterpret_cast<const struct sockaddr_ll*>(iter->ifa_addr);
    uint64_t node = 0;
    for(std::size_t i = 0 ; i < link->sll_halen && i < 8 ; ++i) {
      node = (node << 8) | link->sll_addr[i];
    }
    std::cout << "Interface " << iter->ifa_name << ": " << std::hex << node << std::dec << std::endl;
    if(link->sll_halen != 6 || (link->sll_addr[0] & 0x01) != 0 || node == 0) {
      continue;
    }
    (((link->sll_addr[0] & 0x02) == 0) ? universal : local).emplace(iter->ifa_name, node);
  }
  ::freeifaddrs(addresses);
  UUID v1 = UUID::generateUUID(UUID::Version::MAC_ADDRESS);
  std::cout << "Default node: " << std::hex << v1.getNode() << std::dec << std::endl;
  if(!universal.empty()) {
    anch::ut::assertThat(v1.getNode() == universal.begin()->second, "Node should be {} address", universal.begin()->first);
  } else if(!local.empty()) {
    anch::ut::assertThat(v1.getNode() == local.begin()->second, "Node should be {} address", local.begin()->first);
  } else {
    anch::ut::assertThat((v1.getNode() & 0x010000000000ULL) != 0, "Random node should have multicast bit set");
  }
  // Stable node for v1 and v6
  for(int i = 0 ; i < 100 ; ++i) {
    anch::ut::assertThat(UUID::generateUUID(UUID::Version::MAC_ADDRESS).getNode() == v1.getNode(), "v1 node should be stable");
    anch::ut::assertThat(UUID::generateUUID(UUID::Version::MAC_ADDRESS_R).getNode() == v1.getNode(), "v6 node should be stable");
  }
}

void
testMacAddress() {
  // Valid MAC addresses +
  for(const char* mac : {"01:23:45:67:89:ab", "01-23-45-67-89-ab", "01:23:45:67:89:AB", "01-23-45-67-89-Ab"}) {
    UUID uuid = UUID::generateUUID(UUID::Version::MAC_ADDRESS, mac);
    anch::ut::assertThat(uuid.getNode() == 0x0123456789abULL, "Node for '{}' should be 0123456789ab ; found {:x}", mac, uuid.getNode());
  }
  // Valid MAC addresses -
  // Invalid MAC addresses +
  for(const char* mac : {"12:zz:00:00:00:00", "xyz", "01:23:45:67:89:ab:cd:ef:01", "01:23:45:67:89", "01:23:45:67:89:a",
			 "01:23-45:67:89:ab", "01.23.45.67.89.ab", "0123456789ab", " 01:23:45:67:89:ab", "01:23:45:67:89:ab ",
			 "+1:23:45:67:89:ab", "01::23:45:67:89:a"}) {
    for(UUID::Version version : {UUID::Version::MAC_ADDRESS, UUID::Version::MAC_ADDRESS_R}) {
      try {
	UUID uuid = UUID::generateUUID(version, mac);
	anch::ut::fail("MAC address '" + std::string(mac) + "' should be refused ; found " + uuid.toString());
      } catch(const std::invalid_argument& e) {
	// Expected
      }
    }
    std::cout << "'" << mac << "' refused" << std::endl;
  }
  // Invalid MAC addresses -
}

void
testClockSequence() {
  // 14 bits random clock sequence with RFC 9562 variant: 6 high bits uniform (0x80 to 0xbf with variant)
  const int nb = 64000;
  std::array<int,64> counts{};
  for(int i = 0 ; i < nb ; ++i) {
    UUID uuid = UUID::generateUUID((i % 2 == 0) ? UUID::Version::MAC_ADDRESS : UUID::Version::MAC_ADDRESS_R, MAC);
    anch::ut::assertThat((uuid.getClockSeqHighRes() & 0xC0) == 0x80, "RFC 9562 variant expected: {}", uuid.toString());
    counts[static_cast<std::size_t>(uuid.getClockSeqHighRes() & 0x3F)]++;
  }
  for(std::size_t i = 0 ; i < counts.size() ; ++i) {
    anch::ut::assertThat(counts[i] > nb / 64 / 2 && counts[i] < nb / 64 * 3 / 2, "Clock sequence high value {:x} generated {} times ; expected ~{}", i, counts[i], nb / 64);
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH device UUID provider unit tests")
    .description("Test AnCH version 1 and 6 UUID providers")
    .initialize(startup)
    .add("uuid-v6-format", testV6Format)
    .add("uuid-v1-format", testV1Format)
    .add("uuid-v6-order", testV6Order)
    .add("uuid-v1-v6-unique-threads", testUniqueThreads)
    .add("uuid-default-node", testDefaultNode)
    .add("uuid-mac-address", testMacAddress)
    .add("uuid-clock-sequence", testClockSequence)
    ;
}
