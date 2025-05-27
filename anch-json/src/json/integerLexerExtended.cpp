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
#ifdef ANCH_JSON_C_INT_PARSER
#include "json/lexer.hpp"

#include <charconv>

void
parseHexadecimal(anch::json::Context& context) {
  char current = context.next();
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
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
      context.pbuf += current;
      break;
    default:
      context.back(context);
      return;
    }
    current = context.next();
  } while(true);
}

void
parseOctal(anch::json::Context& context) {
  char current = context.next();
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
      context.pbuf += current;
      break;
    default:
      context.back(context);
      return;
    }
    current = context.next();
  } while(true);
}

int
parseInt(anch::json::Context& context) {
  char current = context.pbuf[0];
  if(current == '-') {
    current = context.next();
  }
  switch(current) {
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
    parseDecimal(context);
    break;
  case '0': // leading 0 is not allowed according to RFC4627 => only decimal format are allowed
    current = context.next();
    if(current == 'x' || current == 'X') {
      parseHexadecimal(context);
      return 16;
    } else {
      parseOctal(context);
      return 8;
    }
    break;
  default:
    if(value == "-") {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  }
  return 10;
}

bool
anch::json::intLex(anch::json::jint& value,
		   anch::json::jint min,
		   anch::json::jint max,
		   anch::json::Context& context) {
  char current = context.discard();
  switch(current) {
  case 'n':
    anch::json::nullLex(context);
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
    {
      context.pbuf.clear();
      context.pbuf += current;
      int base = parseInt(context);
      std::from_chars(context.pbuf.data(), context.pbuf.data() + val.size() -1, value, base);
    }
    break;
  default:
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  return true;
}

#endif // ANCH_JSON_C_INT_PARSER
