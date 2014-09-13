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
#include <condition_variable>
#include <cstring>
#include <map>

#include "threadPool.hpp"

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
    template<typename Derived, typename Cipher>
    class BlockCipherModeOfOperation {

      friend class anch::ThreadPool;

      // Attributes +
    private:
      /*! Cipher parallelization flag */
      bool _cipherParallelizable;

      /*! Decipher parallelization flag */
      bool _decipherParallelizable;

      /*! Maximum number of thread to run in parallel */
      unsigned int _nbThread;

      /*! Output stream mutex */
      std::mutex _streamMutex;

      /*! Write block condition variable */
      std::condition_variable _writeBlock;

      /*! End index */
      std::atomic<uint32_t> _endIdx;

      /*! Write index */
      std::atomic<uint32_t> _writeIdx;

      /*! Output stream */
      std::ostream* _stream;
      // Attributes -


      // Constructors +
    protected:
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
	_nbThread(nbThread),
	_writeBlock(),
	_endIdx(UINT32_MAX),
	_writeIdx(0) {
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
	  _endIdx = UINT32_MAX;
	  _writeIdx = 0;
	  reset();
	  if(!_cipherParallelizable || _nbThread == 1) {
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    std::array<uint8_t, Cipher::getBlockSize()> data;
	    std::array<uint8_t, Cipher::getBlockSize()> out;
	    while(!input.eof()) {
	      input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	      std::streamsize nbRead = input.gcount();
	      if(nbRead == 0) {
		break;

	      } else if(nbRead < static_cast<std::streamsize>(Cipher::getBlockSize())) {
		// \todo manage different padding
		std::cout << "Size: " << nbRead << std::endl;
		std::cout << "Fill " << Cipher::getBlockSize() - nbRead << " 0x00 from index " << nbRead << " to end" << std::endl;
		std::memset(data.data() + nbRead, 0x00, Cipher::getBlockSize() - nbRead);
	      }
	      cipherBlock(data, out, cipher);
	      for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; i++) {
		output << out[i];
	      }
	    }
	    output.flush();

	  } else {
	    _stream = &output;
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    ThreadPool pool(_nbThread, _nbThread * 3);
	    pool.start();
	    std::array<uint8_t,Cipher::getBlockSize()> data;
	    uint32_t index = 0;
	    uint32_t endIdx = 0;
	    while(!input.eof()) {
	      //std::cout << "Read block n°" << index << std::endl;
	      input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	      std::streamsize nbRead = input.gcount();
	      if(nbRead == 0) {
		endIdx = index;
		std::cout << "TOTOTOTOTO " << _writeIdx << std::endl;
		break;

	      } else if(nbRead < static_cast<std::streamsize>(Cipher::getBlockSize())) {
		endIdx = index + 1;
		// \todo manage different padding
		std::cout << "Size: " << nbRead << std::endl;
		std::cout << "Fill " << Cipher::getBlockSize() - nbRead << " 0x00 from index " << nbRead << " to end" << std::endl;
		std::memset(data.data() + nbRead, 0x00, Cipher::getBlockSize() - nbRead);
		std::cout << "TOTOTOTOTO pad " << _writeIdx << std::endl;
	      }
	      pool.add(&Derived::deferredCipherBlock, static_cast<Derived*>(this), index, data, cipher);
	      ++index;
	    }

	    std::unique_lock<std::mutex> finalizeLock(_streamMutex);
	    _writeBlock.wait(finalizeLock, [this,&endIdx]{ return _writeIdx.load() == endIdx; });

	    pool.stop();
	    output.flush();
	    _stream = NULL;

	    finalizeLock.unlock();
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
	  _endIdx = UINT32_MAX;
	  _writeIdx = 0;
	  reset();
	  if(!_decipherParallelizable || _nbThread == 1) {
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    std::array<uint8_t,Cipher::getBlockSize()> out;
	    std::array<uint8_t,Cipher::getBlockSize()> data;
	    std::array<uint8_t,Cipher::getBlockSize()> cipherData;
	    input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	    std::streamsize nbRead = input.gcount();
	    do {
	      if(nbRead < static_cast<std::streamsize>(Cipher::getBlockSize())) {
		output.flush();
		std::cout << nbRead << std::endl;
		throw InvalidBlockException("Invalid block size"); // \todo set better error message
	      }

	      cipherData = data;
	      input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	      nbRead = input.gcount();

	      decipherBlock(cipherData, out, cipher);
	      std::size_t end = Cipher::getBlockSize();
	      if(input.eof()) { // \todo manage different padding algorithms
		int index;
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
	    _stream = &output;
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    ThreadPool pool(_nbThread, _nbThread * 3);
	    pool.start();
	    std::array<uint8_t,Cipher::getBlockSize()> data;
	    std::array<uint8_t,Cipher::getBlockSize()> cipherData;
	    uint32_t index = 0;
	    input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	    std::streamsize nbRead = input.gcount();
	    do {
	      if(nbRead < static_cast<std::streamsize>(Cipher::getBlockSize())) {
		output.flush();
		std::cout << nbRead << std::endl;
		throw InvalidBlockException("Invalid block size"); // \todo set better error message
	      }

	      cipherData = data;
	      input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	      nbRead = input.gcount();

	      if(input.eof()) { // Set end index for padding
		_endIdx = index;
	      }

	      pool.add(&Derived::deferredDecipherBlock, static_cast<Derived*>(this), index, cipherData, cipher);
	      ++index;

	    } while(!input.eof());

	    std::unique_lock<std::mutex> finalizeLock(_streamMutex);
	    _writeBlock.wait(finalizeLock, [this,&index]{ return _writeIdx.load() == index; });

	    pool.stop();
	    output.flush();
	    _stream = NULL;

	    finalizeLock.unlock();
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
      virtual void cipherBlock(const std::array<uint8_t,Cipher::getBlockSize()>& input,
			       std::array<uint8_t,Cipher::getBlockSize()>& output,
			       Cipher& cipher) = 0;

      /*!
       * Decipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to decipher
       * \param output the output block
       */
      virtual void decipherBlock(const std::array<uint8_t,Cipher::getBlockSize()>& input,
				 std::array<uint8_t,Cipher::getBlockSize()>& output,
				 Cipher& cipher) = 0;

      /*!
       * Reset block cipher mode of operation context
       */
      virtual void reset() = 0;

      /*!
       * Cipher a block
       *
       * \param input the input block to cipher
       */
      void deferredCipherBlock(uint32_t index, std::array<uint8_t,Cipher::getBlockSize()> input, Cipher cipher) {
	std::array<uint8_t,Cipher::getBlockSize()> output;
	cipherBlock(input, output, cipher);

	std::unique_lock<std::mutex> lockIdx(_streamMutex);
	_writeBlock.wait(lockIdx, [this,&index]{ return _writeIdx.load() == index; });

	for(std::size_t i = 0 ; i < Cipher::getBlockSize() ; i++) {
	  *_stream << output[i];
	}

	_writeIdx.store(index + 1);

	lockIdx.unlock();
	_writeBlock.notify_all();
      }

      /*!
       * Decipher a block
       *
       * \param input the input block to decipher
       */
      virtual void deferredDecipherBlock(uint32_t index, std::array<uint8_t,Cipher::getBlockSize()> input, Cipher cipher) {
	std::array<uint8_t,Cipher::getBlockSize()> output;
	decipherBlock(input, output, cipher);

	std::unique_lock<std::mutex> lockIdx(_streamMutex);
	_writeBlock.wait(lockIdx, [this,&index]{ return _writeIdx.load() == index; });

	std::size_t end = Cipher::getBlockSize();
	if(index == _endIdx) { // \todo manage different padding algorithms
	  int idx;
	  for(idx = end - 1 ; idx >= 0 ; idx--) {
	    if(output[idx] != 0x00) {
	      idx++; // Works only with text files ...
	      break;
	    }
	  }
	  end = idx;
	}
	for(std::size_t i = 0 ; i < end ; i++) {
	  *_stream << output[i];
	}

	_writeIdx.store(index + 1);

	lockIdx.unlock();
	_writeBlock.notify_all();
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
