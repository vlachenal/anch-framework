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

#include <limits>
#include <chrono>
#include <atomic>
#include <charconv>
#include <stdexcept>
#include <system_error>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

using anch::UUID;


std::shared_mutex UUID::_mutex;
std::atomic_bool UUID::_seeded(false);
std::map<UUID::Version, std::function<UUID(const std::string&)>> UUID::_providers;

// Constructors +
UUID::UUID(const std::string& uuid) {
  parse(uuid);
}

UUID::UUID(uint32_t lowTime,
	   uint16_t midTime,
	   uint16_t highTime,
	   uint16_t clockSeqLow,
	   uint16_t clockSeqHighRes,
	   uint64_t node,
	   UUID::Version version) {
  _bytes[0] = static_cast<uint8_t>(lowTime >> 24);
  _bytes[1] = static_cast<uint8_t>(lowTime >> 16);
  _bytes[2] = static_cast<uint8_t>(lowTime >> 8);
  _bytes[3] = static_cast<uint8_t>(lowTime);
  _bytes[4] = static_cast<uint8_t>(midTime >> 8);
  _bytes[5] = static_cast<uint8_t>(midTime);
  _bytes[6] = static_cast<uint8_t>(((static_cast<uint32_t>(version) & 0x0F) << 4) | ((highTime >> 8) & 0x0F));
  _bytes[7] = static_cast<uint8_t>(highTime);
  _bytes[8] = static_cast<uint8_t>(clockSeqHighRes);
  _bytes[9] = static_cast<uint8_t>(clockSeqLow);
  for(std::size_t i = 0 ; i < 6 ; ++i) {
    _bytes[15 - i] = static_cast<uint8_t>(node >> (8 * i));
  }
}
// Constructors -

// Methods +
/*! Hexadecimal digit values (\c 0xFF for other characters) */
static constexpr std::array<uint8_t,256> HEX_VALUES = []() {
  std::array<uint8_t,256> values{};
  values.fill(0xFF);
  for(uint8_t i = 0 ; i < 10 ; ++i) {
    values[static_cast<std::size_t>('0' + i)] = i;
  }
  for(uint8_t i = 0 ; i < 6 ; ++i) {
    values[static_cast<std::size_t>('a' + i)] = static_cast<uint8_t>(10 + i);
    values[static_cast<std::size_t>('A' + i)] = static_cast<uint8_t>(10 + i);
  }
  return values;
}();

void
UUID::parse(const std::string& uuid) {
  if(uuid.size() != STRING_SIZE) {
    throw std::invalid_argument("Invalid UUID '" + uuid + "': length has to be 36");
  }
  // Check and convert in one pass ; value is modified only when UUID is valid +
  std::array<uint8_t,16> bytes;
  std::size_t pos = 0;
  for(std::size_t i = 0 ; i < 16 ; ++i) {
    if(pos == 8 || pos == 13 || pos == 18 || pos == 23) {
      if(uuid[pos] != '-') {
	throw std::invalid_argument("Invalid UUID '" + uuid + "': unexpected character at position " + std::to_string(pos));
      }
      ++pos;
    }
    uint8_t high = HEX_VALUES[static_cast<unsigned char>(uuid[pos])];
    uint8_t low = HEX_VALUES[static_cast<unsigned char>(uuid[pos + 1])];
    if(high == 0xFF || low == 0xFF) {
      std::size_t bad = (high == 0xFF) ? pos : pos + 1;
      throw std::invalid_argument("Invalid UUID '" + uuid + "': unexpected character at position " + std::to_string(bad));
    }
    bytes[i] = static_cast<uint8_t>((high << 4) | low);
    pos += 2;
  }
  _bytes = bytes;
  // Check and convert in one pass ; value is modified only when UUID is valid -
}

UUID
UUID::parseUUID(const std::string& uuid) {
  return UUID(uuid);
}

char*
UUID::toChars(char* first) const {
  static constexpr char HEX[] = "0123456789abcdef";
  for(std::size_t i = 0 ; i < 16 ; ++i) {
    if(i == 4 || i == 6 || i == 8 || i == 10) {
      *first++ = '-';
    }
    *first++ = HEX[_bytes[i] >> 4];
    *first++ = HEX[_bytes[i] & 0x0F];
  }
  return first;
}

std::string
UUID::toString() const {
  std::string res(STRING_SIZE, '0');
  toChars(res.data());
  return res;
}

void
UUID::registerProvider(UUID::Version version, std::function<UUID(const std::string&)> provider) {
  std::unique_lock<std::shared_mutex> lock(_mutex);
  _providers[version] = std::move(provider);
}

void
UUID::registerRandomUUID() {
  // Initialize random seed if not already done +
  std::unique_lock<std::shared_mutex> lock(_mutex);
  if(!_seeded) { // double checked locking: _seeded is atomic and is set after registration
    _providers[UUID::Version::RANDOM] = [](const std::string&) {return UUID::random();};
    _seeded = true;
  }
  // Initialize random seed if not already done -
}

UUID
UUID::generateUUID(anch::UUID::Version version, const std::string& data) {
  if(!_seeded) {
    registerRandomUUID();
  }

  // Copy provider under shared lock and call it without lock (provider can register providers or generate UUID) +
  std::function<UUID(const std::string&)> provider;
  {
    std::shared_lock<std::shared_mutex> lock(_mutex);
    auto iter = _providers.find(version);
    if(iter == _providers.end()) {
      throw std::runtime_error("No provider has been registered for this UUID version");
    }
    provider = iter->second;
  }
  // Copy provider under shared lock and call it without lock (provider can register providers or generate UUID) -
  return provider(data);
}

