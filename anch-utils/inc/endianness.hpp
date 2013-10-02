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
#ifndef _ANCH_UTILS_ENDIANNESS_H_
#define _ANCH_UTILS_ENDIANNESS_H_

#include <cstddef>


namespace anch {

  /*!
   * Switch data endianness.
   *
   * \param src the object to convert
   * \param dest the converted bytes
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  inline void byteSwap(T src, uint8_t* dest) {
    const uint8_t* words = reinterpret_cast<const uint8_t*>(&src);
    std::size_t size = sizeof(T);
    for(std::size_t i = 0 ; i < size ; i++) {
      dest[i] = words[size - 1 - i];
    }
  }

  /*!
   * Check if system is in big endian bytes order.
   *
   * \return \c true if system is big endian, \c false otherwise
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  inline bool isBigEndian() {
    const uint32_t byteOrderTest = 0x01;
    return (reinterpret_cast<const uint8_t*>(&byteOrderTest)[0] == 0);
  }

  /*!
   * Check if system is in little endian bytes order.
   *
   * \return \c true if system is little endian, \c false otherwise
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  inline bool isLittleEndian() {
    return !isBigEndian();
  }

}

#endif // _ANCH_UTILS_ENDIANNESS_H_
