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
#include "json/lexer.hpp"

#include "json/mappingError.hpp"
#include "json/constants.hpp"

using anch::json::ErrorCode;

void
parseUnknownNumber(anch::json::ReaderContext& context) {
  char current = context.discard();
  // Check first characters +
  if(current == '-') {
    context.next(current);
  }
  if(current == '0') {
    context.next(current);
    if(current == '0') {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  }
  // Check first characters -
  bool dot = false;
  do {
    switch(current) {
    case '.':
      if(dot) {
	throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
      }
      dot = true;
      [[fallthrough]]; // no break
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
      goto end_float;
    }
    context.next(current);
  } while(true);
 end_float:
  // Check that last OK character is not '.' or '-' +
  if(current == '.' || current == '-') {
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  // Check that last OK character is not '.' or '-' -
  context.back();
}

void
lexUnknownStr(anch::json::ReaderContext& context) {
  char current = '\0';
  context.next(current);
  while(current != '\0') {
    switch(current) {
    case anch::json::STRING_DELIMITER:
      goto end_str;
    case '\\': // ignore character and add next
      context.next(current);
      if(current == '\0') {
	throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
      }
      break;
    default:
      break; // Ignore character
    }
    context.next(current);
  }
 end_str:
  return;
}

void
lexUnknownArray(anch::json::ReaderContext& context) {
  char current = context.discard();
  if(current == anch::json::ARRAY_END) {
    return;
  }
  context.back();
  do {
    anch::json::lexUnknown(context);
    current = context.discard();
    if(current == anch::json::ARRAY_END) {
      break;
    }
    if(current != anch::json::FIELD_SEPARATOR) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  } while(true);
}

void
lexUnknownObject(anch::json::ReaderContext& context) {
  char current = context.discard();
  if(current == anch::json::OBJECT_END) {
    return;
  }
  context.back();
  do {
    current = context.discard();
    if(current == anch::json::OBJECT_END) {
      return;
    }
    if(current != anch::json::STRING_DELIMITER) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    lexUnknownStr(context);
    current = context.discard();
    if(current != anch::json::FIELD_VALUE_SEPARATOR) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    anch::json::lexUnknown(context);
    current = context.discard();
    if(current == anch::json::OBJECT_END) {
      break;
    }
    if(current != anch::json::FIELD_SEPARATOR) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    current = context.discard();
  } while(true);
}

void
anch::json::lexUnknown(anch::json::ReaderContext& context) {
  char current = context.discard();
  switch(current) {
  case 'n': // parse null
    anch::json::lexNull(context);
    break;
  case 't': // parse true
    anch::json::lexTrue(context);
    break;
  case 'f': // parse false
    anch::json::lexFalse(context);
    break;
  case anch::json::STRING_DELIMITER: // parse string value
    lexUnknownStr(context);
    break;
  case anch::json::OBJECT_BEGIN: // parse object
    lexUnknownObject(context);
    break;
  case anch::json::ARRAY_BEGIN: // parse array
    lexUnknownArray(context);
    break;
  case '-': // parse number
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
    context.back();
    parseUnknownNumber(context);
    break;
  default:
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
}
