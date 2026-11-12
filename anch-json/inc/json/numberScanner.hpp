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

#include <string_view>

#include "json/readerContext.hpp"

namespace anch::json {

  /*!
   * Check if character can be part of a JSON number (digit, sign, '.', exponent)
   *
   * \param c the character to check
   *
   * \return \c true when character can be part of a number
   */
  bool isNumberChar(char c);

  /*!
   * Check that number characters are a valid JSON number (RFC 8259):
   * \c [-] \c (0|[1-9][0-9]*) \c [.[0-9]+] \c [(e|E)[+|-][0-9]+]
   *
   * \param chars the number characters
   *
   * \return \c true when valid
   */
  bool isJsonNumber(std::string_view chars);

  /*!
   * Scan number characters.\n
   * The first number character has already been read (it is at \c buffer[offset-1] ).
   * When the number is entirely in the reader buffer, the result points directly into it (no copy).
   * Otherwise, characters are copied in context's primitive buffer while the buffer is refilled.\n
   * The context offset is set on the first character which is not a number character.
   *
   * \tparam P the number character predicate type
   *
   * \param context the reader context
   * \param isNumberChar the number character predicate
   *
   * \return the number characters (valid until next buffer refill)
   */
  template<typename P>
  std::string_view scanNumber(anch::json::ReaderContext& context, P isNumberChar);

}

#include "json/impl/numberScanner.hpp"
