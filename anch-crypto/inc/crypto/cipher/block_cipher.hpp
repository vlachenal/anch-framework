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


namespace anch {
  namespace crypto {

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
      virtual void cipher(uint8_t input[B], uint8_t output[B]) = 0;

      /*!
       * Decipher a block
       *
       * \param input the block to decipher
       * \param output the deciphered block
       */
      virtual void decipher(uint8_t input[B], uint8_t output[B]) = 0;
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_BLOCK_CIPHER_H_
