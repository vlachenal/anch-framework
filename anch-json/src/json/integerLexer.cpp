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

bool
anch::json::lexInteger(anch::json::jint& value, anch::json::ReaderContext& context) {
  char current = context.discard();
  switch(current) {
  case 'n':
    anch::json::lexNull(context);
    return false;
  case '0': // case 0 as octal or hexa will be implemented if needed .. forbidden according to RFC4627 (last update 2021)
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  case '-':
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
  char* last = context.pbuf.data() + context.pbuf.size();
  auto res = std::from_chars(context.pbuf.data(), last, value);
  if(res.ptr != last) {
    if(res.ec == std::errc::result_out_of_range) {
      throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
    } else {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
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
  case '0': // case 0 as octal or hexa will be implemented if needed .. forbidden according to RFC4627 (last update 2021)
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
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
  char* last = context.pbuf.data() + context.pbuf.size();
  auto res = std::from_chars(context.pbuf.data(), last, value);
  if(res.ptr != last) {
    if(res.ec == std::errc::result_out_of_range) {
      throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
    } else {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  }
  return true;
}

#endif // ANCH_JSON_C_INT_PARSER
