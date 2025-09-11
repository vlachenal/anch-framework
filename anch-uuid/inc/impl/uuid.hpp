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
  bool
  UUID::operator !=(const UUID& uuid) const {
    return !(*this == uuid);
  }

  inline
  UUID::operator std::string() const {
    return toString();
  }

  inline
  uint32_t
  UUID::getLowTime() const {
    return _lowTime;
  }

  inline
  uint16_t
  UUID::getMidTime() const {
    return _midTime;
  }

  inline
  uint16_t
  UUID::getHighTime() const {
    return _highTime;
  }

  inline
  uint16_t
  UUID::getClockSeqLow() const {
    return _clockSeqLow;
  }

  inline
  uint16_t
  UUID::getClockSeqHighRes() const {
    return _clockSeqHighRes;
  }

  inline
  uint64_t
  UUID::getNode() const {
    return _node;
  }

  inline
  anch::UUID::Version
  UUID::getVersion() const {
    return _version;
  }

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
