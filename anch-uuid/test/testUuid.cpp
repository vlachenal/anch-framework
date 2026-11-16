/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2013 Vincent Lachenal

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
#include <iomanip>
#include <sstream>
#include <chrono>
#include <list>
#include <cstring>
#include <set>
#include <type_traits>
#include <unordered_set>
#include <array>
#include <vector>
#include <atomic>
#include <thread>
#include <stdexcept>

#include "ut/assert.hpp"
#include "ut/unit.hpp"
#include "ut/threadErrors.hpp"

using anch::UUID;

void
testUUIDPerf() {
  std::cout << "Enter in UUID performance test" << std::endl;
  UUID::random(); // Register random UUID for performance mesures => register random UUID provider and initialize seed
  std::list<UUID> uuids;
  double totalTime = 0;
  for(std::size_t i = 0 ; i < 1000 ; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    UUID random = UUID::random();
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dur = stop - start;
    totalTime += dur.count();
    uuids.push_back(random);
  }
  std::cout << "Average generation time: " << totalTime / 1000 << "s" << std::endl;
  totalTime = 0;
  std::list<std::string> strUuids;
  for(UUID& uuid : uuids) {
    auto start = std::chrono::high_resolution_clock::now();
    std::string randomStr = static_cast<std::string>(uuid);
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dur = stop - start;
    totalTime += dur.count();
    strUuids.push_back(randomStr);
  }
  std::cout << "Average string from UUID convertion time: " << totalTime / 1000 << "s" << std::endl;
  totalTime = 0;
  for(std::string uuid : strUuids) {
    auto start = std::chrono::high_resolution_clock::now();
    [[maybe_unused]] UUID plop = UUID(uuid);
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dur = stop - start;
    totalTime += dur.count();
  }
  std::cout << "Average UUID from string convertion time: " << totalTime / 1000 << "s" << std::endl;

}

void
testUUIDCollision() {
  std::set<std::string> uuids;
  for(std::size_t i = 0 ; i < 1000000 ; ++i) {
    std::string random = UUID::random().toString();
    if(uuids.find(random) != uuids.end()) {
      std::cerr << "UUID " << random << " has already been generated" << std::endl;
    } else {
      uuids.insert(random);
    }
  }
  std::cout << uuids.size() << " different UUIDs have been generated" << std::endl;

}

void
tesUUIDSingle() {
  UUID random = UUID::random();
  std::cout << "Random UUID:                   " << random << std::endl;
  std::string randomStr = static_cast<std::string>(random);
  std::cout << "UUID random conversion:        " << randomStr << std::endl;
  std::cout << "UUID random string conversion: " << UUID(randomStr) << std::endl;
}

void
tesUUIDRadomTime() {
  UUID random = UUID::randomTime();
  std::cout << "Random time UUID:              " << random << std::endl;
  std::string randomStr = static_cast<std::string>(random);
  std::cout << "UUID random conversion:        " << randomStr << std::endl;
  std::cout << "UUID random string conversion: " << UUID(randomStr) << std::endl;
}

/*!
 * Reference string conversion (former \c UUID::toString implementation)
 */
std::string
referenceToString(const UUID& uuid) {
  std::ostringstream out;
  out << std::hex
      << std::setfill('0') << std::setw(8) << uuid.getLowTime() << '-'
      << std::setfill('0') << std::setw(4) << uuid.getMidTime() << '-'
      << std::setfill('0') << std::setw(1) << static_cast<int>(uuid.getVersion())
      << std::setfill('0') << std::setw(3) << uuid.getHighTime() << '-'
      << std::setfill('0') << std::setw(2) << uuid.getClockSeqHighRes()
      << std::setfill('0') << std::setw(2) << uuid.getClockSeqLow() << '-'
      << std::setfill('0') << std::setw(12) << uuid.getNode();
  return out.str();
}

void
checkToString(const UUID& uuid) {
  std::string expected = referenceToString(uuid);
  std::string str = uuid.toString();
  anch::ut::assertThat(str == expected, "toString: expected {} ; found {}", expected, str);
  char chars[UUID::STRING_SIZE + 1];
  chars[UUID::STRING_SIZE] = 'X'; // check that nothing is written after UUID
  char* end = uuid.toChars(chars);
  anch::ut::assertThat(end == chars + UUID::STRING_SIZE, "toChars should write {} characters ; found {}", UUID::STRING_SIZE, end - chars);
  anch::ut::assertThat(std::string(chars, UUID::STRING_SIZE) == expected, "toChars: expected {} ; found {}", expected, std::string(chars, UUID::STRING_SIZE));
  anch::ut::assertThat(chars[UUID::STRING_SIZE] == 'X', "toChars has written after UUID");
  anch::ut::assertThat(UUID(str).toString() == str, "Round trip failed for {}", str);
}

