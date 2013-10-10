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
#ifndef _ANCH_CRYPTO_SHA384_512_H_
#define _ANCH_CRYPTO_SHA384_512_H_

#include "crypto/hash/sha2.hpp"
#include "endianness.hpp"

#include <array>
#include <list>


namespace anch {
  namespace crypto {

    /*!
     * \brief SHA2 384/512 implementation.
     *
     * Header only implementation with SHA384 and SHA512 type defintion.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<std::size_t O, const std::array<uint64_t,8>& I>
    class SHA384_512: public SHA2<O,128> {
      /*!
       * SHA384_512 chunk
       *
       * \author Vincent Lachenal
       */
      typedef union {
	/*! Bytes */
	uint8_t bytes[512];

	/*! Words */
	uint64_t words[64];
      } Chunk;

      /*!
       * \ref SHA384_512 context
       *
       * \author Vincent Lachenal
       */
      template<std::size_t Output, const std::array<uint64_t,8>& Init>
      struct Context {
	/*! State */
	std::array<uint64_t,8> state;

	/*! Message size */
	uint64_t size;

	/*! Buffer */
	std::array<uint8_t,128> buffer;

	/*! Digest */
	std::array<uint8_t,Output> digest;

	/*!
	 * \ref SHA384_512 \ref Context constructor
	 */
	Context() {
	  reset();
	}

	/*!
	 * Initialize SHA 384/512 context
	 */
	void reset() {
	  state = Init;
	  size = 0;
	}
      };

      // Attributes +
      /*! \ref SHA384_512 context */
      Context<O,I> _context;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref SHA384_512 default constructor
       */
      SHA384_512();

      /*!
       * \ref SHA384_512 constructor with string
       *
       * \param data The string data to process
       */
      template<class CharT, class Traits, class Allocator>
      SHA384_512(const std::basic_string<CharT,Traits,Allocator>& data) {
	Hash<O,128>::digest(data);
      }

      /*!
       * \ref SHA384_512 constructor with input stream.
       *
       * \param stream The input stream to process
       */
      template<class CharT, class Traits>
      SHA384_512(std::basic_istream<CharT,Traits>& stream) {
	Hash<O,128>::digest(stream);
      }

