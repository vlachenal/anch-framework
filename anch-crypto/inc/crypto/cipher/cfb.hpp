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
      CFB(const std::array<uint8_t,Cipher::getBlockSize()>& initVect, unsigned int nbThread = 1);
      // Constructors -


      // Destructor +
      /*!
       * \ref CFB destructor
       */
      virtual ~CFB();
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
				      uint32_t, Cipher& cipher) override;

      /*!
       * Decipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to decipher
       * \param prevInput the previous input block to decipher
       * \param nbRead the size of block which has been read
       * \param output the output block
       * \param cipher the cipher instance
       *
       * \return the number of bytes to write
       */
      virtual std::size_t decipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
					std::array<uint8_t,Cipher::getBlockSize()>& prevInput,
					std::streamsize nbRead,
					bool,
					std::array<uint8_t,Cipher::getBlockSize()>& output,
					uint32_t, Cipher& cipher) override;

      /*!
       * Reset block cipher mode of operation context
       *
       * \return the initial context
       */
      virtual const std::array<uint8_t,Cipher::getBlockSize()>& reset();
      // Methods -

    };

    // Constructors +
    template<typename Cipher>
    CFB<Cipher>::CFB(const std::array<uint8_t,Cipher::getBlockSize()>& initVect, unsigned int nbThread):
      BlockCipherModeOfOperation<CFB<Cipher>,Cipher>(false, true, nbThread),
      _initVect(initVect),
      _ctxtVect() {
      // Nothing to do
    }
    // Constructors -


    // Destructor +
    template<typename Cipher>
    CFB<Cipher>::~CFB() {
      // Nothing to do
    }
    // Destructor -


    // Methods +
    template<typename Cipher>
    std::size_t
    CFB<Cipher>::cipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
			     std::streamsize nbRead,
			     std::array<uint8_t,Cipher::getBlockSize()>& output,
			     uint32_t, Cipher& cipher) {
      std::array<uint8_t,Cipher::getBlockSize()> data;
      cipher.cipher(_ctxtVect, data);
      for(std::size_t i = 0 ; i < static_cast<std::size_t>(nbRead) ; ++i) {
	output[i] = input[i] ^ data[i];
	_ctxtVect[i] = output[i];
      }
      return static_cast<std::size_t>(nbRead);
    }

    template<typename Cipher>
    std::size_t
    CFB<Cipher>::decipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
			       std::array<uint8_t,Cipher::getBlockSize()>& prevInput,
			       std::streamsize nbRead,
			       bool,
			       std::array<uint8_t,Cipher::getBlockSize()>& output,
			       uint32_t, Cipher& cipher) {
      std::array<uint8_t,Cipher::getBlockSize()> data;
      cipher.cipher(prevInput, data);
      for(std::size_t i = 0 ; i < static_cast<std::size_t>(nbRead) ; ++i) {
	output[i] = data[i] ^ input[i];
      }
      return static_cast<std::size_t>(nbRead);
    }

    template<typename Cipher>
    const std::array<uint8_t,Cipher::getBlockSize()>&
    CFB<Cipher>::reset() {
      _ctxtVect = _initVect;
      return _initVect;
    }
    // Methods -

  }
}
