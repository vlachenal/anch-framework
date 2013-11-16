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
#ifndef _ANCH_CRYPTO_SHA1_H_
#define _ANCH_CRYPTO_SHA1_H_

#include "crypto/hash/hash.hpp"


namespace anch {
  namespace crypto {

    /*!
     * SHA1 hash algorithm implementation\n
     * This algorithm is based on Qt framework algorithm.
     *
     * \author Vincent Lachenal
     */
    class SHA1: public Hash<20,64> {
    private:
      /*!
       * SHA1 chunk
       *
       * \author Vincent Lachenal
       */
      typedef union {
	/*! Bytes */
	uint8_t bytes[64];

	/*! Words */
	uint32_t words[16];
      } Chunk;

      /*!
       * SHA1 context
       *
       * \author Vincent Lachenal
       */
      struct Context {
	/*! State */
	std::array<uint32_t,5> state;

	/*! Message size */
	uint64_t size;

	/*! Buffer */
	std::array<uint8_t,64> buffer;

	/*! Digest */
	std::array<uint8_t,20> digest;

	/*!
	 * \ref SHA1 \ref Context constructor
	 */
	Context() {
	  reset();
	}

	/*!
	 * Initialize SHA1 context
	 */
	void reset() {
	  state = { {0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476,0xC3D2E1F0} };
	  size = 0;
	}
      };

      // Attributes +
    private:
      /*! SHA1 context */
      Context _context;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref SHA1 default constructor
       */
      SHA1();

      /*!
       * \ref SHA1 constructor with string
       *
       * \param data The string data to process
       */
      template<class CharT, class Traits, class Allocator>
      SHA1(const std::basic_string<CharT,Traits,Allocator>& data) {
	Hash::digest(data);
      }

      /*!
       * \ref SHA1 constructor with input stream.
       *
       * \param stream The input stream to process
       */
      template<class CharT, class Traits>
      SHA1(std::basic_istream<CharT,Traits>& stream) {
	Hash::digest(stream);
      }

      /*!
       * \ref SHA1 constructor with data bytes.
       *
       * \param data the data bytes
       * \param len the data length
       */
      SHA1(const uint8_t* data, std::size_t len) {
	Hash::digest(data, len);
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref SHA1 destructor
       */
      virtual ~SHA1();
      // Destructor -


      // Methods +
    public:
      /*!
       * Get the SHA1 hash result
       *
       * \return the SHA1 hash result
       */
      virtual const std::array<uint8_t,20>& digest() const override;

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
      virtual void addData(const uint8_t* data, std::size_t len) override;

      /*!
       * Finalize hash
       */
      virtual void finalize() override;

    private:
      /*!
       * Transform SHA1 with the current chunk
       *
       * \param buffer The data to process
       */
      void transform(const uint8_t* buffer);

      /*!
       * Bits rotation of 32 bits integer from a value
       *
       * \param value The integer to process
       * \param shift The number of bits to shift
       */
      static inline uint32_t rol32(uint32_t value, unsigned int shift){
	return ((value << shift) | value >> (32 - shift));
      }

      /*!
       * Compute SHA1 word
       *
       * \param chunk The SHA1 chunk
       * \param position The position
       */
      static inline uint32_t word(Chunk& chunk, unsigned int position) {
	return (chunk.words[position & 0xf] = rol32(chunk.words[(position + 13)  & 0xf]
						    ^ chunk.words[(position + 8) & 0xf]
						    ^ chunk.words[(position + 2) & 0xf]
						    ^ chunk.words[(position)     & 0xf],
						    1));
      }

      /*!
       * First core function
       *
       * \param chunk The chunk to process
       * \param position The position
       * \param v The first parameter
       * \param w The second parameter
       * \param x The third parameter
       * \param y The fourth parameter
       * \param z The fifth parameter
       */
      static inline void round0(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z){
	z += ((( w & (x ^ y)) ^ y) + chunk.words[position] + 0x5A827999 + rol32(v, 5));
	w = rol32(w, 30);
      }

      /*!
       * Second core function
       *
       * \param chunk The chunk to process
       * \param position The position
       * \param v The first parameter
       * \param w The second parameter
       * \param x The third parameter
       * \param y The fourth parameter
       * \param z The fifth parameter
       */
      static inline void round1(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z){
	z += ((( w & (x ^ y)) ^ y) + word(chunk,position) + 0x5A827999 + rol32(v, 5));
	w = rol32(w, 30);
      }

      /*!
       * Third core function
       *
       * \param chunk The chunk to process
       * \param position The position
       * \param v The first parameter
       * \param w The second parameter
       * \param x The third parameter
       * \param y The fourth parameter
       * \param z The fifth parameter
       */
      static inline void round2(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z){
	z += (( w ^ x ^ y) + word(chunk, position) + 0x6ED9EBA1 + rol32(v, 5));
	w = rol32(w, 30);
      }

      /*!
       * Fourth core function
       *
       * \param chunk The chunk to process
       * \param position The position
       * \param v The first parameter
       * \param w The second parameter
       * \param x The third parameter
       * \param y The fourth parameter
       * \param z The fifth parameter
       */
      static inline void round3(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z){
	z += (((( w | x) & y) | (w & x)) + word(chunk, position) + 0x8F1BBCDC + rol32(v, 5));
	w = rol32(w, 30);
      }

      /*!
       * Fifth core function
       *
       * \param chunk The chunk to process
       * \param position The position
       * \param v The first parameter
       * \param w The second parameter
       * \param x The third parameter
       * \param y The fourth parameter
       * \param z The fifth parameter
       */
      static inline void round4(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z){
	z += ((w ^ x ^ y) + word(chunk, position) + 0xCA62C1D6 + rol32(v, 5));
	w = rol32(w, 30);
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_SHA1_H_
