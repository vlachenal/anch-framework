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
#ifndef _ANCH_CRYPTO_BC_MODOP_H_
#define _ANCH_CRYPTO_BC_MODOP_H_

#include "crypto/cipher/invalidBlockException.hpp"

#include <istream>
#include <ostream>
#include <array>
#include <thread>
#include <cstring>

// \todo remove
#include <iostream>


namespace anch {
  namespace crypto {

    /*!
     * \brief Block cipher mode of operation interface
     *
     * Interface for all block cipher mode of operation implementations.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<typename Cipher>
    class BlockCipherModeOfOperation {

      // Attributes +
    private:
      /*! Cipher parallelization flag */
      bool _cipherParallelizable;

      /*! Decipher parallelization flag */
      bool _decipherParallelizable;

      /*! Maximum number of thread to run in parallel */
      unsigned int _nbThread;

      /*! Initialization vector */
      std::array<uint8_t,Cipher::getBlockSize()> _initVector;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref BlockCipherModeOfOperation constructor
       *
       * \param cipherParallelizable Is cipher parallelizable
       * \param decipherParallelizable Is decipher parallelizable
       * \param nbThread the maximum number of thread to run in parallel (default to 1).
       *                 If is set to 0, it will be set to the number of CPU if found (1 otherwise).
       */
      BlockCipherModeOfOperation(bool cipherParallelizable,
				 bool decipherParallelizable,
				 unsigned int nbThread = 1):
	_cipherParallelizable(cipherParallelizable),
	_decipherParallelizable(decipherParallelizable),
	_nbThread(nbThread) {
	if(nbThread == 0 && (cipherParallelizable || decipherParallelizable)) {
	  _nbThread = std::thread::hardware_concurrency();
	  if(_nbThread == 0) { // Keep at least 1 thread running
	    _nbThread = 1;
	  }
	}
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref BlockCipherModeOfOperation destructor
       */
      virtual ~BlockCipherModeOfOperation() {
	// Nothing to do
      }
      // Destructor -


      // Methods +
    public:
      /*!
       * Cipher input stream
       *
       * \param input the input stream to cipher
       * \param output the ouput stream to write in
       */
      void cipher(std::istream& input,
		  std::ostream& output,
		  const std::string& key) {
	if(input && output) {
	  if(!_cipherParallelizable || _nbThread == 1) {
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    char data[Cipher::getBlockSize()];
	    uint8_t out[Cipher::getBlockSize()];
	    while(!input.eof()) {
	      input.read(data, Cipher::getBlockSize());
	      std::streamsize nbRead = input.gcount();
	      if(nbRead < Cipher::getBlockSize()) {
		// \todo manage different padding
		std::cout << "Size: " << nbRead << std::endl;
		std::cout << "Fill " << Cipher::getBlockSize() - nbRead << " 0x00 from index " << nbRead << " to end" << std::endl;
		std::memset(data + nbRead, 0x00, Cipher::getBlockSize() - nbRead);
	      }
	      cipherBlock(reinterpret_cast<uint8_t*>(data), out, cipher);
	      for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; i++) {
		output << out[i];
	      }
	    }
	    output.flush();

	  } else {
	    char data[Cipher::getBlockSize()];
	    while(!input.eof()) {
	      input.read(data, Cipher::getBlockSize());
	      deferredCipherBlock(reinterpret_cast<uint8_t*>(data), input.gcount());
	    }

	  }
	}
      }

      /*!
       * Decipher input stream
       *
       * \param input the input stream to decipher
       * \param output the ouput stream to write in
       */
      void decipher(std::istream& input,
		    std::ostream& output,
		    const std::string& key) {
	if(input && output) {
	  if(!_cipherParallelizable || _nbThread == 1) {
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    uint8_t out[Cipher::getBlockSize()];
	    char data[Cipher::getBlockSize()];
	    char cipherData[Cipher::getBlockSize()];
	    input.read(data, Cipher::getBlockSize());
	    std::streamsize nbRead = input.gcount();
	    do {
	      if(nbRead < Cipher::getBlockSize()) {
		output.flush();
		std::cout << nbRead << std::endl;
		throw InvalidBlockException("Invalid block size"); // \todo set better error message
	      }

	      std::memcpy(cipherData, data, Cipher::getBlockSize());
	      input.read(data, Cipher::getBlockSize());
	      nbRead = input.gcount();

	      decipherBlock(reinterpret_cast<uint8_t*>(cipherData), out, cipher);
	      std::size_t end = Cipher::getBlockSize();
	      if(input.eof()) { // \todo manage different padding algorithms
		std::size_t index;
		for(index = end - 1 ; index >= 0 ; index--) {
		  if(out[index] != 0x00) {
		    index++; // Works only with text files ...
		    break;
		  }
		}
		end = index;
	      }
	      for(std::size_t i = 0 ; i < end ; i++) {
		output << out[i];
	      }

	    } while(!input.eof());
	    output.flush();

	  } else {
	    char data[Cipher::getBlockSize()];
	    while(!input.eof()) {
	      input.read(data, Cipher::getBlockSize());
	      deferredDecipherBlock(reinterpret_cast<uint8_t*>(data), input.gcount());
	    }

	  }
	}
      }

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
			       Cipher& cipher)
	const = 0;

      /*!
       * Decipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to decipher
       * \param output the output block
       */
      virtual void decipherBlock(uint8_t input[Cipher::getBlockSize()],
				 uint8_t output[Cipher::getBlockSize()],
				 Cipher& cipher)
	const = 0;

      /*!
       * Cipher a block
       *
       * \param input the input block to cipher
       */
      virtual void deferredCipherBlock(uint8_t input[Cipher::getBlockSize()], std::streamsize size) const {
	std::cout << input << std::endl;
	std::cout << size << std::endl;
      }

      /*!
       * Decipher a block
       *
       * \param input the input block to decipher
       */
      virtual void deferredDecipherBlock(uint8_t input[Cipher::getBlockSize()], std::streamsize size) const {
	std::cout << input << std::endl;
	std::cout << size << std::endl;
      }
      // Methods -


      // Accessors +
    public:
      /*!
       * Maximum number of parallel thread setter
       *
       * \param nbThread the maximum number of parallel thread to set
       */
      inline void setNbThread(unsigned int nbThread) {
	_nbThread = nbThread;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_CRYPTO_BC_MODOP_H_
