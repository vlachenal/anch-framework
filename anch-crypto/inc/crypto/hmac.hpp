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
#ifndef _ANCH_CRYPTO_HMAC_
#define _ANCH_CRYPTO_HMAC_

#include "crypto/hash/hash.hpp"


namespace anch {
  namespace crypto {

    /*!
     * Compute HMAC according to hash algorithm, key and message.\n
     * This function uses C++ string for key and message.
     *
     * \param key the key to use
     * \param message the message to use
     */
    template<class H, class CharT, class Traits, class Allocator>
    H HMAC(const std::basic_string<CharT,Traits,Allocator>& key,
	   const std::basic_string<CharT,Traits,Allocator>& message) {
      // Treatment on key +
      std::basic_ostringstream<CharT,Traits,Allocator> keyBuf;
      const std::size_t block = H::getBlockSize();
      if(key.length() > block) {
	keyBuf << H(key);
      } else {
	keyBuf << key;
      }
      std::array<uint8_t,block> keyArray;
      keyArray.fill(0x00);
      const std::basic_string<CharT,Traits,Allocator>& buffer = keyBuf.str();
      for(std::size_t i = 0 ; i < buffer.length() ; i++) {
	keyArray[i] = static_cast<uint8_t>(buffer[i]);
      }
      // Treatment on key -
      // Compute paddings +
      std::basic_ostringstream<CharT,Traits,Allocator> inPad;
      std::basic_ostringstream<CharT,Traits,Allocator> outPad;
      // 'Magic numbers' 0x36 and 0x5c are specified in RFC2104 to have a large hamming distance
      for(std::size_t i = 0 ; i < block ; i++) {
	inPad << static_cast<const char>(static_cast<uint8_t>(0x36) ^ keyArray[i]);
	outPad << static_cast<const char>(static_cast<uint8_t>(0x5c) ^ keyArray[i]);
      }
      // Compute paddings -
      // First hash +
      inPad << message;
      // First hash -
      // Second hash +
      const std::array<uint8_t,H::getOutputSize()>& inDigest = H(inPad.str()).digest();
      for(uint8_t byte : inDigest) {
	outPad << static_cast<char>(byte);
      }
      return H(outPad.str());
      // Second hash -
    }

    /*!
     * Compute HMAC according to hash algorithm, key and message.\n
     * This function uses characters for key and message.
     *
     * \param key the key to use
     * \param message the message to use
     */
    template<class H>
    H HMAC(const char* const key, const char* const message) {
      return HMAC<H>(std::string(key), std::string(message));
    }

  }
}

#endif // _ANCH_CRYPTO_HMAC_
