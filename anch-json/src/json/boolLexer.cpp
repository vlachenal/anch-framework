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

void
anch::json::lexTrue(anch::json::ReaderContext& context) {
  static char expected[4] = "rue";
  char current;
  for(int i = 0 ; i < 3 ; ++i) {
    context.next(current);
    if(current != expected[i]) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  }
}

void
anch::json::lexFalse(anch::json::ReaderContext& context) {
  static char expected[5] = "alse";
  char current;
  for(int i = 0 ; i < 4 ; ++i) {
    context.next(current);
    if(current != expected[i]) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
  }
}

bool
anch::json::lexBoolean(bool& value, anch::json::ReaderContext& context) {
  char current = context.discard();
  switch(current) {
  case 't':
    anch::json::lexTrue(context);
    value = true;
    return true;
  case 'f':
    anch::json::lexFalse(context);
    value = false;
    return true;
  case 'n':
    anch::json::lexNull(context);
    return false;
  default:
    throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
  }
}
