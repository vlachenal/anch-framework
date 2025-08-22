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

void
anch::json::lexObject(const std::map<std::string,
		      anch::json::LexFunc>& readers,
		      std::set<std::string>& fields,
		      anch::json::ReaderContext& context) {
  char current = '\0';
  std::string field;
  do {
    // Look for field +
    if(!anch::json::lexField(field, context)) { // '}' has been found
      break;
    }
    // Look for field -
    // Look for ':' +
    current = context.discard();
    if(current != anch::json::FIELD_VALUE_SEPARATOR) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    // Look for ':' -
    auto iter = readers.find(field);
    if(iter == readers.cend()) {
      context.parseUnknown();
    } else {
      std::invoke(iter->second, context);
      fields.insert(field);
    }
    // Look for ',' for next field or '}' for end +
    current = context.discard(); // \todo authorizes ",}" sequence for now ... change it ???
    switch(current) {
    case anch::json::FIELD_SEPARATOR: // consume next field
      break;
    case anch::json::OBJECT_END: // end object parsing
      goto end_obj;
    default:
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    // Look for ',' for next field or '}' for end -
  } while(true);
 end_obj:
  return;
}

void
anch::json::lexArray(anch::json::AddItem addFunc, anch::json::ReaderContext& context) {
  char current = context.discard();
  if(current == 'n') {
    anch::json::lexNull(context);
    return;
  } else if(current != anch::json::ARRAY_BEGIN) {
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  current = context.discard();
  if(current == anch::json::ARRAY_END) {
    return;
  }
  context.back();
  do {
    std::invoke(addFunc, context);
    current = context.discard();
    if(current == anch::json::ARRAY_END) {
      return;
    }
    if(current != anch::json::FIELD_SEPARATOR) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  } while(true);
}

void
anch::json::lexMap(anch::json::PushItem pushFunc, anch::json::ReaderContext& context) {
  char current = context.discard();
  if(current == 'n') {
    anch::json::lexNull(context);
    return;
  } else if(current != anch::json::OBJECT_BEGIN) {
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
  current = context.discard();
  if(current == anch::json::OBJECT_END) {
    return;
  }
  context.back();
  std::string field;
  do {
    // Look for field +
    if(!anch::json::lexField(field, context)) { // '}' has been found
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    // Look for field -
    // Look for ':' +
    current = context.discard();
    if(current != anch::json::FIELD_VALUE_SEPARATOR) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    // Look for ':' -
    std::invoke(pushFunc, field, context);
    // Look for ',' for next field or '}' for end +
    current = context.discard();
    if(current == anch::json::OBJECT_END) {
      return;
    }
    if(current != anch::json::FIELD_SEPARATOR) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    // Look for ',' for next field or '}' for end -
  } while(true);
}
