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

      /*! Error flag */
      std::atomic<bool> _error;

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
	_writeIdx(0),
	_error(false) {
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
	  reset();
	  if(!_cipherParallelizable || _nbThread == 1) {
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    std::array<uint8_t, Cipher::getBlockSize()> data;
	    std::array<uint8_t, Cipher::getBlockSize()> out;
	    uint32_t index = 0;
	    while(!input.eof()) {
	      input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	      std::streamsize nbRead = input.gcount();
	      if(nbRead == 0) {
		break;
	      }
	      std::size_t nbBlocks = cipherBlock(data, nbRead, out, index, cipher);
	      for(std::size_t i = 0 ; i < nbBlocks ; ++i) {
		output << out[i];
	      }
	      index++;
	    }
	    output.flush();

	  } else {
	    _endIdx = UINT32_MAX;
	    _writeIdx = 0;
	    _stream = &output;
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    ThreadPool pool(_nbThread);
	    pool.start();
	    std::array<uint8_t,Cipher::getBlockSize()> data;
	    uint32_t index = 0;
	    uint32_t endIdx = 0;
	    while(!input.eof()) {
	      // Add lock to avoid memory consumption on big data +
	      std::unique_lock<std::mutex> readLock(_streamMutex);
	      _writeBlock.wait(readLock, [this,&index]{ return  index < (_writeIdx.load() + _nbThread * 3); });
	      // Add lock to avoid memory consumption on big data -

	      input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	      std::streamsize nbRead = input.gcount();
	      if(nbRead == 0) {
		endIdx = index;
		break;

	      } else if(nbRead < static_cast<std::streamsize>(Cipher::getBlockSize())) {
		endIdx = index + 1;
	      }
	      pool.add(&Derived::deferredCipherBlock, static_cast<Derived*>(this), index, data, nbRead, cipher);
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
	  std::array<uint8_t,Cipher::getBlockSize()> prevData = reset();
	  if(!_decipherParallelizable || _nbThread == 1) {
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    std::array<uint8_t,Cipher::getBlockSize()> out;
	    std::array<uint8_t,Cipher::getBlockSize()> data;
	    std::array<uint8_t,Cipher::getBlockSize()> cipherData;
	    input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	    std::streamsize nbRead = input.gcount();
	    uint32_t index = 0;
	    do {
	      cipherData = data;
	      std::size_t read = nbRead;
	      input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());

	      std::size_t end = decipherBlock(cipherData, prevData, read, input.eof(), out, index, cipher);
	      for(std::size_t i = 0 ; i < end ; ++i) {
	    	output << out[i];
	      }

	      prevData = cipherData;
	      nbRead = input.gcount();
	      index++;

	    } while(nbRead != 0);
	    output.flush();

	  } else {
	    _endIdx = UINT32_MAX;
	    _writeIdx = 0;
	    _stream = &output;
	    Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	    ThreadPool pool(_nbThread);
	    pool.start();
	    std::array<uint8_t,Cipher::getBlockSize()> data;
	    std::array<uint8_t,Cipher::getBlockSize()> cipherData;
	    uint32_t index = 0;
	    input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());
	    std::streamsize nbRead = input.gcount();
	    do {
	      cipherData = data;

	      // Add lock to avoid memory consumption on big data +
	      std::unique_lock<std::mutex> readLock(_streamMutex);
	      _writeBlock.wait(readLock, [this,&index]{ return  index < (_writeIdx.load() + _nbThread * 3); });
	      // Add lock to avoid memory consumption on big data -

	      input.read(reinterpret_cast<char*>(data.data()), Cipher::getBlockSize());

	      if(input.eof()) { // Set end index for padding
		_endIdx = index;
	      }

	      pool.add(&Derived::deferredDecipherBlock, static_cast<Derived*>(this), index, cipherData, prevData, nbRead, cipher);
	      ++index;

	      prevData = cipherData;
	      nbRead = input.gcount();
	    } while(nbRead != 0 && !_error);

	    std::unique_lock<std::mutex> finalizeLock(_streamMutex);
	    _writeBlock.wait(finalizeLock, [this,&index]{ return _writeIdx.load() == index; });

	    if(_error) {
	      throw InvalidBlockException("Error while decipher data");
	    }

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
       * \param nbRead the size of block which has been read
       * \param output the output block
       * \param index the block index
       * \param cipher the cipher instance
       *
       * \return the number of bytes to write
       */
      virtual std::size_t cipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
				      std::streamsize nbRead,
				      std::array<uint8_t,Cipher::getBlockSize()>& output,
				      uint32_t index,
				      Cipher& cipher) = 0;

      /*!
       * Decipher a block.\n
       * This method will handle initialization vector management.
       *
       * \param input the input block to decipher
       * \param prevInput the previous input block to decipher
       * \param nbRead the size of block which has been read
       * \param lastBlock is last block to decipher
       * \param output the output block
       * \param index the block index
       * \param cipher the cipher instance
       *
       * \return the number of bytes to write
       */
      virtual std::size_t decipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
					std::array<uint8_t,Cipher::getBlockSize()>& prevInput,
					std::streamsize nbRead,
					bool lastBlock,
					std::array<uint8_t,Cipher::getBlockSize()>& output,
					uint32_t index,
					Cipher& cipher) = 0;

      /*!
       * Reset block cipher mode of operation context
       *
       * \return the initial context
       */
      virtual const std::array<uint8_t,Cipher::getBlockSize()>& reset() = 0;

      /*!
       * Cipher a block
       *
       * \param input the input block to cipher
       */
      void deferredCipherBlock(uint32_t index,
			       std::array<uint8_t,Cipher::getBlockSize()> input,
			       std::streamsize nbRead,
			       Cipher cipher) {
	std::array<uint8_t,Cipher::getBlockSize()> output;
	std::size_t nbBlocks = cipherBlock(input, nbRead, output, index, cipher);

	std::unique_lock<std::mutex> indexLock(_streamMutex);
	_writeBlock.wait(indexLock, [this,&index]{ return _writeIdx.load() == index; });

	for(std::size_t i = 0 ; i < nbBlocks ; ++i) {
	  *_stream << output[i];
	}

	_writeIdx.store(index + 1);

	indexLock.unlock();
	_writeBlock.notify_all();
      }

      /*!
       * Decipher a block
       *
       * \param input the input block to decipher
       */
      virtual void deferredDecipherBlock(uint32_t index,
					 std::array<uint8_t, Cipher::getBlockSize()> input,
					 std::array<uint8_t, Cipher::getBlockSize()> prevInput,
					 std::streamsize nbRead,
					 Cipher cipher) {
	std::array<uint8_t,Cipher::getBlockSize()> output;
	std::size_t nbBlocks = 0;
	try {
	  nbBlocks = decipherBlock(input, prevInput, nbRead, (index == _endIdx), output, index, cipher);
	} catch(const InvalidBlockException& e) {
	  _error = true;
	}

	std::unique_lock<std::mutex> indexLock(_streamMutex);
	_writeBlock.wait(indexLock, [this,&index]{ return _writeIdx.load() == index; });

	for(std::size_t i = 0 ; i < nbBlocks ; ++i) {
	  *_stream << output[i];
	}

	_writeIdx.store(index + 1);

	indexLock.unlock();
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
