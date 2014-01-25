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
#ifndef _ANCH_CRYPTO_CTR_H_
#define _ANCH_CRYPTO_CTR_H_

#include "crypto/cipher/bcModOp.hpp"

namespace anch {
  namespace crypto {

    /*!
     * \brief Counter implementation.
     *
     * CTR mode is also known as ICM (integer counter mode) and SIC (segmented integer counter).
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<typename Cipher>
    class CTR: public BlockCipherModeOfOperation<CTR<Cipher>,Cipher> {

      // Attributes +
    private:
      /*! Initialization vector */
      std::array<uint8_t,Cipher::getBlockSize()> _nonce;

      /*! Context vector */
      std::array<uint8_t,Cipher::getBlockSize()> _ctxtVect;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref CTR constructor
       *
       * \param nonce the nonce
       * \param nbThread the maximum number of thread to run in parallel (default to 1).
       *                 If is set to 0, it will be set to the number of CPU if found (1 otherwise).
       */
      CTR(const std::array<uint8_t,Cipher::getBlockSize()>& nonce, unsigned int nbThread = 1):
	BlockCipherModeOfOperation<CTR<Cipher>,Cipher>(false, false, nbThread),
	_nonce(nonce),
	_ctxtVect() {
	// Nothing to do
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref CTR destructor
       */
      virtual ~CTR() {
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
			       Cipher& cipher) override {
	uint8_t data[Cipher::getBlockSize()];
	cipher.cipher(_ctxtVect.data(), data);
	for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; i++) {
	  output[i] = input[i] ^ data[i];
	  increment();
	}
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
				 Cipher& cipher) override {
	uint8_t data[Cipher::getBlockSize()];
	cipher.cipher(_ctxtVect.data(), data);
	for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; i++) {
	  output[i] = input[i] ^ data[i];
	  increment();
	}
      }

      /*!
       * Reset block cipher mode of operation context
       */
      virtual void reset() {
	_ctxtVect = _nonce;
	uint16_t* counter = reinterpret_cast<uint16_t*>(&_ctxtVect.data()[Cipher::getBlockSize() - 2]);
	*counter = 0;
      }

      /*!
       * Increments context vector counter
       */
      inline void increment() {
	uint16_t* counter = reinterpret_cast<uint16_t*>(&_ctxtVect.data()[Cipher::getBlockSize() - 2]);
	(*counter)++;
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_CTR_H_