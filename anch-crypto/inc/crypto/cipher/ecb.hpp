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
#ifndef _ANCH_CRYPTO_ECB_H_
#define _ANCH_CRYPTO_ECB_H_

#include "crypto/cipher/bcModOp.hpp"


namespace anch {
  namespace crypto {

    /*!
     * \brief Electronic codebook implementation.
     *
     * Block cipher mode of operation simpliest (and weakest) algorithm.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<typename Cipher>
    class ECB: public BlockCipherModeOfOperation<Cipher> {

      // Constructors +
    public:
      /*!
       * \ref ECB constructor
       *
       * \param nbThread the maximum number of thread to run in parallel (default to 1).
       *                 If is set to 0, it will be set to the number of CPU if found (1 otherwise).
       */
      ECB(unsigned int nbThread = 1): BlockCipherModeOfOperation<Cipher>(true, true, nbThread) {
	// Nothing to do
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref ECB destructor
       */
      virtual ~ECB() {
	// Nothing to do
      }
      // Destructor -


      // Methods +
    protected:
      /*!
       * Cipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to cipher
       * \param output the output block
       */
      virtual void cipherBlock(uint8_t input[Cipher::getBlockSize()],
			       uint8_t output[Cipher::getBlockSize()],
			       Cipher& cipher) const {
	cipher.cipher(input, output);
      }

      /*!
       * Decipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to decipher
       * \param output the output block
       */
      virtual void decipherBlock(uint8_t input[Cipher::getBlockSize()],
				 uint8_t output[Cipher::getBlockSize()],
				 Cipher& cipher) const {
	cipher.decipher(input, output);
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_ECB_H_
