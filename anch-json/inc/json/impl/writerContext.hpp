/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2020 Vincent Lachenal

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
#pragma once

#include <array>
#include <charconv>
#include <ios>
#include <string>
#include <system_error>

#include "json/constants.hpp"

namespace anch::json {

  inline
  void
  WriterContext::write(char c) {
    if(_sbuf == NULL || _sbuf->sputc(c) == std::char_traits<char>::eof()) [[unlikely]] {
      output.setstate(std::ios_base::badbit);
    }
  }

  inline
  void
  WriterContext::write(std::string_view str) {
    std::streamsize size = static_cast<std::streamsize>(str.size());
    if(_sbuf == NULL || _sbuf->sputn(str.data(), size) != size) [[unlikely]] {
      output.setstate(std::ios_base::badbit);
    }
  }

  template<typename T>
  inline
  void
  WriterContext::writeNumber(T value) {
    char buffer[64]; // enough for any integer or shortest floating point representation
    auto res = std::to_chars(buffer, buffer + sizeof(buffer), value);
    if(res.ec != std::errc()) [[unlikely]] {
      output.setstate(std::ios_base::failbit);
      return;
    }
    write(std::string_view(buffer, static_cast<std::size_t>(res.ptr - buffer)));
  }

  /*! Characters which have to be escaped in JSON strings ('"', '\\' and control characters) */
  inline constexpr std::array<bool,256> ESCAPED_CHARS = []() {
    std::array<bool,256> chars{};
    for(std::size_t i = 0 ; i < 0x20 ; ++i) {
      chars[i] = true;
    }
    chars[static_cast<unsigned char>(anch::json::STRING_DELIMITER)] = true;
    chars[static_cast<unsigned char>('\\')] = true;
    return chars;
  }();

  inline
  void
  WriterContext::writeEscaped(std::string_view str) {
    const char* begin = str.data();
    const char* end = begin + str.size();
    for(const char* iter = begin ; iter != end ; ++iter) {
      if(ESCAPED_CHARS[static_cast<unsigned char>(*iter)]) [[unlikely]] {
	write(std::string_view(begin, static_cast<std::size_t>(iter - begin)));
	writeEscapeSequence(*iter);
	begin = iter + 1;
      }
    }
    write(std::string_view(begin, static_cast<std::size_t>(end - begin)));
  }

  inline
  void
  WriterContext::writeString(std::string_view str) {
    write(anch::json::STRING_DELIMITER);
    writeEscaped(str);
    write(anch::json::STRING_DELIMITER);
  }

  inline
  void
  WriterContext::writeField(const std::string& field) {
    next();
    writeString(field);
    write(anch::json::FIELD_VALUE_SEPARATOR);
    if(_options.prettify_nbs != 0) {
      write(' ');
    }
  }

  inline
  void
  WriterContext::writeRawField(std::string_view field) {
    next();
    write(anch::json::STRING_DELIMITER);
    write(field);
    write(anch::json::STRING_DELIMITER);
    write(anch::json::FIELD_VALUE_SEPARATOR);
    if(_options.prettify_nbs != 0) {
      write(' ');
    }
  }

  inline
  void
  WriterContext::beginArray() {
    write(anch::json::ARRAY_BEGIN);
    _firsts.push_back(true);
  }

  inline
  void
  WriterContext::endArray() {
    _firsts.pop_back();
    if(_options.prettify_nbs != 0) {
      indent();
    }
    write(anch::json::ARRAY_END);
  }

  inline
  void
  WriterContext::beginObject() {
    write(anch::json::OBJECT_BEGIN);
    _firsts.push_back(true);
  }

  inline
  void
  WriterContext::endObject() {
    _firsts.pop_back();
    if(_options.prettify_nbs != 0) {
      indent();
    }
    write(anch::json::OBJECT_END);
  }

  inline
  void
  WriterContext::next() {
    if(!_firsts.back()) {
      write(anch::json::FIELD_SEPARATOR);
    } else {
      _firsts.back() = false;
    }
    if(_options.prettify_nbs != 0) {
      indent();
    }
  }

}
