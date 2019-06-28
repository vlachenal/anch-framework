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

#include <sstream>
#include <array>


namespace anch {
  namespace crypto {

    /*!
     * Compute HMAC according to hash algorithm, key and message.\n
     * This function uses C++ string for key and message.
     *
     * \param key the key to use
     * \param message the message to use
     */
    template<typename H>
    H HMAC(const std::string& key, const std::string& message) {
      // Treatment on key +
      std::ostringstream keyBuf;
      const std::size_t block = H::getBlockSize();
      if(key.length() > block) {
	keyBuf << H(key);
      } else {
	keyBuf << key;
      }
      std::array<uint8_t,block> keyArray;
      keyArray.fill(0x00);
      const std::string& buffer = keyBuf.str();
      for(std::size_t i = 0 ; i < buffer.length() ; ++i) {
	keyArray[i] = static_cast<uint8_t>(buffer[i]);
      }
      // Treatment on key -

      // Compute paddings +
      std::size_t msgLen = message.length();
      std::size_t inSize = block + msgLen;
      uint8_t* inPad = new uint8_t[inSize];
      std::array<uint8_t, (block + H::getOutputSize())> outPad;
      // 'Magic numbers' 0x36 and 0x5c are specified in RFC2104 to have a large hamming distance
      for(std::size_t i = 0 ; i < block ; ++i) {
	inPad[i] = static_cast<uint8_t>(0x36) ^ keyArray[i];
	outPad[i] = static_cast<uint8_t>(0x5c) ^ keyArray[i];
      }
      // Compute paddings -

      // First hash +
      const uint8_t* msg = reinterpret_cast<const uint8_t*>(message.c_str());
      for(std::size_t i = 0 ; i < msgLen ; ++i) {
	inPad[i + block] = msg[i];
      }
      const std::array<uint8_t,H::getOutputSize()>& inDigest = H(inPad, inSize).digest();
      delete[] inPad;
      // First hash -

      // Second hash +
      std::size_t idx = block;
      for(uint8_t byte : inDigest) {
	outPad[idx] = byte;
	++idx;
      }
      return H(outPad.data(), outPad.size());
      // Second hash -
    }

  }
}
