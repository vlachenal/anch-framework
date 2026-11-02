/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <bit>
#include <concepts>
#include <cstdint>


namespace anch {

  /*!
   * Switch data endianness.\n
   * Write the \c sizeof(T) bytes of \c src in reverse order into \c dest (\c dest can overlap \c src storage).
   *
   * \param src the integer to convert
   * \param dest the converted bytes (at least \c sizeof(T) bytes)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<std::integral T>
  void byteSwap(T src, uint8_t* dest) noexcept;

  /*!
   * Check if system is in big endian bytes order.
   *
   * \return \c true if system is big endian, \c false otherwise
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  constexpr bool isBigEndian() noexcept;

  /*!
   * Check if system is in little endian bytes order.
   *
   * \return \c true if system is little endian, \c false otherwise
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  constexpr bool isLittleEndian() noexcept;

}

#include "impl/endianness.hpp"
