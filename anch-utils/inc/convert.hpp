/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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

#include <concepts>
#include <expected>
#include <system_error>
#include <string_view>
#include <typeinfo>


namespace anch {

  /*!
   * Number type which can be converted by \ref anch::convert "convert" (any integer or floating point type except \c bool).
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename N>
  concept Convertible = (std::integral<N> && !std::same_as<N, bool>) || std::floating_point<N>;

  /*!
   * Performs strict conversion from string to number.\n
   * \n
   * The whole string has to be a number: leading or trailing characters (including spaces and \c '+' sign) are rejected.\n
   * The conversion does not depend on locale. Negative values are rejected for unsigned types and out of range
   * values are rejected. \c int8_t and \c uint8_t are converted as numbers (not as characters).\n
   * Floating point values can be written in fixed or scientific notation, and can be \c inf or \c nan.
   *
   * \param str the string to convert
   *
   * \return the number
   *
   * \throw std::bad_cast conversion error
   */
  template<anch::Convertible N>
  N convert(std::string_view str);

  /*!
   * Performs strict conversion from string to number without exception.\n
   * \n
   * Same rules as \ref anch::convert "convert": the whole string has to be a number (no leading or trailing
   * characters, no space, no \c '+' sign), conversion does not depend on locale, negative values are rejected for
   * unsigned types.
   *
   * \param str the string to convert
   *
   * \return the number, or \c std::errc::invalid_argument (empty string, not a number, leading or trailing
   *         characters) or \c std::errc::result_out_of_range (value out of type range)
   *
   * \since 0.3
   */
  template<anch::Convertible N>
  std::expected<N, std::errc> tryConvert(std::string_view str) noexcept;

}

#include "impl/convert.hpp"
