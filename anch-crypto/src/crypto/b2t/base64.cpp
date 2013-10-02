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
#include "crypto/b2t/base64.hpp"

using anch::crypto::Base64;

using std::string;
using std::ostringstream;


const char Base64::_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/";
const char Base64::_padding = '=';


// Constructors +
/*!
 * \ref Base64 private constructor
 */
Base64::Base64() {
  // Nothing to do
}
// Constructors -


// Destructor +
/*!
 * \ref Base64 destructor
 */
Base64::~Base64() {
  // Nothing to do
}
// Destructor -


// Encoding methods +
/*!
 * Encode block in base64.
 *
 * \param data the block address to encode
 * \param length the data length
 * \param buffer the buffer to write in
 *
 * \return the number of read bytes
 */
uint8_t
Base64::encode(const uint8_t* data, uint64_t length, char buffer[5]) noexcept {
  uint8_t read = 0;
  buffer[0] = Base64::_alphabet[data[0] >> 2];
  if(length >= 3) {
    read = 3;
    buffer[1] = Base64::_alphabet[((data[0] & 3) << 4) | (data[1] >> 4)];
    buffer[2] = Base64::_alphabet[((data[1] & 0x0F) << 2) | (data[2] >> 6)];
    buffer[3] = Base64::_alphabet[data[2] & 0x3F];

  } else if(length == 1) {
    read = 1;
    buffer[1] = Base64::_alphabet[(data[0] & 3) << 4];
    buffer[2] = Base64::_padding;
    buffer[3] = Base64::_padding;

  } else {
    read = 2;
    buffer[1] = Base64::_alphabet[((data[0] & 3) << 4) | (data[1] >> 4)];
    buffer[2] = Base64::_alphabet[(data[1] & 0x0F) << 2];
    buffer[3] = Base64::_padding;
  }
  return read;
}
// Encoding methods -

// Decoding methods +
/*!
 * Decode block in base64.
 *
 * \param data the block address to decode
 * \param length the data length
 * \param buffer the buffer to write in
 *
 * \return the number of read bytes
 */
void
Base64::decode(const char* data, uint64_t length, uint8_t buffer[4]) noexcept {
  uint8_t current;
  char bytes[4];
  uint8_t read;
  for(read = 0 ; read < 4 && read < length && data[read] != '='; read++) {
    current = data[read];
    if(current >= 'A' && current <= 'Z') {
      bytes[read] = current - 'A';
    } else if(current >= 'a' && current <= 'z') {
      bytes[read] = current + 26 - 'a';
    } else if(current >= '0' && current <= '9') {
      bytes[read] = current + 52 - '0';
    } else if(current == '+') {
      bytes[read] = 62;
    } else if(current == '/') {
      bytes[read] = 63;
    }
  }
  if(read > 0) {
    buffer[0] = (bytes[0] << 2) | (bytes[1] >> 4);
    if(read > 2) {
      buffer[1] = (bytes[1] << 4) | (bytes[2] >> 2);
      if(read > 3) {
	buffer[2] = (bytes[2] << 6) | bytes[3];
      }
    }
  }
}
// Decoding methods -
