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
#ifndef _ANCH_CRYPTO_SHA224_256_H_
#define _ANCH_CRYPTO_SHA224_256_H_

#include "crypto/hash/sha2.hpp"
#include "endianness.hpp"

#include <array>
#include <list>


namespace anch {
  namespace crypto {

    /*! Static translation array */
    std::array<uint32_t,64> TR_CONST = { {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
	0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
	0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
	0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
	0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
	0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
	0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
	0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
	0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
      } };

    /*!
     * \brief SHA2 224/256 implementation.
     *
     * Header only implementation with SHA224 and SHA256 type defintion.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<std::size_t O, const std::array<uint32_t,8>& I>
    class SHA224_256: public SHA2<O,64> {
      /*!
       * SHA224_256 chunk
       *
       * \author Vincent Lachenal
       */
      typedef union {
	/*! Bytes */
	uint8_t bytes[256];

	/*! Words */
	uint32_t words[64];
      } Chunk;

      /*!
       * \ref SHA224_256 context
       *
       * \author Vincent Lachenal
       */
      template<std::size_t Output, const std::array<uint32_t,8>& Init>
      struct Context {
	/*! State */
	std::array<uint32_t,8> state;

	/*! Message size */
	uint64_t size;

	/*! Buffer */
	std::array<uint8_t,64> buffer;

	/*! Digest */
	std::array<uint8_t,Output> digest;

	/*!
	 * \ref SHA224_256 \ref Context constructor
	 */
	Context() {
	  reset();
	}

	/*!
	 * Initialize SHA 224/256 context
	 */
	void reset() {
	  state = Init;
	  size = 0;
	}
      };

      // Attributes +
      /*! \ref SHA224_256 context */
      Context<O,I> _context;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref SHA224_256 default constructor
       */
      SHA224_256();

      /*!
       * \ref SHA224_256 constructor with string
       *
       * \param data The string data to process
       */
      template<class CharT, class Traits, class Allocator>
      SHA224_256(const std::basic_string<CharT,Traits,Allocator>& data) {
	Hash<O,64>::digest(data);
      }

      /*!
       * \ref SHA224_256 constructor with input stream.
       *
       * \param stream The input stream to process
       */
      template<class CharT, class Traits>
      SHA224_256(std::basic_istream<CharT,Traits>& stream) {
	Hash<O,64>::digest(stream);
      }

