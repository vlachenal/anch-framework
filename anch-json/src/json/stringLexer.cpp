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
#include "json/lexer.hpp"

#include <array>
#include <cstdint>

#include "json/mappingError.hpp"
#include "json/constants.hpp"

using anch::json::ErrorCode;


namespace {

  /*!
   * Characters which stop string block copy: string end, escape character and end of data ('\0' which is
   * also the sentinel after the reader buffer)
   */
  constexpr std::array<bool,256> STOP_CHARS = []() {
    std::array<bool,256> stops{};
    stops[static_cast<unsigned char>(anch::json::STRING_DELIMITER)] = true;
    stops[static_cast<unsigned char>('\\')] = true;
    stops[0] = true;
    return stops;
  }();

  /*!
   * Read 4 hexadecimal digits (\c \\uXXXX escape sequence)
   *
   * \param context the reader context
   *
   * \return the code unit
   *
   * \throw anch::json::MappingError invalid hexadecimal digit
   */
  uint32_t
  readHex4(anch::json::ReaderContext& context) {
    uint32_t value = 0;
    char current;
    for(int i = 0 ; i < 4 ; ++i) {
      context.next(current);
      uint32_t digit;
      if(current >= '0' && current <= '9') {
	digit = static_cast<uint32_t>(current - '0');
      } else if(current >= 'a' && current <= 'f') {
	digit = static_cast<uint32_t>(current - 'a' + 10);
      } else if(current >= 'A' && current <= 'F') {
	digit = static_cast<uint32_t>(current - 'A' + 10);
      } else {
	throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
      }
      value = (value << 4) | digit;
    }
    return value;
  }

  /*!
   * Append code point to string as UTF-8
   *
   * \param out the string to append to
   * \param cp the code point
   */
  void
  appendUtf8(std::string& out, uint32_t cp) {
    if(cp < 0x80) {
      out += static_cast<char>(cp);
    } else if(cp < 0x800) {
      out += static_cast<char>(0xC0 | (cp >> 6));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    } else if(cp < 0x10000) {
      out += static_cast<char>(0xE0 | (cp >> 12));
      out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
      out += static_cast<char>(0xF0 | (cp >> 18));
      out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
      out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      out += static_cast<char>(0x80 | (cp & 0x3F));
    }
  }

  /*!
   * Decode \c \\uXXXX escape sequence (\c \\u has already been read) with UTF-16 surrogate pairs
   *
   * \param out the string to append to
   * \param context the reader context
   *
   * \throw anch::json::MappingError invalid sequence or lone surrogate
   */
  void
  decodeUnicode(std::string& out, anch::json::ReaderContext& context) {
    uint32_t cp = readHex4(context);
    if(cp >= 0xD800 && cp <= 0xDBFF) { // high surrogate: low surrogate has to follow
      char current;
      context.next(current);
      if(current != '\\') {
	throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
      }
      context.next(current);
      if(current != 'u') {
	throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
      }
      uint32_t low = readHex4(context);
      if(low < 0xDC00 || low > 0xDFFF) {
	throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
      }
      cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
    } else if(cp >= 0xDC00 && cp <= 0xDFFF) { // lone low surrogate
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    appendUtf8(out, cp);
  }

  /*!
   * Decode escape sequence (\c \\ has already been read)
   *
   * \param out the string to append to
   * \param context the reader context
   *
   * \throw anch::json::MappingError invalid escape sequence
   */
  void
  decodeEscape(std::string& out, anch::json::ReaderContext& context) {
    char current;
    context.next(current);
    switch(current) {
    case '"':
    case '\\':
    case '/':
      out += current;
      break;
    case 'b':
      out += '\b';
      break;
    case 'f':
      out += '\f';
      break;
    case 'n':
      out += '\n';
      break;
    case 'r':
      out += '\r';
      break;
    case 't':
      out += '\t';
      break;
    case 'u':
      decodeUnicode(out, context);
      break;
    default: // includes end of data
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  }

}

void
anch::json::lexStringContent(std::string& value, anch::json::ReaderContext& context) {
  value.clear();
  char current;
  do {
    // Copy characters until next stop character (buffer[buffer_size] is always '\0') +
    const char* begin = context.buffer + context.offset;
    const char* iter = begin;
    while(!STOP_CHARS[static_cast<unsigned char>(*iter)]) {
      ++iter;
    }
    value.append(begin, static_cast<std::size_t>(iter - begin));
    context.offset = static_cast<uint32_t>(iter - context.buffer);
    // Copy characters until next stop character -
    // Consume stop character (refill buffer when its end has been reached) +
    context.next(current);
    switch(current) {
    case anch::json::STRING_DELIMITER:
      return;
    case '\\':
      decodeEscape(value, context);
      break;
    case '\0': // end of data before end of string
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    default: // buffer has been refilled: first character is a string character
      value += current;
    }
    // Consume stop character (refill buffer when its end has been reached) -
  } while(true);
}

void
anch::json::lexStringContent(anch::json::ReaderContext& context) {
  anch::json::lexStringContent(context.pbuf, context);
}

bool
anch::json::lexString(std::string& value, anch::json::ReaderContext& context) {
  char current = context.discard();
  if(current == 'n') { // parse null
    anch::json::lexNull(context);
    return false;
  }
  if(current != anch::json::STRING_DELIMITER) { // look for begin string value ; fail if not found
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  anch::json::lexStringContent(value, context);
  return true;
}

bool
anch::json::lexField(std::string& field, anch::json::ReaderContext& context) {
  char current = context.discard();
  if(current == anch::json::OBJECT_END) { // end object parsing
    return false;
  }
  if(current != anch::json::STRING_DELIMITER) { // look for begin string value ; fail if not found
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  anch::json::lexStringContent(field, context);
  return true;
}
