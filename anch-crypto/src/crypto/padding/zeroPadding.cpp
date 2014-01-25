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
#include "crypto/padding/zeroPadding.hpp"

using anch::crypto::ZeroPadding;


/*!
 * Pad data
 *
 * \param data the data to pad
 * \param len the data length
 * \param expLen the expected length
 */
void
ZeroPadding::pad(uint8_t* data, std::size_t len, std::size_t expLen) {
  if(len < expLen) {
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
ZeroPadding::length(uint8_t* data, std::size_t len) {
  std::size_t dataLen = len;
  std::size_t idx = dataLen - 1;
  while(data[idx] == 0x00 && idx > 0) {
    idx--;
  }
  dataLen = idx + 1;
  return dataLen;
}