      /*!
       * \ref SHA224_256 constructor with data bytes.
       *
       * \param data the data bytes
       * \param len the dataa length
       */
      SHA224_256(const uint8_t* data, std::size_t len) {
	Hash<O,64>::digest(data, len);
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref SHA1 destructor
       */
      virtual ~SHA224_256() {
	// Nothing to do
      }
      // Destructor -


      // Methods +
    public:
      /*!
       * Get the SHA 224/256 hash result
       *
       * \return the SHA 224/256 hash result
       */
      virtual const std::array<uint8_t,O>& digest() const override {
	return _context.digest;
      }

    protected:
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
	uint32_t rest = static_cast<uint32_t>(_context.size & static_cast<uint64_t>(63));

	uint64_t availableData = static_cast<uint64_t>(len) + static_cast<uint64_t>(rest);
	_context.size += len;

	if(availableData < static_cast<uint64_t>(64)) {
	  std::memcpy(&_context.buffer[rest], &data[0], len);

	} else {
	  uint64_t i = static_cast<uint64_t>(64 - rest);
	  std::memcpy(&_context.buffer[rest], &data[0], static_cast<uint32_t>(i));
	  transform(_context.buffer.data());

	  uint64_t lastI = len - ((len + rest) & static_cast<uint64_t>(63));
	  for( ; i < lastI ; i += 64) {
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
	uint8_t sizeInBits[8];
	if(anch::isLittleEndian()) {
	  anch::byteSwap(messageSize << 3, sizeInBits);
	} else {
	  uint64_t tmp = messageSize << 3;
	  std::memcpy(sizeInBits, &tmp, 8);
	}

	addData((const uint8_t*)"\200", 1);

	uint8_t zero[64];
	std::memset(zero, 0, 64);
	if(static_cast<int>(messageSize & 63) > 56 - 1) {
	  addData(zero, 64 - 1 - static_cast<size_t>(messageSize & 63));
	  addData(zero, 64 - 8);
	} else {
	  addData(zero, 64 - 1 - 8 - static_cast<size_t>(messageSize & 63));
	}

	addData(sizeInBits, 8);

	if(anch::isLittleEndian()) {
	  for(size_t i = 0 ; i < 8 ; i++) {
	    anch::byteSwap(_context.state[i], _context.digest.data() + (i * 4));
	  }
	} else {
	  std::memcpy(_context.digest.data(),
		      _context.state.data(),
		      sizeof(uint32_t) * O);
	}
      }

    private:
      /*!
       * Swap byte for endianness conversion
       *
       * \param buf The 4-bytes words to process
       * \param count The number of operation to do
       */
      inline void
      bytesSwap(uint32_t* buf, uint8_t count) {
	if(anch::isLittleEndian()) {
	  uint8_t* words = reinterpret_cast<uint8_t*>(buf);
	  do {
	    anch::byteSwap(*buf,words);
	    buf++;
	    words += 4;
	  } while(--count);
	}
      }

      /*!
       * Transform SHA 224/256 with the current chunk
       *
       * \param buffer The data to process
       */
      void transform(const uint8_t* buffer) {
	uint8_t chunkBuffer[256];
	std::memcpy(chunkBuffer, buffer, 256);

	Chunk* chunk = reinterpret_cast<Chunk*>(&chunkBuffer);
	bytesSwap(chunk->words, 64);
	for(int i = 16 ; i < 64 ; i++) {
	  chunk->words[i] = sigma1(chunk->words[i-2]) + chunk->words[i-7]
	    + sigma0(chunk->words[i-15]) + chunk->words[i-16];
	}

	// Copy state[] to working vars
	uint32_t a = _context.state[0];
	uint32_t b = _context.state[1];
	uint32_t c = _context.state[2];
	uint32_t d = _context.state[3];
	uint32_t e = _context.state[4];
	uint32_t f = _context.state[5];
	uint32_t g = _context.state[6];
	uint32_t h = _context.state[7];

	for(int i = 0 ; i < 64 ; i++) {
	  uint32_t temp1 = h + SIGMA1(e) + SHA2<O,64>::ch(e, f, g) + TR_CONST[i] + chunk->words[i];
	  uint32_t temp2 = SIGMA0(a) + SHA2<O,64>::maj(a, b, c);
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
       * Shift 32 bits word from n bits to right
       *
       * \param bits the number of bits to shift
       * \param word the 32 bits word to shift
       */
      static constexpr inline uint32_t shiftRight(uint8_t bits, uint32_t word) {
	return (word >> bits);
      }

      /*!
       * Rotate 32 bits word from n bits to left
       *
       * \param bits the number of bits to rotate
       * \param word the 32 bits word to rotate
       */
      static constexpr inline uint32_t rotateLeft(uint8_t bits, uint32_t word) {
	return ((word << bits) | (word >> (32 - bits)));
      }

      /*!
       * Rotate 32 bits word from n bits to right
       *
       * \param bits the number of bits to rotate
       * \param word the 32 bits word to rotate
       */
      static constexpr inline uint32_t rotateRight(uint8_t bits, uint32_t word) {
	return ((word >> bits) | (word << (32 - bits)));
      }

      /*!
       * SHA224/256 first sigma function
       *
       * \param word the word to transform
       */
      static constexpr inline uint32_t SIGMA0(uint32_t word) {
	return (rotateRight(2, word) ^ rotateRight(13, word) ^ rotateRight(22, word));
      }

      /*!
       * SHA224/256 second sigma function
       *
       * \param word the word to transform
       */
      static constexpr inline uint32_t SIGMA1(uint32_t word) {
	return (rotateRight(6, word) ^ rotateRight(11, word) ^ rotateRight(25, word));
      }

      /*!
       * SHA224/256 first sigma function
       *
       * \param word the word to transform
       */
      static constexpr inline uint32_t sigma0(uint32_t word) {
	return (rotateRight(7, word) ^ rotateRight(18, word) ^ shiftRight(3, word));
      }

      /*!
       * SHA224/256 second sigma function
       *
       * \param word the word to transform
       */
      static constexpr inline uint32_t sigma1(uint32_t word) {
	return (rotateRight(17, word) ^ rotateRight(19, word) ^ shiftRight(10, word));
      }
      // Methods -

    };

    /*! SHA256 initial values */
    std::array<uint32_t,8> SHA256_VALUES = { {
	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
	0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
      } };
    /*!
     * SHA256 defintion
     */
    typedef SHA224_256<32,SHA256_VALUES> SHA256;

    /*! SHA224 initial values */
    std::array<uint32_t,8> SHA224_VALUES = { {
	0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939,
	0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4
      } };
    /*!
     * SHA224 defintion
     */
    typedef SHA224_256<28,SHA224_VALUES> SHA224;

  }
}

#endif // _ANCH_CRYPTO_SHA224_256_H_
