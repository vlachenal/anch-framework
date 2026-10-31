/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ANCH_JSON_C_INT_PARSER
#include "json/lexer.hpp"

#include <charconv>

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
 * Check that parsed integer has no leading zero (only \c 0 and \c -0 can start with \c 0 according to RFC 8259)
 *
 * \param context the reader context which contains the parsed integer
 *
 * \throw anch::json::MappingError when integer has leading zero
 */
static void
checkLeadingZero(anch::json::ReaderContext& context) {
  std::size_t first = (context.pbuf.starts_with('-')) ? 1 : 0;
  if(context.pbuf.size() > first + 1 && context.pbuf[first] == '0') {
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
    context.pbuf.clear();
    context.pbuf += current;
    anch::json::parseDecimal(context);
    break;
  default:
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  checkLeadingZero(context);
  char* last = context.pbuf.data() + context.pbuf.size();
  auto res = std::from_chars(context.pbuf.data(), last, value);
  // On overflow, std::from_chars consumes every digit (ptr == last): error code has to be checked first
  if(res.ec == std::errc::result_out_of_range) {
    throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
  }
  if(res.ec != std::errc() || res.ptr != last) {
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
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
    context.pbuf.clear();
    context.pbuf += current;
    anch::json::parseDecimal(context);
    break;
  default:
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  checkLeadingZero(context);
  char* last = context.pbuf.data() + context.pbuf.size();
  auto res = std::from_chars(context.pbuf.data(), last, value);
  // On overflow, std::from_chars consumes every digit (ptr == last): error code has to be checked first
  if(res.ec == std::errc::result_out_of_range) {
    throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
  }
  if(res.ec != std::errc() || res.ptr != last) {
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  return true;
}

#endif // ANCH_JSON_C_INT_PARSER
