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

#include "crypto/hash/hash.hpp"
#include "endianness.hpp"

#include <cstring>

namespace anch {
  namespace crypto {

    /*!
     * \brief SHA2 abstract class.
     *
     * \ref SHA2 contains algorithms for SHA224/256/384/512.\n
     * The children have to define the translation array and the sigma
     * transformation functions.\n
     * The template parameters are:
     * - O: the output size in byte
     * - B: the treatment block size in byte
     * - W: the word type (32 bits or 64 bits)
     * - R: the number of round for each chunk
     * - I: the initial state
     *
     * SHA224/256/384/512 are implemented in this library.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
    class SHA2: public Hash<O,B> {

    protected:
      /*!
       * \ref SHA2 chunk
       *
       * \author Vincent Lachenal
       */
      typedef union {
	/*! Bytes */
	uint8_t bytes[B * sizeof(W)];

	/*! Words */
	W words[B];
      } Chunk;

      /*!
       * \ref SHA2 context
       *
       * \author Vincent Lachenal
       */
      template<const std::array<W,8>& Init>
      struct Context {
	/*! State */
	std::array<W,8> state;

	/*! Message size */
	uint64_t size;

	/*! Buffer */
	uint8_t buffer[B];

	/*! Digest */
	std::array<uint8_t,O> digest;

	/*!
	 * \ref SHA2 \ref Context constructor
	 */
	Context() {
	  reset();
	}

	/*!
	 * Initialize \ref SHA2 context
	 */
	void reset() {
	  state = Init;
	  size = 0;
	}
      };

      // Attributes +
    protected:
      /*! \ref SHA2 context */
      Context<I> _context;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref SHA2 default constructor
       */
      SHA2(): Hash<O,B>(), _context() {
	Hash<O,B>::_digest = &_context.digest;
      }
      // Constructors -

    public:
      // Destructor +
      /*!
       * \ref SHA2 destructor
       */
      virtual ~SHA2() {
	// Nothing to do
      }
      // Destructor -


      // Methods +
    public:
      /*!
       * Get the SHA2 hash result
       *
       * \return the SHA2 hash result
       */
      virtual const std::array<uint8_t,O>& digest() const override {
	return *Hash<O,B>::_digest;
      }

    protected:
      /*!
       * Shift n bits word from n bits to right
       *
       * \param bits the number of bits to shift
       * \param word the n bits word to shift
       */
      static constexpr inline W shiftRight(uint8_t bits, W word) {
	return (word >> bits);
      }

      /*!
       * Rotate n bits word from n bits to left
       *
       * \param bits the number of bits to rotate
       * \param word the n bits word to rotate
       */
      static constexpr inline W rotateLeft(uint8_t bits, W word) {
	return ((word << bits) | (word >> (sizeof(W) * 8 - bits)));
      }

      /*!
       * Rotate n bits word from n bits to right
       *
       * \param bits the number of bits to rotate
       * \param word the n bits word to rotate
       */
      static constexpr inline W rotateRight(uint8_t bits, W word) {
	return ((word >> bits) | (word << (sizeof(W) * 8 - bits)));
      }

    private:
      /*!
       * Reset hash context
       */
      virtual void reset() override {
	_context.reset();
      }

      /*!
       * Compute hash for data with the current hash
       *
       * \param data The data to add
       * \param len The data length
       */
      virtual void addData(const uint8_t* data, std::size_t len) override {
	uint32_t rest = static_cast<uint32_t>(_context.size & static_cast<uint64_t>(B - 1));

	uint64_t availableData = static_cast<uint64_t>(len) + static_cast<uint64_t>(rest);
	_context.size += len;

	if(availableData < static_cast<uint64_t>(B)) {
	  std::memcpy(&_context.buffer[rest], &data[0], len);

	} else {
	  uint64_t i = static_cast<uint64_t>(B - rest);
	  std::memcpy(&_context.buffer[rest], &data[0], static_cast<uint32_t>(i));
	  transform(_context.buffer);

	  uint64_t lastI = len - ((len + rest) & static_cast<uint64_t>(B - 1));
	  for( ; i < lastI ; i += B) {
	    transform(&data[i]);
	  }

	  std::memcpy(&_context.buffer[0], &data[i], len - i);
	}
      }

