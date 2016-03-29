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
#ifndef _ANCH_CRYPTO_BLOCK_CIPHER_H_
#define _ANCH_CRYPTO_BLOCK_CIPHER_H_

#include <stdint.h>
#include <cstddef>
#include <array>


namespace anch {
  namespace crypto {

    /*! Galois multiply by 2 lookup table */
    extern const uint8_t ANCH_GALOIS_MULT2[256];

    /*! Galois multiply by 3 lookup table */
    extern const uint8_t ANCH_GALOIS_MULT3[256];

    /*! Galois multiply by 9 lookup table */
    extern const uint8_t ANCH_GALOIS_MULT9[256];

    /*! Galois multiply by 11 lookup table */
    extern const uint8_t ANCH_GALOIS_MULT11[256];

    /*! Galois multiply by 13 lookup table */
    extern const uint8_t ANCH_GALOIS_MULT13[256];

    /*! Galois multiply by 14 lookup table */
    extern const uint8_t ANCH_GALOIS_MULT14[256];

    /*!
     * \brief Block cipher interface
     *
     * Interface for all block cipher implementations.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<std::size_t B>
    class BlockCipher {

      // Methods +
    public:
      /*!
       * Cipher a block
       *
       * \param input the block to cipher
       * \param output the ciphered block
       */
      virtual void cipher(const std::array<uint8_t,B>& input, std::array<uint8_t,B>& output) = 0;

      /*!
       * Decipher a block
       *
       * \param input the block to decipher
       * \param output the deciphered block
       */
      virtual void decipher(const std::array<uint8_t,B>& input, std::array<uint8_t,B>& output) = 0;

      /*!
       * Block size getter.
       *
       * \return the block size
       */
      constexpr inline static std::size_t getBlockSize() {
	return B;
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_BLOCK_CIPHER_H_
