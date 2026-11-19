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

#include "anch-json-export.hpp"

namespace anch::json {

  // Lexer functions used by context (declared in json/lexer.hpp) +
  ANCH_JSON_EXPORT char discardFail(anch::json::ReaderContext& context);
  ANCH_JSON_EXPORT char lexDiscard(anch::json::ReaderContext& context);
  ANCH_JSON_EXPORT void lexUnknown(anch::json::ReaderContext& context);
  ANCH_JSON_EXPORT void lexUnknownFail(anch::json::ReaderContext& context);
  // Lexer functions used by context (declared in json/lexer.hpp) -

  [[gnu::always_inline]] inline // called for each token: has to be inlined whatever the code size of the unit is
  char
  ReaderContext::discard() {
    // Fast path: next character is not discardable +
    char current;
    next(current);
    switch(current) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      break;
    default:
      return current;
    }
    // Fast path: next character is not discardable -
    back();
    if(options.deserialize_max_discard_char == 0) {
      return anch::json::discardFail(*this);
    }
    return anch::json::lexDiscard(*this);
  }

  inline
  void
  ReaderContext::parseUnknown() {
    if(options.deserialize_ignore_unknown_field) {
      anch::json::lexUnknown(*this);
    } else {
      anch::json::lexUnknownFail(*this);
    }
  }

  inline
  bool
  ReaderContext::next(char& current) {
    // Consume input stream in buffer when buffer end has been reached and input is available +
    if(offset == options.buffer_size) [[unlikely]] {
      refillBuffer();
    }
    // Consume input stream in buffer when buffer end has been reached and input is available -
    current = buffer[offset++];
    return (offset == options.buffer_size);
  }

  inline
  void
  ReaderContext::back() {
    --offset;
  }

  inline
  void
  ReaderContext::enterLevel() {
    ++depth;
    if(options.max_depth != 0 && depth > options.max_depth) [[unlikely]] {
      throwDepthError();
    }
  }

  inline
  void
  ReaderContext::leaveLevel() noexcept {
    --depth;
  }

  // DepthGuard +
  inline
  DepthGuard::DepthGuard(anch::json::ReaderContext& context): _context(context) {
    _context.enterLevel();
  }

  inline
  DepthGuard::~DepthGuard() noexcept {
    _context.leaveLevel();
  }
  // DepthGuard -

}