void
testUUIDToString() {
  checkToString(UUID());
  checkToString(UUID("00000000-0000-0000-0000-000000000000"));
  checkToString(UUID("deb94ebc-be28-4899-981a-29199b7a487d"));
  checkToString(UUID("ffffffff-ffff-4fff-bfff-ffffffffffff"));
  checkToString(UUID("01234567-89ab-1def-8123-456789abcdef"));
  for(int i = 0 ; i < 10000 ; ++i) {
    checkToString(UUID::random());
    checkToString(UUID::randomTime());
  }
}

void
testUUIDParse() {
  UUID uuid("DEB94EBC-BE28-4899-981A-29199B7A487D"); // uppercase is accepted
  anch::ut::assertThat(uuid.toString() == "deb94ebc-be28-4899-981a-29199b7a487d", "Uppercase parsing: found {}", uuid.toString());
  anch::ut::assertThat(uuid.getVersion() == UUID::Version::RANDOM, "Version should be 4");
  anch::ut::assertThat(uuid.getNode() == 0x29199b7a487dULL, "Node should be 29199b7a487d ; found {:x}", uuid.getNode());
}

void
testUUIDParseKO() {
  const std::string valid = "deb94ebc-be28-4899-981a-29199b7a487d";
  for(const char* str : {"",
                         "deb94ebc-be28-4899-981a-29199b7a487",    // too short
                         "deb94ebc-be28-4899-981a-29199b7a487d0",  // too long
                         "1234zzzz-e89b-42d3-a456-426614174000",   // not hexadecimal (was accepted)
                         "deb94ebc_be28-4899-981a-29199b7a487d",   // '_' instead of '-'
                         "deb94ebcbbe28-4899-981a-29199b7a487d",   // missing '-'
                         " eb94ebc-be28-4899-981a-29199b7a487d",   // leading space (skipped by former std::stoul)
                         "+eb94ebc-be28-4899-981a-29199b7a487d",   // sign
                         "deb94ebc-be28-4899-981a-29199b7a487g"}) {
    UUID uuid(valid);
    try {
      uuid.parse(str);
      anch::ut::fail("Parsing '" + std::string(str) + "' should have failed");
    } catch(const std::invalid_argument& e) {
      std::cout << e.what() << std::endl;
    }
    anch::ut::assertThat(uuid.toString() == valid, "UUID should not be modified by failed parsing ; found {}", uuid.toString());
  }
}

void
testUUIDBytes() {
  UUID uuid("01234567-89ab-4def-9123-456789abcdef");
  std::array<uint8_t,16> expected = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0x4d, 0xef, 0x91, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
  anch::ut::assertThat(uuid.toBytes() == expected, "toBytes does not match RFC 9562 big-endian layout");
  UUID res = UUID::fromBytes(expected);
  anch::ut::assertThat(res == uuid, "fromBytes: expected {} ; found {}", uuid.toString(), res.toString());
  anch::ut::assertThat(res.getVersion() == UUID::Version::RANDOM, "fromBytes: version should be 4");
  for(int i = 0 ; i < 10000 ; ++i) { // round trip
    UUID random = UUID::random();
    anch::ut::assertThat(UUID::fromBytes(random.toBytes()) == random, "Round trip failed for {}", random.toString());
  }
  // Namespaces +
  anch::ut::assertThat(UUID::namespaceDNS().toString() == "6ba7b810-9dad-11d1-80b4-00c04fd430c8", "Bad DNS namespace");
  anch::ut::assertThat(UUID::namespaceURL().toString() == "6ba7b811-9dad-11d1-80b4-00c04fd430c8", "Bad URL namespace");
  anch::ut::assertThat(UUID::namespaceOID().toString() == "6ba7b812-9dad-11d1-80b4-00c04fd430c8", "Bad OID namespace");
  anch::ut::assertThat(UUID::namespaceX500().toString() == "6ba7b814-9dad-11d1-80b4-00c04fd430c8", "Bad X500 namespace");
  // Namespaces -
}

