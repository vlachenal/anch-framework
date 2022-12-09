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

namespace anch::crypto {

  inline void
  Base64::encodeURI(std::istream& data, std::ostream& output) noexcept {
    return encode(data, output, URL | NOPADDING);
  }

  inline std::string
  Base64::encodeURI(std::istream& data) noexcept {
    return encode(data, URL | NOPADDING);
  }

  inline void
  Base64::encodeURI(const std::string& data, std::ostream& output) noexcept {
    return encode(data, output, URL | NOPADDING);
  }

  inline std::string
  Base64::encodeURI(const std::string& data) noexcept {
    return encode(data, URL | NOPADDING);
  }

  inline void
  Base64::encodeURI(const uint8_t* data, uint64_t length, std::ostream& output) noexcept {
    return encode(data, length, output, URL | NOPADDING);
  }

  inline std::string
  Base64::encodeURI(const uint8_t* data, uint64_t length) noexcept {
    return encode(data, length, URL | NOPADDING);
  }

  inline void
  Base64::decodeURI(std::istream& data, std::ostream& output) {
    decode(data, output, URL | NOPADDING);
  }

  inline std::string
  Base64::decodeURI(std::istream& data) {
    return decode(data, URL | NOPADDING);
  }

  inline void
  Base64::decodeURI(const std::string& data, std::ostream& output) {
    decode(data, output, URL | NOPADDING);
  }

  inline std::string
  Base64::decodeURI(const std::string& data) {
    return decode(data, URL | NOPADDING);
  }

  inline void
  Base64::decodeURI(const char* data, uint64_t length, std::ostream& output) {
    decode(data, length, output, URL | NOPADDING);
  }


  inline std::string
  Base64::decodeURI(const char* data, uint64_t length) {
    return  decode(data, length, URL | NOPADDING);
  }

}  // anch::crypto
