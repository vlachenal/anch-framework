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
#include "crypto/padding/pkcs5Padding.hpp"

using anch::crypto::PKCS5Padding;


/*!
 * Pad data
 *
 * \param data the data to pad
 * \param len the data length
 * \param expLen the expected length
 */
void
PKCS5Padding::pad(uint8_t* data, std::size_t len, std::size_t expLen) {
  std::size_t diff = expLen - len;
  if(diff > 0) {
    data[expLen - 1] = static_cast<uint8_t>(diff);
    for(std::size_t i = expLen - 2 ; i >= len ; i--) {
      data[i] = static_cast<uint8_t>(diff);
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
PKCS5Padding::length(uint8_t* data, std::size_t len) {
  std::size_t dataLen = len;
  uint8_t nbBytes = data[len - 1];
  if(nbBytes > 1 && nbBytes < len) {
    bool isPadded = true;
    for(std::size_t i = len - 2 ; i > len - nbBytes - 1 ; i--) {
      if(data[i] != nbBytes) {
	isPadded = false;
	break;
      }
    }
    if(isPadded) {
      dataLen = len - nbBytes;
    }

  } else if(nbBytes == 1) {
    dataLen = len - 1;
  }

  return dataLen;
}