void
testUUIDClockSeq() {
  // 14 bits clock sequence: first byte of 4th group is 0x80 to 0xbf (variant + 6 bits), each value ~1/64
  const int nb = 200000;
  std::array<int,256> counts{};
  for(int i = 0 ; i < nb ; ++i) {
    std::array<uint8_t,16> bytes = UUID::random().toBytes();
    counts[bytes[8]]++;
  }
  for(int b = 0 ; b < 256 ; ++b) {
    if(b < 0x80 || b > 0xbf) {
      anch::ut::assertThat(counts[static_cast<std::size_t>(b)] == 0, "Byte {:x} should never be generated (variant)", b);
    } else {
      int count = counts[static_cast<std::size_t>(b)];
      anch::ut::assertThat(count > nb / 64 / 2 && count < nb / 64 * 3 / 2, "Byte {:x} generated {} times ; expected ~{}", b, count, nb / 64);
    }
  }
}

void
testUUIDProviderConcurrency() {
  // Register providers while generating UUID from other threads (data race checked with ThreadSanitizer)
  std::atomic_bool stop(false);
  anch::ut::ThreadErrors errors; // failed assertions are raised again in test thread
  std::vector<std::thread> threads;
  threads.emplace_back([&stop]() {
    for(int i = 0 ; i < 2000 ; ++i) {
      UUID::registerProvider(UUID::Version::EXP, [](const std::string&) { return UUID::random(); });
    }
    stop = true;
  });
  for(int t = 0 ; t < 3 ; ++t) {
    threads.emplace_back(errors.wrap([&stop]() {
      while(!stop) {
	UUID uuid = UUID::generateUUID(UUID::Version::RANDOM, "");
	anch::ut::assertThat(uuid.getVersion() == UUID::Version::RANDOM, "Version should be 4");
      }
    }));
  }
  for(std::thread& thread : threads) {
    thread.join();
  }
  errors.rethrow();
  anch::ut::assertThat(UUID::generateUUID(UUID::Version::EXP, "").getVersion() == UUID::Version::RANDOM, "Registered provider should be used");
}

void
testUUIDRandomFormat() {
  for(int i = 0 ; i < 10000 ; ++i) {
    // Version 4: version and variant bits, other bits random
    std::array<uint8_t,16> bytes = UUID::random().toBytes();
    anch::ut::assertThat((bytes[6] >> 4) == 4, "Version 4 expected");
    anch::ut::assertThat((bytes[8] & 0xC0) == 0x80, "RFC 9562 variant expected");
    // Version 7: Unix timestamp in milliseconds on 48 first bits
    auto before = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    bytes = UUID::randomTime().toBytes();
    auto after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int64_t timestamp = 0;
    for(std::size_t b = 0 ; b < 6 ; ++b) {
      timestamp = (timestamp << 8) | bytes[b];
    }
    anch::ut::assertThat((bytes[6] >> 4) == 7, "Version 7 expected");
    anch::ut::assertThat((bytes[8] & 0xC0) == 0x80, "RFC 9562 variant expected");
    anch::ut::assertThat(timestamp >= before && timestamp <= after, "Timestamp {} should be between {} and {}", timestamp, before, after);
  }
}

void
testUUIDUtcTimestamp() {
  // 0x01B21DD213814000 100 ns intervals between 1582-10-15 and 1970-01-01 (Unix epoch)
  auto before = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  uint64_t timestamp = UUID::getUtcTimestamp();
  auto after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  int64_t unixMs = static_cast<int64_t>((timestamp - 0x01B21DD213814000ULL) / 10000);
  anch::ut::assertThat(unixMs >= before && unixMs <= after, "Timestamp {} (Unix ms {}) should be between {} and {}", timestamp, unixMs, before, after);
}

