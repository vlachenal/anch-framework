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
#ifndef _ANCH_CRYPTO_CFB_H_
#define _ANCH_CRYPTO_CFB_H_

#include "crypto/cipher/bcModOp.hpp"

namespace anch {
  namespace crypto {

    /*!
     * \brief Cipher feedback implementation.
     *
     * CFB messages do not need to be padded.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<typename Cipher>
    class CFB: public BlockCipherModeOfOperation<CFB<Cipher>,Cipher> {

      // Attributes +
    private:
      /*! Initialization vector */
      std::array<uint8_t,Cipher::getBlockSize()> _initVect;

      /*! Context vector */
      std::array<uint8_t,Cipher::getBlockSize()> _ctxtVect;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref CFB constructor
       *
       * \param initVect the initialization vector
       * \param nbThread the maximum number of thread to run in parallel (default to 1).
       *                 If is set to 0, it will be set to the number of CPU if found (1 otherwise).
       */
      CFB(const std::array<uint8_t,Cipher::getBlockSize()>& initVect, unsigned int nbThread = 1):
	BlockCipherModeOfOperation<CFB<Cipher>,Cipher>(false, true, nbThread),
	_initVect(initVect),
	_ctxtVect() {
	// Nothing to do
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref CFB destructor
       */
      virtual ~CFB() {
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
      virtual void cipherBlock(const std::array<uint8_t,Cipher::getBlockSize()>& input,
			       std::array<uint8_t,Cipher::getBlockSize()>& output,
			       Cipher& cipher) override {
	std::array<uint8_t,Cipher::getBlockSize()> data;
	cipher.cipher(_ctxtVect, data);
	for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; i++) {
	  output[i] = input[i] ^ data[i];
	  _ctxtVect[i] = output[i];
	}
      }

      /*!
       * Decipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to decipher
       * \param output the output block
       */
      virtual void decipherBlock(const std::array<uint8_t,Cipher::getBlockSize()>& input,
				 std::array<uint8_t,Cipher::getBlockSize()>& output,
				 Cipher& cipher) override {
	std::array<uint8_t,Cipher::getBlockSize()> data;
	cipher.cipher(_ctxtVect, data);
	for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; i++) {
	  output[i] = data[i] ^ input[i];
	  _ctxtVect[i] = input[i];
	}
      }

      /*!
       * Reset block cipher mode of operation context
       */
      virtual void reset() {
	_ctxtVect = _initVect;
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_CFB_H_
