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

#include <charconv>

void
anch::json::parseNumber(anch::json::ReaderContext& context) {
  context.pbuf.clear();
  bool dot = false;
  char current = context.discard();
  // Check first characters +
  if(current == '-') {
    context.pbuf += current;
    context.next(current);
  }
  if(current == '0') {
    context.pbuf += current;
    context.next(current);
    if(current != '.') {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    context.pbuf += current;
    context.next(current);
    dot = true;
  }
  context.back();
  // Check first characters -
  anch::json::parseDecimal(context);
  context.next(current);
  // Check for '.' +
  if(current == '.') {
    if(dot) {
      throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
    }
    context.pbuf += current;
    anch::json::parseDecimal(context);
    context.next(current);
  }
  // Check for '.' -
  // Check for exponential +
  if(current == 'e' || current == 'E' ) {
    context.pbuf += 'E';
    context.next(current);
    if(current == '+' || current == '-') {
      context.pbuf += current;
    } else {
      context.pbuf += '+';
      context.back();
    }
    anch::json::parseDecimal(context);
    context.next(current);
  }
  // Check for exponential -
  context.back();
}

bool
anch::json::lexNumber(double& value, anch::json::ReaderContext& context) {
  char current = context.discard();
  if(current == 'n') {
    anch::json::lexNull(context);
    return false;
  }
  context.back();
  anch::json::parseNumber(context);
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
