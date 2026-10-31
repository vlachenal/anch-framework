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
#pragma once

namespace anch {

  inline
  UUID::operator std::string() const {
    return toString();
  }

  inline
  uint32_t
  UUID::getLowTime() const {
    return (static_cast<uint32_t>(_bytes[0]) << 24)
      | (static_cast<uint32_t>(_bytes[1]) << 16)
      | (static_cast<uint32_t>(_bytes[2]) << 8)
      | static_cast<uint32_t>(_bytes[3]);
  }

  inline
  uint16_t
  UUID::getMidTime() const {
    return static_cast<uint16_t>((_bytes[4] << 8) | _bytes[5]);
  }

  inline
  uint16_t
  UUID::getHighTime() const {
    return static_cast<uint16_t>(((_bytes[6] & 0x0F) << 8) | _bytes[7]);
  }

  inline
  uint16_t
  UUID::getClockSeqLow() const {
    return _bytes[9];
  }

  inline
  uint16_t
  UUID::getClockSeqHighRes() const {
    return _bytes[8];
  }

  inline
  uint64_t
  UUID::getNode() const {
    uint64_t node = 0;
    for(std::size_t i = 10 ; i < 16 ; ++i) {
      node = (node << 8) | _bytes[i];
    }
    return node;
  }

  inline
  anch::UUID::Version
  UUID::getVersion() const {
    return static_cast<anch::UUID::Version>(_bytes[6] >> 4);
  }

  inline
  UUID
  UUID::fromBytes(const std::array<uint8_t,16>& bytes) {
    UUID uuid;
    uuid._bytes = bytes;
    return uuid;
  }

  inline
  std::array<uint8_t,16>
  UUID::toBytes() const {
    return _bytes;
  }

}

inline
std::size_t
std::hash<anch::UUID>::operator()(const anch::UUID& uuid) const noexcept {
  // Bytes are random for most versions: combine the two halves
  std::array<uint8_t,16> bytes = uuid.toBytes();
  uint64_t high = 0;
  uint64_t low = 0;
  for(std::size_t i = 0 ; i < 8 ; ++i) {
    high = (high << 8) | bytes[i];
    low = (low << 8) | bytes[i + 8];
  }
  return static_cast<std::size_t>(high ^ (low * 0x9E3779B97F4A7C15ULL));
}

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& out, const anch::UUID& uuid) {
  std::ios_base::fmtflags flags = out.flags(); // Save current flags
  out << std::hex
      << std::setfill('0') << std::setw(8) << uuid.getLowTime() << '-'
      << std::setfill('0') << std::setw(4) << uuid.getMidTime() << '-'
      << std::setfill('0') << std::setw(1) << static_cast<int>(uuid.getVersion())
      << std::setfill('0') << std::setw(3) << uuid.getHighTime() << '-'
      << std::setfill('0') << std::setw(2) << uuid.getClockSeqHighRes()
      << std::setfill('0') << std::setw(2) << uuid.getClockSeqLow() << '-'
      << std::setfill('0') << std::setw(12) << uuid.getNode();
  out.flags(flags); // Restore flags
  return out;
}
