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
#include "crypto/padding/iso7816_4Padding.hpp"

using anch::crypto::ISO7816_4Padding;


/*!
 * Pad data
 *
 * \param data the data to pad
 * \param len the data length
 * \param expLen the expected length
 */
void
ISO7816_4Padding::pad(uint8_t* data, std::size_t len, std::size_t expLen) {
  if(len < expLen) {
    data[len - 1] = 0x80;
    for(std::size_t idx = len ; idx < expLen ; idx++) {
      data[idx] = 0x00;
    }
  }
}

/*!
 * Unpad data
 *
 * \param data the data to unpad
 * \param len the block length
 *
 * \return the data length
 */
std::size_t
ISO7816_4Padding::length(uint8_t* data, std::size_t len) {
  std::size_t dataLen = len;
  for(std::size_t i = len - 1 ; i >= 0 ; i--) {
    if(data[i] == 0x80) {
      dataLen = i + i;
      break;
    } else if(data[i] != 0x00) {
      break;
    }
  }
  return dataLen;
}
