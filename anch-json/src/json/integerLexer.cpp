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
#ifndef ANCH_JSON_C_INT_PARSER
#include "json/lexer.hpp"

#include <charconv>
#include <string_view>

#include "json/numberScanner.hpp"

using anch::json::ErrorCode;

void
anch::json::parseDecimal(anch::json::ReaderContext& context) {
  char current = '\0';
  context.next(current);
  do {
    switch(current) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      context.pbuf += current;
      break;
    default:
      context.back();
      return;
    }
    context.next(current);
  } while(true);
}

/*!
 * Check that number characters are a valid JSON integer: optional '-' (when signed), then \c 0 or digits
 * without leading zero (RFC 8259)
 *
 * \param chars the number characters
 * \param allowMinus allow '-' as first character
 *
 * \return \c true when valid
 */
static bool
isJsonInteger(std::string_view chars, bool allowMinus) {
  std::size_t i = 0;
  if(allowMinus && !chars.empty() && chars[0] == '-') {
    ++i;
  }
  if(i == chars.size()) { // no digit
    return false;
  }
  if(chars[i] == '0' && chars.size() - i > 1) { // leading zero
    return false;
  }
  for( ; i < chars.size() ; ++i) {
    if(chars[i] < '0' || chars[i] > '9') {
      return false;
    }
  }
  return true;
}

/*!
 * Convert integer characters
 *
 * \param chars the number characters
 * \param value the value to write in
 * \param context the reader context (for errors)
 *
 * \throw anch::json::MappingError invalid integer or overflow
 */
template<typename T>
static void
toInteger(std::string_view chars, T& value, anch::json::ReaderContext& context) {
  const char* last = chars.data() + chars.size();
  auto res = std::from_chars(chars.data(), last, value);
  // On overflow, std::from_chars consumes every digit (ptr == last): error code has to be checked first
  if(res.ec == std::errc::result_out_of_range) {
    throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
  }
  if(res.ec != std::errc() || res.ptr != last) {
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
}

bool
anch::json::lexInteger(anch::json::jint& value, anch::json::ReaderContext& context) {
  char current = context.discard();
  switch(current) {
  case 'n':
    anch::json::lexNull(context);
    return false;
  case '-':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    break;
  default:
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  std::string_view chars = anch::json::scanNumber(context, [](char c) {
    return (c >= '0' && c <= '9') || c == '-';
  });
  if(!isJsonInteger(chars, true)) {
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  toInteger(chars, value, context);
  return true;
}

bool
anch::json::lexUInteger(anch::json::juint& value, anch::json::ReaderContext& context) {
  char current = context.discard();
  switch(current) {
  case 'n':
    anch::json::lexNull(context);
    return false;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    break;
  default:
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  std::string_view chars = anch::json::scanNumber(context, [](char c) {
    return c >= '0' && c <= '9';
  });
  if(!isJsonInteger(chars, false)) {
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  toInteger(chars, value, context);
  return true;
}

#endif // ANCH_JSON_C_INT_PARSER
