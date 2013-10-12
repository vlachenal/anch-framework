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
#ifndef _ANCH_CRYPTO_SHA224_256_H_
#define _ANCH_CRYPTO_SHA224_256_H_

#include "crypto/hash/sha2.hpp"

#include <array>
#include <list>


namespace anch {
  namespace crypto {

    /*!
     * \brief SHA2 224/256 implementation.
     *
     * Header only implementation with SHA224 and SHA256 type defintion.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<std::size_t O, const std::array<uint32_t,8>& I>
    class SHA224_256: public SHA2<O,64,uint32_t,64,I> {

      // Constructors +
    public:
      /*!
       * \ref SHA224_256 default constructor
       */
      SHA224_256(): SHA2<O,64,uint32_t,64,I>() {
	// Nothing to do
      }

      /*!
       * \ref SHA224_256 constructor with string
       *
       * \param data The string data to process
       */
      template<class CharT, class Traits, class Allocator>
      SHA224_256(const std::basic_string<CharT,Traits,Allocator>& data):
	SHA2<O,64,uint32_t,64,I>() {
    	Hash<O,64>::digest(data);
      }

      /*!
       * \ref SHA224_256 constructor with input stream.
       *
       * \param stream The input stream to process
       */
      template<class CharT, class Traits>
      SHA224_256(std::basic_istream<CharT,Traits>& stream):
	SHA2<O,64,uint32_t,64,I>() {
    	Hash<O,64>::digest(stream);
      }

      /*!
       * \ref SHA224_256 constructor with data bytes.
       *
       * \param data the data bytes
       * \param len the dataa length
       */
      SHA224_256(const uint8_t* data, std::size_t len):
	SHA2<O,64,uint32_t,64,I>() {
    	Hash<O,64>::digest(data, len);
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref SHA224_256 destructor
       */
      virtual ~SHA224_256() {
    	// Nothing to do
      }
      // Destructor -

      // Methods +
    private:
      /*!
       * SHA224/256 translation array getter.
       *
       * \return the SHA224/256 translation array
       */
      virtual const std::array<uint32_t,64>& getTranslationArray() const {
	static std::array<uint32_t,64> trArray = { {
	    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
	    0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
	    0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
	    0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
	    0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
	    0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
	    0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
	    0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
	    0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	  } };
	return trArray;
      }

      /*!
       * SHA224/256 first sigma function
       *
       * \param word the word to transform
       */
      virtual uint32_t SIGMA0(uint32_t word) const {
	return (SHA2<O,64,uint32_t,64,I>::rotateRight(2, word)
		^ SHA2<O,64,uint32_t,64,I>::rotateRight(13, word)
		^ SHA2<O,64,uint32_t,64,I>::rotateRight(22, word));
      }

      /*!
       * SHA224/256 second sigma function
       *
       * \param word the word to transform
       */
      virtual uint32_t SIGMA1(uint32_t word) const {
	return (SHA2<O,64,uint32_t,64,I>::rotateRight(6, word)
		^ SHA2<O,64,uint32_t,64,I>::rotateRight(11, word)
		^ SHA2<O,64,uint32_t,64,I>::rotateRight(25, word));
      }

      /*!
       * SHA224/256 first sigma function
       *
       * \param word the word to transform
       */
      virtual uint32_t sigma0(uint32_t word) const {
	return (SHA2<O,64,uint32_t,64,I>::rotateRight(7, word)
		^ SHA2<O,64,uint32_t,64,I>::rotateRight(18, word)
		^ SHA2<O,64,uint32_t,64,I>::shiftRight(3, word));
      }

      /*!
       * SHA224/256 second sigma function
       *
       * \param word the word to transform
       */
      virtual uint32_t sigma1(uint32_t word) const {
	return (SHA2<O,64,uint32_t,64,I>::rotateRight(17, word)
		^ SHA2<O,64,uint32_t,64,I>::rotateRight(19, word)
		^ SHA2<O,64,uint32_t,64,I>::shiftRight(10, word));
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_SHA224_256_H_