void
testUUIDV7Monotonic() {
  // Single thread: strictly increasing (binary and string order)
  UUID previous = UUID::randomTime();
  for(int i = 0 ; i < 200000 ; ++i) {
    UUID current = UUID::randomTime();
    anch::ut::assertThat(previous.toBytes() < current.toBytes(), "{} should be lower than {}", previous.toString(), current.toString());
    anch::ut::assertThat(previous.toString() < current.toString(), "{} should be lower than {} (string)", previous.toString(), current.toString());
    previous = current;
  }
  // 4 threads: strictly increasing in each thread, timestamp and counter unique between threads
  const int nbThreads = 4;
  const int nb = 50000;
  std::vector<std::vector<uint64_t>> keys(nbThreads);
  std::vector<std::thread> threads;
  for(int t = 0 ; t < nbThreads ; ++t) {
    threads.emplace_back([&keys, t]() {
      keys[static_cast<std::size_t>(t)].reserve(nb);
      for(int i = 0 ; i < nb ; ++i) {
	std::array<uint8_t,16> bytes = UUID::randomTime().toBytes();
	// timestamp (48 bits) + counter (12 bits in bytes 6-7, 4 bits in byte 8)
	uint64_t key = 0;
	for(std::size_t b = 0 ; b < 6 ; ++b) {
	  key = (key << 8) | bytes[b];
	}
	key = (key << 16) | (static_cast<uint64_t>(bytes[6] & 0x0F) << 12) | (static_cast<uint64_t>(bytes[7]) << 4) | ((bytes[8] >> 2) & 0x0F);
	keys[static_cast<std::size_t>(t)].push_back(key);
      }
    });
  }
  for(std::thread& thread : threads) {
    thread.join();
  }
  std::set<uint64_t> all;
  for(const std::vector<uint64_t>& threadKeys : keys) {
    for(std::size_t i = 1 ; i < threadKeys.size() ; ++i) {
      anch::ut::assertThat(threadKeys[i - 1] < threadKeys[i], "UUID v7 should be strictly increasing in thread");
    }
    all.insert(threadKeys.begin(), threadKeys.end());
  }
  anch::ut::assertThat(all.size() == static_cast<std::size_t>(nbThreads * nb), "Timestamp and counter should be unique ; found {} / {}", all.size(), nbThreads * nb);
}

void
testUUIDValueType() {
  static_assert(sizeof(UUID) == 16, "UUID should be 16 bytes");
  static_assert(std::is_trivially_copyable_v<UUID>, "UUID should be trivially copyable");
  static_assert(!std::has_virtual_destructor_v<UUID>, "UUID destructor should not be virtual");
  anch::ut::assertThat(UUID().toString() == "00000000-0000-0000-0000-000000000000", "Default UUID should be nil");
  // Order is binary order and string order +
  std::vector<UUID> uuids;
  for(int i = 0 ; i < 10000 ; ++i) {
    uuids.push_back(UUID::random());
  }
  for(std::size_t i = 1 ; i < uuids.size() ; ++i) {
    const UUID& a = uuids[i - 1];
    const UUID& b = uuids[i];
    anch::ut::assertThat((a < b) == (a.toString() < b.toString()), "Order of {} and {} should be string order", a.toString(), b.toString());
    anch::ut::assertThat((a < b) == (a.toBytes() < b.toBytes()), "Order of {} and {} should be binary order", a.toString(), b.toString());
    anch::ut::assertThat((a == b) == (a.toString() == b.toString()), "Equality of {} and {}", a.toString(), b.toString());
  }
  // Order is binary order and string order -
  // Containers +
  std::set<UUID> set(uuids.begin(), uuids.end());
  std::unordered_set<UUID> hashSet(uuids.begin(), uuids.end());
  anch::ut::assertThat(set.size() == uuids.size() && hashSet.size() == uuids.size(), "All UUID should be in containers");
  anch::ut::assertThat(hashSet.contains(UUID(uuids[42].toString())), "Parsed UUID should be found in unordered_set");
  // Containers -
  // Parts constructor +
  UUID parts(0x01234567, 0x89ab, 0x0def, 0x23, 0x91, 0x456789abcdefULL, UUID::Version::RANDOM);
  anch::ut::assertThat(parts.toString() == "01234567-89ab-4def-9123-456789abcdef", "Parts constructor: found {}", parts.toString());
  anch::ut::assertThat(parts.getLowTime() == 0x01234567 && parts.getMidTime() == 0x89ab && parts.getHighTime() == 0x0def
		       && parts.getClockSeqLow() == 0x23 && parts.getClockSeqHighRes() == 0x91 && parts.getNode() == 0x456789abcdefULL
		       && parts.getVersion() == UUID::Version::RANDOM, "Parts getters");
  // Parts constructor -
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH UUID unit tests")
    .description("Test AnCH UUID library")
    .add("uuid-to-string", testUUIDToString)
    .add("uuid-parse", testUUIDParse)
    .add("uuid-parse-ko", testUUIDParseKO)
    .add("uuid-bytes", testUUIDBytes)
    .add("uuid-clock-seq", testUUIDClockSeq)
    .add("uuid-random-format", testUUIDRandomFormat)
    .add("uuid-utc-timestamp", testUUIDUtcTimestamp)
    .add("uuid-v7-monotonic", testUUIDV7Monotonic)
    .add("uuid-value-type", testUUIDValueType)
    .add("uuid-provider-concurrency", testUUIDProviderConcurrency)
    .add("uuid-perf", testUUIDPerf)
    .add("uuid-collision", testUUIDCollision)
    .add("uuid-random", tesUUIDSingle)
    .add("uuid-random-time", tesUUIDRadomTime)
    ;
}
