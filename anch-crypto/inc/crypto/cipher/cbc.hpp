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
#ifndef _ANCH_CRYPTO_CBC_H_
#define _ANCH_CRYPTO_CBC_H_

#include "crypto/cipher/bcModOp.hpp"

namespace anch {
  namespace crypto {

    /*!
     * \brief Cipher-block chaining implementation.
     *
     * CBC is the most commonly used mode of operation
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<typename Cipher, typename Padding>
    class CBC: public BlockCipherModeOfOperation<CBC<Cipher,Padding>,Cipher> {

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
       * \ref CBC constructor
       *
       * \param initVect the initialization vector
       * \param nbThread the maximum number of thread to run in parallel (default to 1).
       *                 If is set to 0, it will be set to the number of CPU if found (1 otherwise).
       */
      CBC(const std::array<uint8_t,Cipher::getBlockSize()>& initVect, unsigned int nbThread = 1):
	BlockCipherModeOfOperation<CBC<Cipher,Padding>,Cipher>(false, true, nbThread),
	_initVect(initVect),
	_ctxtVect() {
	// Nothing to do
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref CBC destructor
       */
      virtual ~CBC() {
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
	std::array<uint8_t,Cipher::getBlockSize()> data;
	for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; ++i) {
	  data[i] = input[i] ^ _ctxtVect[i];
	}
	cipher.cipher(data, output);
	for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; ++i) {
	  _ctxtVect[i] = output[i];
	}
	return Cipher::getBlockSize(); // This mode pad data => the number of bytes to write will always be a complete block
      }

      /*!
       * Decipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to decipher
       * \param prevInput the previous input block to decipher
       * \param nbRead the size of block which has been read
       * \param lastBlock is last block to decipher
       * \param output the output block
       * \param cipher the cipher instance
       *
       * \return the number of bytes to write
       */
      virtual std::size_t decipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
					std::array<uint8_t,Cipher::getBlockSize()>& prevInput,
					std::streamsize nbRead,
					bool lastBlock,
					std::array<uint8_t,Cipher::getBlockSize()>& output,
					uint32_t, Cipher& cipher) override {
	if(lastBlock && static_cast<std::size_t>(nbRead) != Cipher::getBlockSize()) {
	  throw InvalidBlockException("Invalid block size");
	}
	std::array<uint8_t,Cipher::getBlockSize()> data;
	cipher.decipher(input, data);
	for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; ++i) {
	  output[i] = data[i] ^ prevInput[i];
	}
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
	_ctxtVect = _initVect;
	return _initVect;
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_CBC_H_
