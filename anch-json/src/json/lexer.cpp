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

#include "json/mappingError.hpp"
#include "json/constants.hpp"

using anch::json::ErrorCode;

void
anch::json::lexUnknownFail(anch::json::ReaderContext& context) {
  throw anch::json::MappingError(ErrorCode::UNEXPECTED_FIELD, context);
}

char
anch::json::discardFail(anch::json::ReaderContext& context) {
  char current;
  context.next(current);
  switch(current) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
  default:
    return current;
  }
}

char
anch::json::lexDiscard(anch::json::ReaderContext& context) {
  char current;
  int64_t nb = 0;
  do {
    bool bufEnd = context.next(current);
    switch(current) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      ++nb;
      break;
    default:
      goto end;
    }
    if(context.options.deserialize_max_discard_char < 0) {
      continue;
    }
    if(bufEnd && nb > context.options.deserialize_max_discard_char) {
      throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
    }
  } while(true);
 end:
  if(context.options.deserialize_max_discard_char > 0 && nb > context.options.deserialize_max_discard_char) {
    throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
  }
  return current;
}

void
anch::json::lexNull(anch::json::ReaderContext& context) {
  static char expected[4] = "ull";
  char current;
  for(int i = 0 ; i < 3 ; ++i) {
    context.next(current);
    if(current != expected[i]) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  }
}

bool
anch::json::objectHasValueLex(anch::json::ReaderContext& context) {
  char current = context.discard();
  if(current == anch::json::OBJECT_BEGIN) {
    return true;
  } else if(current == 'n') {
    anch::json::lexNull(context);
    return false;
  }
  throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
}
