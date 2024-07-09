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
#include <iomanip>

namespace anch::crypto {

  template<std::size_t O, std::size_t B>
  const std::array<uint8_t,O>&
  Hash<O,B>::digest(const std::string& data) {
    reset();
    addData(reinterpret_cast<const uint8_t*>(data.data()), data.length());
    finalize();
    return *_digest;
  }

  template<std::size_t O, std::size_t B>
  template<std::size_t N>
  const std::array<uint8_t,O>&
  Hash<O,B>::digest(const std::array<uint8_t,N>& data) {
    reset();
    addData(data.data(), N);
    finalize();
    return digest();
  }

  template<std::size_t O, std::size_t B>
  const std::array<uint8_t,O>&
  Hash<O,B>::digest(const uint8_t* data, std::size_t len) {
    reset();
    addData(data, len);
    finalize();
    return digest();
  }

  template<std::size_t O, std::size_t B>
  const std::array<uint8_t,O>&
  Hash<O,B>::digest(std::istream& stream) {
    reset();
    if(stream) {
      char data[1024];
      while(!stream.eof()) {
	stream.read(data, 1024);
	addData(reinterpret_cast<uint8_t*>(data), static_cast<std::size_t>(stream.gcount()));
      }
      finalize();
    }
    return digest();
  }

}

template<std::size_t O, std::size_t B>
std::ostream&
operator << (std::ostream& out, const anch::crypto::Hash<O,B>& hash) {
  std::ios_base::fmtflags flags = out.flags(); // Save current flags
  out << std::hex;
  for(const uint8_t& byte : hash.digest()) {
    out << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(byte);
  }
  out.flags(flags); // Restore flags
  return out;
}
