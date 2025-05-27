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
#include "json/impl/stringsMapper.hpp"
/*
#include <regex>

#include "json/mappingFunctions.hpp"
#include "json/mappingError.hpp"

std::regex backslash("\\\\");

#include <iostream> // \todo remove

void
anch::json::deserializeStringValue(std::string& value, anch::json::Context& context) {
  // Look for '"'
  int current = context.input.get();
  if(current != anch::json::STRING_DELIMITER) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
  }
  while(context.input) {
    // \todo max char in value ...
    current = context.input.get();
    if(current == anch::json::STRING_DELIMITER) {
      break;
    }
    if(current != '\\') {
      value += static_cast<char>(current);
    } else {
      if(!context.input) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
      }
      value += static_cast<char>(context.input.get());
      // \todo unicode
    }
  }
  if(!context.input && current != anch::json::STRING_DELIMITER) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
  }
}
*/
