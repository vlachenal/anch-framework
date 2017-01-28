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
    template<typename Cipher, typename Padding>
    class ECB: public BlockCipherModeOfOperation<ECB<Cipher,Padding>,Cipher> {

      // Attributes +
    private:
      /*! Initialization vector */
      std::array<uint8_t,Cipher::getBlockSize()> _initVect;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref ECB constructor
       *
       * \param nbThread the maximum number of thread to run in parallel (default to 1).
       *                 If is set to 0, it will be set to the number of CPU if found (1 otherwise).
       */
      ECB(unsigned int nbThread = 1):
	BlockCipherModeOfOperation<ECB<Cipher,Padding>,Cipher>(true, true, nbThread), _initVect() {
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
       * \param nbRead the size of block which has been read
       * \param output the output block
       * \param cipher the cipher instance
       *
       * \return the number of bytes to write
       */
      virtual std::size_t cipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
				      std::streamsize nbRead,
				      std::array<uint8_t,Cipher::getBlockSize()>& output,
				      uint32_t, Cipher& cipher) override {
	if(static_cast<std::size_t>(nbRead) != Cipher::getBlockSize()) {
	  Padding::pad(input.data(), static_cast<std::size_t>(nbRead), Cipher::getBlockSize());
	}
	cipher.cipher(input, output);
	return Cipher::getBlockSize(); // This mode pad data => the number of bytes to write will always be a complete block
      }

      /*!
       * Decipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to decipher
       * \param nbRead the size of block which has been read
       * \param lastBlock is last block to decipher
       * \param output the output block
       * \param cipher the cipher instance
       *
       * \return the number of bytes to write
       */
      virtual std::size_t decipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
					std::array<uint8_t,Cipher::getBlockSize()>&,
					std::streamsize nbRead,
					bool lastBlock,
					std::array<uint8_t,Cipher::getBlockSize()>& output,
					uint32_t, Cipher& cipher) override {
	if(lastBlock && static_cast<std::size_t>(nbRead) != Cipher::getBlockSize()) {
	  throw InvalidBlockException("Invalid block size");
	}
	cipher.decipher(input, output);
	if(lastBlock) {
	  return Padding::length(output.data(), Cipher::getBlockSize());
	} else {
	  return Cipher::getBlockSize();
	}
      }

      /*!
       * Reset block cipher mode of operation context
       *
       * \return the initial context
       */
      virtual const std::array<uint8_t,Cipher::getBlockSize()>& reset() {
	return _initVect;
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_ECB_H_
