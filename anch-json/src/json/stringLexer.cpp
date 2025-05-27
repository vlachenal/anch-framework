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
anch::json::lexStringContent(anch::json::ReaderContext& context) {
  context.pbuf.clear();
  char current = '\0';
  do {
    context.next(current);
    if(current == anch::json::STRING_DELIMITER) {
      break;
    }
    if(current == '\\') { // ignore character and add next
      context.next(current);
      if(current == '\0') {
	throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
      }
    }
    context.pbuf += current;
  } while(current != '\0');
}

/*void
anch::json::lexStrContent(anch::json::ReaderContext& context) {
  context.pbuf.clear();
  char current = '\0';
  uint32_t begin = context.offset;
  do {
    bool refill = context.next(current);
    if(current == anch::json::STRING_DELIMITER) {
      goto end_str;
    }
    if(current == '\\') { // ignore character and add next
      context.pbuf.append(context.buffer + begin, context.offset - begin - 1);
      context.next(current);
      if(current == '\0') {
	throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
      }
      begin = context.offset;
    }
    if(refill) {
      std::cout << "append buffer from: " << context.buffer[begin] << " ; nb=" << (context.offset - begin) << std::endl;
      context.pbuf.append(context.buffer + begin, context.offset - begin);
      begin = 0;
    }
  } while(current != '\0');
 end_str:
  if(context.offset > 1) {
    uint32_t end = context.offset - 1; // Ignore '"' which is last read character
    if(end >= begin) {
      context.pbuf.append(context.buffer + begin, end - begin);
    }
  }
  return;
  }*/

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
  anch::json::lexStringContent(context);
  value = context.pbuf;
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
  anch::json::lexStringContent(context);
  field = context.pbuf;
  return true;
}
