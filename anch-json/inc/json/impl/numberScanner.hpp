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

#include <cstdint>

namespace anch::json {

  inline
  bool
  isNumberChar(char c) {
    return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E';
  }

  inline
  bool
  isJsonNumber(std::string_view chars) {
    auto isDigit = [](char c) {
      return c >= '0' && c <= '9';
    };
    std::size_t i = 0;
    std::size_t size = chars.size();
    if(i < size && chars[i] == '-') {
      ++i;
    }
    // Integer part +
    if(i == size) {
      return false;
    }
    if(chars[i] == '0') {
      ++i;
    } else if(chars[i] >= '1' && chars[i] <= '9') {
      while(i < size && isDigit(chars[i])) {
	++i;
      }
    } else {
      return false;
    }
    // Integer part -
    // Fraction part +
    if(i < size && chars[i] == '.') {
      std::size_t begin = ++i;
      while(i < size && isDigit(chars[i])) {
	++i;
      }
      if(i == begin) {
	return false;
      }
    }
    // Fraction part -
    // Exponent part +
    if(i < size && (chars[i] == 'e' || chars[i] == 'E')) {
      ++i;
      if(i < size && (chars[i] == '+' || chars[i] == '-')) {
	++i;
      }
      std::size_t begin = i;
      while(i < size && isDigit(chars[i])) {
	++i;
      }
      if(i == begin) {
	return false;
      }
    }
    // Exponent part -
    return i == size;
  }

  template<typename P>
  inline
  std::string_view
  scanNumber(anch::json::ReaderContext& context, P isNumberChar) {
    const char* start = context.buffer + context.offset - 1;
    const char* end = context.buffer + context.options.buffer_size;
    const char* iter = context.buffer + context.offset;
    while(iter != end && isNumberChar(*iter)) {
      ++iter;
    }
    if(iter != end) [[likely]] { // number is entirely in buffer
      context.offset = static_cast<uint32_t>(iter - context.buffer);
      return std::string_view(start, static_cast<std::size_t>(iter - start));
    }
    // Number can continue in next buffer: copy characters and refill buffer +
    context.pbuf.assign(start, static_cast<std::size_t>(iter - start));
    context.offset = static_cast<uint32_t>(context.options.buffer_size);
    char current;
    do {
      context.next(current);
      if(!isNumberChar(current)) {
	context.back();
	break;
      }
      context.pbuf += current;
    } while(true);
    // Number can continue in next buffer: copy characters and refill buffer -
    return context.pbuf;
  }

}