      /*!
       * Finalize hash
       */
      virtual void finalize() override {
	uint64_t messageSize = _context.size;
	uint8_t sizeInBits[sizeof(W) * 2];
	if(anch::isLittleEndian()) {
	  anch::byteSwap(messageSize << 3, sizeInBits);
	} else {
	  uint64_t tmp = messageSize << 3;
	  std::memcpy(sizeInBits, &tmp, sizeof(W) * 2);
	}

	addData((const uint8_t*)"\200", 1);

	uint8_t zero[B];
	std::memset(zero, 0, B);
	if(static_cast<int>(messageSize & (B - 1)) > (B - sizeof(W) * 2 - 1)) {
	  addData(zero, B - 1 - static_cast<size_t>(messageSize & (B - 1)));
	  addData(zero, B - 8);
	} else {
	  addData(zero, B - 1 - 8 - static_cast<size_t>(messageSize & (B - 1)));
	}

	addData(sizeInBits, 8);

	if(anch::isLittleEndian()) {
	  for(std::size_t i = 0 ; i < 8 ; i++) {
	    anch::byteSwap(_context.state[i], _context.digest.data() + (i * sizeof(W)));
	  }
	} else {
	  std::memcpy(_context.digest.data(),
		      _context.state.data(),
		      sizeof(W) * O);
	}
      }

      /*!
       * \ref SHA2 translation array getter.
       *
       * \return the \ref SHA2 translation array
       */
      virtual const std::array<W,R>& getTranslationArray() const = 0;

      /*!
       * Transform SHA2 with the current chunk
       *
       * \param buffer The data to process
       */
      void transform(const uint8_t* buffer) {
	uint8_t chunkBuffer[B * sizeof(W)];
	std::memcpy(chunkBuffer, buffer, B);

	Chunk* chunk = reinterpret_cast<Chunk*>(&chunkBuffer);
	bytesSwap(chunk->words, B);
	for(std::size_t i = 16 ; i < B ; i++) {
	  chunk->words[i] = sigma1(chunk->words[i-2]) + chunk->words[i-7]
	    + sigma0(chunk->words[i-15]) + chunk->words[i-16];
	}

	// Copy state[] to working vars
	W a = _context.state[0];
	W b = _context.state[1];
	W c = _context.state[2];
	W d = _context.state[3];
	W e = _context.state[4];
	W f = _context.state[5];
	W g = _context.state[6];
	W h = _context.state[7];

	const std::array<W,R>& trArray = getTranslationArray();
	for(uint32_t i = 0 ; i < R ; i++) {
	  W temp1 = h + SIGMA1(e) + ch(e, f, g) + trArray[i] + chunk->words[i];
	  W temp2 = SIGMA0(a) + maj(a, b, c);
	  h = g;
	  g = f;
	  f = e;
	  e = d + temp1;
	  d = c;
	  c = b;
	  b = a;
	  a = temp1 + temp2;
	}

	// Add the working vars back into state
	_context.state[0] += a;
	_context.state[1] += b;
	_context.state[2] += c;
	_context.state[3] += d;
	_context.state[4] += e;
	_context.state[5] += f;
	_context.state[6] += g;
	_context.state[7] += h;
      }

      /*!
       * Swap byte for endianness conversion
       *
       * \param buf The 4-bytes words to process
       * \param count The number of operation to do
       */
      static inline void bytesSwap(W* buf, uint8_t count) {
	if(anch::isLittleEndian()) {
	  uint8_t* words = reinterpret_cast<uint8_t*>(buf);
	  do {
	    anch::byteSwap(*buf,words);
	    buf++;
	    words += sizeof(W);
	  } while(--count);
	}
      }

      /*!
       * Some SHA2 transformation function
       *
       * \param x first value
       * \param y second value
       * \param z third value
       */
      static inline W ch(W x, W y, W z) {
	return ((x & (y ^ z)) ^ z);
      }

      /*!
       * Some SHA2 transformation function
       *
       * \param x first value
       * \param y second value
       * \param z third value
       */
      static inline W maj(W x, W y, W z) {
	return ((x & (y | z)) | (y & z));
      }

      /*!
       * \ref SHA2 first sigma function
       *
       * \param word the word to transform
       */
      virtual W SIGMA0(W word) const = 0;

      /*!
       * \ref SHA2 second sigma function
       *
       * \param word the word to transform
       */
      virtual W SIGMA1(W word) const = 0;

      /*!
       * \ref SHA2 first sigma function
       *
       * \param word the word to transform
       */
      virtual W sigma0(W word) const = 0;

      /*!
       * \ref SHA2 second sigma function
       *
       * \param word the word to transform
       */
      virtual W sigma1(W word) const = 0;
      // Methods -

    };

  }
}