      /*!
       * \ref SHA384_512 constructor with data bytes.
       *
       * \param data the data bytes
       * \param len the dataa length
       */
      SHA384_512(const uint8_t* data, std::size_t len) {
	Hash<O,128>::digest(data, len);
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref SHA1 destructor
       */
      virtual ~SHA384_512() {
	// Nothing to do
      }
      // Destructor -


      // Methods +
    public:
      /*!
       * Get the SHA 384/512 hash result
       *
       * \return the SHA 384/512 hash result
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
	uint32_t rest = static_cast<uint32_t>(_context.size & static_cast<uint64_t>(127));

	uint64_t availableData = static_cast<uint64_t>(len) + static_cast<uint64_t>(rest);
	_context.size += len;

	if(availableData < static_cast<uint64_t>(128)) {
	  std::memcpy(&_context.buffer[rest], &data[0], len);

	} else {
	  uint64_t i = static_cast<uint64_t>(128 - rest);
	  std::memcpy(&_context.buffer[rest], &data[0], static_cast<uint32_t>(i));
	  transform(_context.buffer.data());

	  uint64_t lastI = len - ((len + rest) & static_cast<uint64_t>(127));
	  for( ; i < lastI ; i += 128) {
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
	uint8_t sizeInBits[16];
	if(anch::isLittleEndian()) {
	  anch::byteSwap(messageSize << 3, sizeInBits);
	} else {
	  uint64_t tmp = messageSize << 3;
	  std::memcpy(sizeInBits, &tmp, 16);
	}

	addData((const uint8_t*)"\200", 1);

	uint8_t zero[128];
	std::memset(zero, 0, 128);
	if(static_cast<int>(messageSize & 127) > 112 - 1) {
	  addData(zero, 128 - 1 - static_cast<size_t>(messageSize & 127));
	  addData(zero, 128 - 8);
	} else {
	  addData(zero, 128 - 1 - 8 - static_cast<size_t>(messageSize & 127));
	}

	addData(sizeInBits, 8);

	if(anch::isLittleEndian()) {
	  for(size_t i = 0 ; i < 8 ; i++) {
	    anch::byteSwap(_context.state[i], _context.digest.data() + (i * 8));
	  }
	} else {
	  std::memcpy(_context.digest.data(),
		      _context.state.data(),
		      sizeof(uint64_t) * O);
	}
      }

    private:
      /*!
       * SHA384/512 translation array getter.
       *
       * \return the SHA384/512 translation array
       */
      static const std::array<uint64_t,80>& getTranslationArray() {
	static std::array<uint64_t,80> trArray = { {
	    0x428A2F98D728AE22, 0x7137449123EF65CD, 0xB5C0FBCFEC4D3B2F, 0xE9B5DBA58189DBBC,
	    0x3956C25BF348B538, 0x59F111F1B605D019, 0x923F82A4AF194F9B, 0xAB1C5ED5DA6D8118,
	    0xD807AA98A3030242, 0x12835B0145706FBE, 0x243185BE4EE4B28C, 0x550C7DC3D5FFB4E2,
	    0x72BE5D74F27B896F, 0x80DEB1FE3B1696B1, 0x9BDC06A725C71235, 0xC19BF174CF692694,
	    0xE49B69C19EF14AD2, 0xEFBE4786384F25E3, 0x0FC19DC68B8CD5B5, 0x240CA1CC77AC9C65,
	    0x2DE92C6F592B0275, 0x4A7484AA6EA6E483, 0x5CB0A9DCBD41FBD4, 0x76F988DA831153B5,
	    0x983E5152EE66DFAB, 0xA831C66D2DB43210, 0xB00327C898FB213F, 0xBF597FC7BEEF0EE4,
	    0xC6E00BF33DA88FC2, 0xD5A79147930AA725, 0x06CA6351E003826F, 0x142929670A0E6E70,
	    0x27B70A8546D22FFC, 0x2E1B21385C26C926, 0x4D2C6DFC5AC42AED, 0x53380D139D95B3DF,
	    0x650A73548BAF63DE, 0x766A0ABB3C77B2A8, 0x81C2C92E47EDAEE6, 0x92722C851482353B, 
	    0xA2BFE8A14CF10364, 0xA81A664BBC423001, 0xC24B8B70D0F89791, 0xC76C51A30654BE30,
	    0xD192E819D6EF5218, 0xD69906245565A910, 0xF40E35855771202A, 0x106AA07032BBD1B8,
	    0x19A4C116B8D2D0C8, 0x1E376C085141AB53, 0x2748774CDF8EEB99, 0x34B0BCB5E19B48A8,
	    0x391C0CB3C5C95A63, 0x4ED8AA4AE3418ACB, 0x5B9CCA4F7763E373, 0x682E6FF3D6B2B8A3,
	    0x748F82EE5DEFB2FC, 0x78A5636F43172F60, 0x84C87814A1F0AB72, 0x8CC702081A6439EC,
	    0x90BEFFFA23631E28, 0xA4506CEBDE82BDE9, 0xBEF9A3F7B2C67915, 0xC67178F2E372532B,
	    0xCA273ECEEA26619C, 0xD186B8C721C0C207, 0xEADA7DD6CDE0EB1E, 0xF57D4F7FEE6ED178,
	    0x06F067AA72176FBA, 0x0A637DC5A2C898A6, 0x113F9804BEF90DAE, 0x1B710B35131C471B,
	    0x28DB77F523047D84, 0x32CAAB7B40C72493, 0x3C9EBE0A15C9BEBC, 0x431D67C49C100D4C,
	    0x4CC5D4BECB3E42B6, 0x597F299CFC657E2A, 0x5FCB6FAB3AD6FAEC, 0x6C44198C4A475817
	  } };
	return trArray;
      }

      /*!
       * Swap byte for endianness conversion
       *
       * \param buf The 4-bytes words to process
       * \param count The number of operation to do
       */
      inline void
      bytesSwap(uint64_t* buf, uint8_t count) {
	if(anch::isLittleEndian()) {
	  uint8_t* words = reinterpret_cast<uint8_t*>(buf);
	  do {
	    anch::byteSwap(*buf,words);
	    buf++;
	    words += 8;
	  } while(--count);
	}
      }

      /*!
       * Transform SHA 384/512 with the current chunk
       *
       * \param buffer The data to process
       */
      void transform(const uint8_t* buffer) {
	uint8_t chunkBuffer[1024];
	std::memcpy(chunkBuffer, buffer, 1024);

	Chunk* chunk = reinterpret_cast<Chunk*>(&chunkBuffer);
	bytesSwap(chunk->words, 128);
	for(int i = 16 ; i < 128 ; i++) {
	  chunk->words[i] = sigma1(chunk->words[i-2]) + chunk->words[i-7]
	    + sigma0(chunk->words[i-15]) + chunk->words[i-16];
	}

	// Copy state[] to working vars
	uint64_t a = _context.state[0];
	uint64_t b = _context.state[1];
	uint64_t c = _context.state[2];
	uint64_t d = _context.state[3];
	uint64_t e = _context.state[4];
	uint64_t f = _context.state[5];
	uint64_t g = _context.state[6];
	uint64_t h = _context.state[7];

	const std::array<uint64_t,80>& trArray = getTranslationArray();
	for(int i = 0 ; i < 80 ; i++) {
	  uint64_t temp1 = h + SIGMA1(e) + SHA2<O,128>::ch(e, f, g) + trArray[i] + chunk->words[i];
	  uint64_t temp2 = SIGMA0(a) + SHA2<O,128>::maj(a, b, c);
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
       * Shift 64 bits word from n bits to right
       *
       * \param bits the number of bits to shift
       * \param word the 64 bits word to shift
       */
      static constexpr inline uint64_t shiftRight(uint8_t bits, uint64_t word) {
	return (word >> bits);
      }

      /*!
       * Rotate 64 bits word from n bits to left
       *
       * \param bits the number of bits to rotate
       * \param word the 64 bits word to rotate
       */
      static constexpr inline uint64_t rotateLeft(uint8_t bits, uint64_t word) {
	return ((word << bits) | (word >> (64 - bits)));
      }

      /*!
       * Rotate 64 bits word from n bits to right
       *
       * \param bits the number of bits to rotate
       * \param word the 64 bits word to rotate
       */
      static constexpr inline uint64_t rotateRight(uint8_t bits, uint64_t word) {
	return ((word >> bits) | (word << (64 - bits)));
      }

      /*!
       * SHA384/512 first sigma function
       *
       * \param word the word to transform
       */
      static constexpr inline uint64_t SIGMA0(uint64_t word) {
	return (rotateRight(28, word) ^ rotateRight(34, word) ^ rotateRight(39, word));
      }

      /*!
       * SHA384/512 second sigma function
       *
       * \param word the word to transform
       */
      static constexpr inline uint64_t SIGMA1(uint64_t word) {
	return (rotateRight(14, word) ^ rotateRight(18, word) ^ rotateRight(41, word));
      }

      /*!
       * SHA384/512 first sigma function
       *
       * \param word the word to transform
       */
      static constexpr inline uint64_t sigma0(uint64_t word) {
	return (rotateRight(1, word) ^ rotateRight(8, word) ^ shiftRight(7, word));
      }

      /*!
       * SHA384/512 second sigma function
       *
       * \param word the word to transform
       */
      static constexpr inline uint64_t sigma1(uint64_t word) {
	return (rotateRight(19, word) ^ rotateRight(61, word) ^ shiftRight(6, word));
      }
      // Methods -

    };

    /*! SHA512 initial values */
    std::array<uint64_t,8> SHA512_VALUES = { {
	0x6A09E667F3BCC908, 0xBB67AE8584CAA73B, 0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
	0x510E527FADE682D1, 0x9B05688C2B3E6C1F, 0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179
      } };
    /*!
     * SHA512 defintion
     */
    typedef SHA384_512<64,SHA512_VALUES> SHA512;

    /*! SHA384 initial values */
    std::array<uint64_t,8> SHA384_VALUES = { {
	0xCBBB9D5DC1059ED8, 0x629A292A367CD507, 0X9159015A3070DD17, 0X152FECD8F70E5939, 
	0x67332667FFC00B31, 0x8EB44A8768581511, 0xDB0C2E0D64F98FA7, 0x47B5481DBEFA4FA4
      } };
    /*!
     * SHA384 defintion
     */
    typedef SHA384_512<48,SHA384_VALUES> SHA384;

  }
}

#endif // _ANCH_CRYPTO_SHA384_512_H_
