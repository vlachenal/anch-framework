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
       * \param key the cipher key
       */
      void cipher(std::istream& input,
		  std::ostream& output,
		  const std::string& key) {
	if(input && output) {
	  reset();
	  if(!_cipherParallelizable || _nbThread == 1) {
	    cipherSequentially(input, output, key);

	  } else {
	    cipherInParallel(input, output, key);
	  }
	} else {
	  // error
	}
      }

      /*!
       * Decipher input stream
       *
       * \param input the input stream to decipher
       * \param output the ouput stream to write in
       * \param key the decipher key
       */
      void decipher(std::istream& input,
		    std::ostream& output,
		    const std::string& key) {
	if(input && output) {
	  std::array<uint8_t,Cipher::getBlockSize()> prevData = reset();
	  if(!_decipherParallelizable || _nbThread == 1) {
	    decipherSequentially(input, output, key, prevData);

	  } else {
	    decipherInParallel(input, output, key, prevData);
	  }
	} else {
	  // error
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

    private:
      /*!
       * Cipher input stream into output stream sequentially
       *
       * \param input the input stream
       * \param output the output stream
       * \param key the cipher key
       */
      inline void cipherSequentially(std::istream& input,
				     std::ostream& output,
				     const std::string& key) {
	Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	std::array<uint8_t, Cipher::getBlockSize()> data;
	std::array<uint8_t, Cipher::getBlockSize()> out;
	uint32_t index = 0;
	while(!input.eof()) {
	  input.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(Cipher::getBlockSize()));
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
      }

      /*!
       * Cipher input stream into output stream in parallel according to configuration put in constructor.
       *
       * \param input the input stream
       * \param output the output stream
       * \param key the cipher key
       */
      inline void cipherInParallel(std::istream& input,
				   std::ostream& output,
				   const std::string& key) {
	std::array<uint8_t,Cipher::getBlockSize()>* data = new std::array<uint8_t,Cipher::getBlockSize()>[_nbThread];
	std::array<uint8_t,Cipher::getBlockSize()>* result = new std::array<uint8_t,Cipher::getBlockSize()>[_nbThread];
	std::size_t* nbWrite = new std::size_t[_nbThread];
	uint32_t index = 0;
	std::deque<std::thread> threads;
	std::vector<Cipher> ciph;
	ciph.push_back(Cipher(reinterpret_cast<const uint8_t*>(key.data())));
	for(std::size_t i = 1 ; i < _nbThread ; ++i) {
	  ciph.push_back(Cipher(ciph[0]));
	}
	while(!input.eof()) {
	  for(std::size_t i = 0 ; i < _nbThread ; ++i) {
	    input.read(reinterpret_cast<char*>(data[i].data()), static_cast<std::streamsize>(Cipher::getBlockSize()));
	    std::streamsize nbRead = input.gcount();
	    if(nbRead == 0) {
	      break;
	    }
	    threads.push_back(std::thread(&Derived::deferredCipherBlock,
					  this,
					  index++,
					  std::ref(data[i]),
					  nbRead,
					  std::ref(result[i]),
					  std::ref(ciph[i]),
					  std::ref(nbWrite[i])));
	    if(static_cast<std::size_t>(nbRead) != Cipher::getBlockSize()) {
	      break;
	    }
	  }
	  for(uint32_t i = 0 ; !threads.empty() ; ++i) {
	    std::thread& th = threads.front();
	    th.join();
	    for(uint32_t j = 0 ; j < nbWrite[i] ; ++j) {
	      output << result[i][j];
	    }
	    threads.pop_front();
	  }
	}

	output.flush();
	delete[] data;
	delete[] result;
	delete[] nbWrite;
      }

      /*!
       * Decipher input stream into output stream sequentially
       *
       * \param input the input stream
       * \param output the output stream
       * \param key the cipher key
       * \param prevData the previous data
       */
      void decipherSequentially(std::istream& input,
				std::ostream& output,
				const std::string& key,
				std::array<uint8_t,Cipher::getBlockSize()>& prevData) {
	Cipher cipher(reinterpret_cast<const uint8_t*>(key.data()));
	std::array<uint8_t,Cipher::getBlockSize()> out;
	std::array<uint8_t,Cipher::getBlockSize()> data;
	std::array<uint8_t,Cipher::getBlockSize()> cipherData;
	input.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(Cipher::getBlockSize()));
	std::streamsize nbRead = input.gcount();
	uint32_t index = 0;
	do {
	  cipherData = data;
	  std::streamsize read = nbRead;
	  input.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(Cipher::getBlockSize()));

	  std::size_t end = decipherBlock(cipherData, prevData, read, input.eof(), out, index, cipher);
	  for(std::size_t i = 0 ; i < end ; ++i) {
	    output << out[i];
	  }

	  prevData = cipherData;
	  nbRead = input.gcount();
	  index++;

	} while(nbRead != 0);
	output.flush();
      }

      /*!
       * Decipher input stream into output stream in parallel according to configuration put in constructor.
       *
       * \param input the input stream
       * \param output the output stream
       * \param key the cipher key
       * \param prevData the previous data
       */
      inline void decipherInParallel(std::istream& input,
				     std::ostream& output,
				     const std::string& key,
				     std::array<uint8_t,Cipher::getBlockSize()>& prevData) {
	std::array<uint8_t,Cipher::getBlockSize()>* data = new std::array<uint8_t,Cipher::getBlockSize()>[_nbThread + 1];
	std::array<uint8_t,Cipher::getBlockSize()>* result = new std::array<uint8_t,Cipher::getBlockSize()>[_nbThread];
	std::size_t* nbWrite = new std::size_t[_nbThread];
	uint32_t index = 0;
	std::deque<std::thread> threads;
	std::vector<Cipher> ciph;
	ciph.push_back(Cipher(reinterpret_cast<const uint8_t*>(key.data())));
	for(std::size_t i = 1 ; i < _nbThread ; ++i) {
	  ciph.push_back(Cipher(ciph[0]));
	}
	bool lastBlock = false;
	input.read(reinterpret_cast<char*>(data[0].data()), static_cast<std::streamsize>(Cipher::getBlockSize()));
	std::size_t nbRead = static_cast<std::size_t>(input.gcount());
	do {
	  for(std::size_t i = 0, idx = 1 ; i < _nbThread ; ++i, ++idx) {
	    input.read(reinterpret_cast<char*>(data[idx].data()), static_cast<std::streamsize>(Cipher::getBlockSize()));
	    std::streamsize nextCount = input.gcount();
	    if(nextCount == 0) {
	      lastBlock = true;
	    }
	    threads.push_back(std::thread(&Derived::deferredDecipherBlock,
					  this,
					  index++,
					  std::ref(data[i]),
					  prevData,
					  nbRead,
					  lastBlock,
					  std::ref(result[i]),
					  std::ref(ciph[i]),
					  std::ref(nbWrite[i])));
	    if(lastBlock) {
	      break;
	    }
	    nbRead = static_cast<std::size_t>(nextCount);
	    prevData = data[i];
	  }
	  for(uint32_t i = 0 ; !threads.empty() ; ++i) {
	    std::thread& th = threads.front();
	    th.join();
	    for(uint32_t j = 0 ; j < nbWrite[i] ; ++j) {
	      output << result[i][j];
	    }
	    threads.pop_front();
	  }
	  if(_error) {
	    throw InvalidBlockException("Error while decipher stream");
	  }
	  data[0] = data[_nbThread];
	} while(!lastBlock);

	output.flush();
	delete[] data;
	delete[] result;
	delete[] nbWrite;
      }

      /*!
       * Cipher a block
       *
       * \param index the block index
       * \param input the input block to cipher
       * \param nbRead the number of bytes which have been read
       * \param cipher the cipher instance to use
       */
      void deferredCipherBlock(uint32_t index,
			       std::array<uint8_t,Cipher::getBlockSize()>& input,
			       std::streamsize nbRead,
			       std::array<uint8_t,Cipher::getBlockSize()>& output,
			       Cipher& cipher,
			       std::size_t& nbWrite) {
	nbWrite = cipherBlock(input, nbRead, output, index, cipher);
      }

      /*!
       * Decipher a block
       *
       * \param index the block index
       * \param input the input block to cipher
       * \param prevInput the previous input
       * \param nbRead the number of bytes which have been read
       * \param cipher the cipher instance to use
       */
      virtual void deferredDecipherBlock(uint32_t index,
					 std::array<uint8_t, Cipher::getBlockSize()>& input,
					 std::array<uint8_t, Cipher::getBlockSize()> prevInput,
					 std::streamsize nbRead,
					 bool lastBlock,
					 std::array<uint8_t, Cipher::getBlockSize()>& output,
					 Cipher& cipher,
					 std::size_t& nbWrite) {
	try {
	  nbWrite = decipherBlock(input, prevInput, nbRead, lastBlock, output, index, cipher);
	} catch(const InvalidBlockException& e) {
	  _error = true;
	}
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
