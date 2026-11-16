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
#pragma once

#include <string>
#include <functional>
#include <utility>

#include "json/constants.hpp"

namespace anch::json {

  template<typename F>
  void
  lexObject(F&& onField, anch::json::ReaderContext& context) {
    anch::json::DepthGuard guard(context);
    char current = '\0';
    std::string field;
    bool first = true;
    do {
      // Look for field +
      if(!anch::json::lexField(field, context)) { // '}' has been found
	if(!first) { // ",}" sequence
	  throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
	}
	break;
      }
      first = false;
      // Look for field -
      // Look for ':' +
      current = context.discard();
      if(current != anch::json::FIELD_VALUE_SEPARATOR) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
      }
      // Look for ':' -
      std::invoke(onField, std::as_const(field), context);
      // Look for ',' for next field or '}' for end +
      current = context.discard();
      switch(current) {
      case anch::json::FIELD_SEPARATOR: // consume next field
	break;
      case anch::json::OBJECT_END: // end object parsing
	return;
      default:
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
      }
      // Look for ',' for next field or '}' for end -
    } while(true);
  }

  template<typename F>
  bool
  lexArray(F&& addFunc, anch::json::ReaderContext& context) {
    char current = context.discard();
    if(current == 'n') {
      anch::json::lexNull(context);
      return false;
    } else if(current != anch::json::ARRAY_BEGIN) {
      throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
    }
    anch::json::DepthGuard guard(context);
    current = context.discard();
    if(current == anch::json::ARRAY_END) {
      return true;
    }
    context.back();
    std::size_t index = 0;
    do {
      try {
	std::invoke(addFunc, context);
      } catch(anch::json::MappingError& error) {
	error.prependIndex(index);
	throw;
      }
      ++index;
      current = context.discard();
      if(current == anch::json::ARRAY_END) {
	return true;
      }
      if(current != anch::json::FIELD_SEPARATOR) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
      }
    } while(true);
  }

  template<typename F>
  bool
  lexMap(F&& pushFunc, anch::json::ReaderContext& context) {
    char current = context.discard();
    if(current == 'n') {
      anch::json::lexNull(context);
      return false;
    } else if(current != anch::json::OBJECT_BEGIN) {
      throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
    }
    anch::json::DepthGuard guard(context);
    current = context.discard();
    if(current == anch::json::OBJECT_END) {
      return true;
    }
    context.back();
    std::string field;
    do {
      // Look for field +
      if(!anch::json::lexField(field, context)) { // '}' has been found
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
      }
      // Look for field -
      // Look for ':' +
      current = context.discard();
      if(current != anch::json::FIELD_VALUE_SEPARATOR) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
      }
      // Look for ':' -
      try {
	std::invoke(pushFunc, std::as_const(field), context);
      } catch(anch::json::MappingError& error) {
	error.prependField(field);
	throw;
      }
      // Look for ',' for next field or '}' for end +
      current = context.discard();
      if(current == anch::json::OBJECT_END) {
	return true;
      }
      if(current != anch::json::FIELD_SEPARATOR) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
      }
      // Look for ',' for next field or '}' for end -
    } while(true);
  }

}
