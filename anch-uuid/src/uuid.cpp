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

using anch::UUID;


std::mutex UUID::_mutex;
std::atomic_bool UUID::_seeded(false);
std::map<UUID::Version, std::function<UUID(const std::string&)>> UUID::_providers;

// Constructors +
UUID::UUID(): _lowTime(0),
	      _midTime(0),
	      _highTime(0),
	      _clockSeqLow(0),
	      _clockSeqHighRes(0),
	      _node(0),
	      _version(Version::NOT_SET) {
  // Nothing to do
}

UUID::UUID(const UUID& uuid): _lowTime(uuid._lowTime),
			      _midTime(uuid._midTime),
			      _highTime(uuid._highTime),
			      _clockSeqLow(uuid._clockSeqLow),
			      _clockSeqHighRes(uuid._clockSeqHighRes),
			      _node(uuid._node),
			      _version(uuid._version) {
  // Nothing to do
}

UUID::UUID(UUID&& uuid): _lowTime(uuid._lowTime),
			 _midTime(uuid._midTime),
			 _highTime(uuid._highTime),
			 _clockSeqLow(uuid._clockSeqLow),
			 _clockSeqHighRes(uuid._clockSeqHighRes),
			 _node(uuid._node),
			 _version(uuid._version) {
  // Nothing to do
}

UUID::UUID(const std::string& uuid): _lowTime(0),
				     _midTime(0),
				     _highTime(0),
				     _clockSeqLow(0),
				     _clockSeqHighRes(0),
				     _node(0) {
  parse(uuid);
}

UUID::UUID(uint32_t lowTime,
	   uint16_t midTime,
	   uint16_t highTime,
	   uint16_t clockSeqLow,
	   uint16_t clockSeqHighRes,
	   uint64_t node,
	   UUID::Version version): _lowTime(lowTime),
				   _midTime(midTime),
				   _highTime(highTime),
				   _clockSeqLow(clockSeqLow),
				   _clockSeqHighRes(clockSeqHighRes),
				   _node(node),
				   _version(version) {
  // Nothing to do
}
// Constructors -


// Destructor +
UUID::~UUID() {
  // Nothing to do
}
// Destructor -

// Methods +
void
UUID::parse(const std::string& uuid) {
  _lowTime = static_cast<uint32_t>(std::stoul(uuid.substr(0, 8), 0, 16));
  _midTime = static_cast<uint16_t>(std::stoul(uuid.substr(9, 4), 0, 16));
  _version = static_cast<anch::UUID::Version>(std::stoi(uuid.substr(14, 1), 0, 16));
  _highTime = static_cast<uint16_t>(std::stoul(uuid.substr(15, 3), 0, 16));
  _clockSeqHighRes = static_cast<uint16_t>(std::stoul(uuid.substr(19, 2), 0, 16));
  _clockSeqLow = static_cast<uint16_t>(std::stoul(uuid.substr(21, 2), 0, 16));
  _node = std::stoull(uuid.substr(24, 12), 0, 16);
}

UUID
UUID::parseUUID(const std::string& uuid) {
  return UUID(uuid);
}

std::string
UUID::toString() const {
  std::ostringstream out;
  out << std::hex
      << std::setfill('0') << std::setw(8) << _lowTime << '-'
      << std::setfill('0') << std::setw(4) << _midTime << '-'
      << std::setfill('0') << std::setw(1) << static_cast<int>(_version)
      << std::setfill('0') << std::setw(3) << _highTime << '-'
      << std::setfill('0') << std::setw(2) << _clockSeqHighRes
      << std::setfill('0') << std::setw(2) << _clockSeqLow << '-'
      << std::setfill('0') << std::setw(12) << _node;
  return out.str();
}

void
UUID::registerProvider(UUID::Version version, std::function<UUID(const std::string&)> provider) {
  _providers[version] = provider;
}

void
UUID::registerRandomUUID() {
  // Initialize random seed if not already done +
  std::lock_guard<std::mutex> lock(_mutex);
  if(!_seeded) { // double check locking ... not really great ...
    std::random_device rand;
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

  if(_providers.find(version) == _providers.end()) {
    throw std::runtime_error("No provider has been registered for this UUID version");
  }
  return _providers[version](data);
}

UUID
UUID::random() {
  if(!_seeded) {
    registerRandomUUID();
  }

  UUID uuid;
  uuid._version = UUID::Version::RANDOM;
  static std::uniform_int_distribution<uint16_t> dist16;
  static std::uniform_int_distribution<uint32_t> dist32;
  static std::uniform_int_distribution<uint64_t> dist64(0, 0xFFFFFFFFFFFF); // Max 12 hexadecimal digits

  // Timestamp +
  uuid._lowTime = dist32(getRandomEngine());
  uuid._midTime = dist16(getRandomEngine());
  uuid._highTime = dist16(getRandomEngine()) & TIME_HIGH_MASK;
  // Timestamp -

  // Sequence +
  uint16_t clockSeq = getDistSeq()(getRandomEngine());
  uuid._clockSeqLow = clockSeq & SEQ_LOW_MASK;
  uuid._clockSeqHighRes = (clockSeq & SEQ_HIGH_MASK) >> 8;
  uuid._clockSeqHighRes |= 0x80;
  // Sequence -

  // Node +
  uuid._node = dist64(getRandomEngine());
  // Node -
  return uuid;
}

uint64_t
UUID::getUtcTimestamp() {
  static const uint64_t UTC_OFFSET = 0x01B21DD213814000;
  std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
  auto epoch = now.time_since_epoch();
  int64_t timestamp = (std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count()) / 100;
  return UTC_OFFSET + (uint64_t)timestamp;
}

std::random_device&
UUID::getRandomEngine() {
  static std::random_device engine;
  return engine;
}

std::uniform_int_distribution<uint16_t>&
UUID::getDistSeq() {
  static std::uniform_int_distribution<uint16_t> distSeq(0, SEQ_HIGH_MASK);
  return distSeq;
}
// Methods -

// Operators +
bool
UUID::operator ==(const UUID& uuid) const {
  return _lowTime == uuid._lowTime
    && _midTime == uuid._midTime
    && _version == uuid._version
    && _highTime == uuid._highTime
    && _clockSeqHighRes == uuid._clockSeqHighRes
    && _clockSeqLow == uuid._clockSeqLow
    && _node == uuid._node
    ;
}

UUID&
UUID::operator =(const UUID& uuid) {
  _lowTime = uuid._lowTime;
  _midTime = uuid._midTime;
  _version = uuid._version;
  _highTime = uuid._highTime;
  _clockSeqHighRes = uuid._clockSeqHighRes;
  _clockSeqLow = uuid._clockSeqLow;
  _node = uuid._node;
  return *this;
}
// Operators -