UUID
UUID::generateUUID(anch::UUID::Version version, const UUID& nameSpace, const std::string& name) {
  std::array<uint8_t,16> bytes = nameSpace.toBytes();
  std::string data(reinterpret_cast<const char*>(bytes.data()), bytes.size());
  data += name;
  return generateUUID(version, data);
}

const UUID&
UUID::namespaceDNS() {
  static const UUID ns("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
  return ns;
}

const UUID&
UUID::namespaceURL() {
  static const UUID ns("6ba7b811-9dad-11d1-80b4-00c04fd430c8");
  return ns;
}

const UUID&
UUID::namespaceOID() {
  static const UUID ns("6ba7b812-9dad-11d1-80b4-00c04fd430c8");
  return ns;
}

const UUID&
UUID::namespaceX500() {
  static const UUID ns("6ba7b814-9dad-11d1-80b4-00c04fd430c8");
  return ns;
}

/*!
 * Fill buffer with cryptographically secure random bytes.\n
 * \c arc4random_buf is used when available (fast, thread and fork safe), \c getentropy otherwise.
 *
 * \param buffer the buffer to fill
 * \param size the number of bytes (at most 256)
 *
 * \throw std::system_error random bytes are not available
 */
static void
fillRandomBytes(uint8_t* buffer, std::size_t size) {
#ifdef ANCH_UUID_ARC4RANDOM
  arc4random_buf(buffer, size);
#else
  if(getentropy(buffer, size) != 0) {
    throw std::system_error(errno, std::generic_category(), "Unable to get random bytes");
  }
#endif
}

UUID
UUID::random() {
  if(!_seeded) {
    registerRandomUUID();
  }
  // 16 random bytes in one call, then version (4) and RFC 9562 variant (10xx) bits are forced
  UUID uuid;
  fillRandomBytes(uuid._bytes.data(), uuid._bytes.size());
  uuid._bytes[6] = static_cast<uint8_t>((uuid._bytes[6] & 0x0F) | 0x40);
  uuid._bytes[8] = static_cast<uint8_t>((uuid._bytes[8] & 0x3F) | 0x80);
  return uuid;
}

/*!
 * Version 7 monotonic state: Unix timestamp in milliseconds (48 bits) and counter (16 bits) packed as
 * \c (timestamp << 16) | counter (RFC 9562 section 6.2, method 1). The counter is written in the 12 \c rand_a bits
 * and in the 4 first \c rand_b bits (after variant).
 */
static std::atomic<uint64_t> V7_STATE(0);

/*!
 * Get next version 7 timestamp and counter.\n
 * On a new millisecond, counter is initialized with a 15 bits random value (most significant bit is \c 0 to keep at
 * least 32768 increments). In the same millisecond, or when clock goes backward, counter is incremented: its overflow
 * increments the timestamp. Successive values are strictly increasing, including between threads.
 *
 * \param randomCounter the random counter initialization value (only 15 bits are used)
 *
 * \return the packed timestamp and counter
 */
static uint64_t
nextV7State(uint16_t randomCounter) {
  // Unix time in milliseconds (system_clock epoch is Unix epoch since C++20)
  auto epoch = std::chrono::system_clock::now().time_since_epoch();
  uint64_t now = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count());
  uint64_t previous = V7_STATE.load(std::memory_order_relaxed);
  uint64_t next;
  do {
    if(now > (previous >> 16)) { // new millisecond: random counter
      next = (now << 16) | (randomCounter & 0x7FFFu);
    } else { // same millisecond or clock goes backward: increment counter (overflow increments timestamp)
      next = previous + 1;
    }
  } while(!V7_STATE.compare_exchange_weak(previous, next, std::memory_order_relaxed));
  return next;
}

UUID
UUID::randomTime() {
  if(!_seeded) {
    registerRandomUUID();
  }
  // Random bytes in one call (counter initialization value is taken from bytes 6 and 7) +
  UUID uuid;
  fillRandomBytes(uuid._bytes.data(), uuid._bytes.size());
  uint64_t state = nextV7State(static_cast<uint16_t>((uuid._bytes[6] << 8) | uuid._bytes[7]));
  // Random bytes in one call (counter initialization value is taken from bytes 6 and 7) -
  // Timestamp (48 bits, big-endian) and counter +
  uint64_t timestamp = state >> 16;
  uint16_t counter = static_cast<uint16_t>(state & 0xFFFF);
  for(std::size_t i = 0 ; i < 6 ; ++i) {
    uuid._bytes[5 - i] = static_cast<uint8_t>(timestamp >> (8 * i));
  }
  uuid._bytes[6] = static_cast<uint8_t>(0x70 | (counter >> 12)); // version 7 + 4 most significant counter bits
  uuid._bytes[7] = static_cast<uint8_t>(counter >> 4); // next 8 counter bits (end of rand_a)
  // Variant (10), 4 least significant counter bits, 2 random bits
  uuid._bytes[8] = static_cast<uint8_t>(0x80 | ((counter & 0x0F) << 2) | (uuid._bytes[8] & 0x03));
  // Timestamp (48 bits, big-endian) and counter -
  return uuid;
}

uint64_t
UUID::getUtcTimestamp() {
  static const uint64_t UTC_OFFSET = 0x01B21DD213814000;
  // 100 ns intervals since 1582-10-15 (Gregorian calendar) from Unix time (system_clock epoch since C++20)
  auto epoch = std::chrono::system_clock::now().time_since_epoch();
  int64_t timestamp = (std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count()) / 100;
  return UTC_OFFSET + (uint64_t)timestamp;
}
// Methods -
