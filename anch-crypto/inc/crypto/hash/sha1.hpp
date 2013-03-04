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
#ifndef _ANCH_CRYPTO_SHA1_
#define _ANCH_CRYPTO_SHA1_

#include <iostream>
#include <ostream>
#include <iomanip>
#include <array>


namespace anch {
  namespace crypto {

    /**
     * SHA1 hash algorithm implementation<br>
     * This algorithm is based on Qt framework algorithm.
     *
     * @author Vincent Lachenal
     */
    class SHA1 {
    private:
      /**
       * SHA1 chunk
       *
       * @author Vincent Lachenal
       */
      typedef union {
	/** Bytes */
	uint8_t bytes[64];

	/** Words */
	uint32_t words[16];
      } Chunk;

      /**
       * SHA1 context
       *
       * @author Vincent Lachenal
       */
      struct Context {
	/** State */
	std::array<uint32_t,5> state;

	/** Message size */
	uint64_t size;

	/** Buffer */
	std::array<uint8_t,64> buffer;

	/** Digest */
	std::array<uint8_t,20> digest;

	/**
	 * {@link SHA1} {@link Context} constructor
	 */
	Context() {
	  reset();
	}

	/**
	 * Initialize SHA1 context
	 */
	void reset() {
	  state = { {0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476,0xC3D2E1F0} };
	  size = 0;
	}
      };

      // Attributes +
    private:
      /** SHA1 context */
      Context _context;
      // Attributes -


      // Constructors +
    public:
      /**
       * {@link SHA1} default constructor
       */
      SHA1();

      /**
       * {@link SHA1} constructor with string
       *
       * @param data The string data to process
       */
      template<class CharT, class Traits, class Allocator>
      SHA1(const std::basic_string<CharT,Traits,Allocator>& data) {
	addData(reinterpret_cast<const uint8_t*>(data.data()), data.length());
	finalize();
      }

      /**
       * {@link SHA1} constructor with input stream.
       *
       * @param stream The input stream to process
       */
      template<class CharT, class Traits>
      SHA1(std::basic_istream<CharT,Traits>& stream) {
	if(stream) {
	  char data[1024];
	  while(!stream.eof()) {
	    stream.read(data, 1024);
	    addData(reinterpret_cast<uint8_t*>(data), stream.gcount());
	  }
	  finalize();
	}
      }
      // Constructors -


      // Destructor +
      /**
       * {@link SHA1} destructor
       */
      virtual ~SHA1();
      // Destructor -


      // Methods +
    public:
      /**
       * Get the MD5 hash result
       *
       * @return the MD5 hash result
       */
      const std::array<uint8_t,20>& digest() const;

    private:

      /**
       * Compute hash for data with the current hash
       *
       * @param data The data to add
       * @param len The data length
       */
      void addData(const uint8_t* data, size_t len);

      /**
       * Transform SHA1 with the current chunk
       *
       * @param buffer The data to process
       */
      void transform(const uint8_t* buffer);

      /**
       * Finalize hash
       */
      void finalize();

      /**
       * Bits rotation of 32 bits integer from a value
       *
       * @param value The integer to process
       * @param shift The number of bits to shift
       */
      static inline uint32_t rol32(uint32_t value, unsigned int shift);

      /**
       * Compute SHA1 word
       *
       * @param chunk The SHA1 chunk
       * @param position The position
       */
      static inline uint32_t word(Chunk& chunk, unsigned int position);

      /**
       * Swap byte for endianness conversion
       *
       * @param buf The 4-bytes words to process
       * @param count The number of operation to do
       */
      static inline void bytesSwap(uint32_t* buf, uint8_t count);

      /**
       * First core function
       *
       * @param chunk The chunk to process
       * @param position The position
       * @param v The first parameter
       * @param w The second parameter
       * @param x The third parameter
       * @param y The fourth parameter
       * @param z The fifth parameter
       */
      static inline void round0(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z);

      /**
       * Second core function
       *
       * @param chunk The chunk to process
       * @param position The position
       * @param v The first parameter
       * @param w The second parameter
       * @param x The third parameter
       * @param y The fourth parameter
       * @param z The fifth parameter
       */
      static inline void round1(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z);

      /**
       * Third core function
       *
       * @param chunk The chunk to process
       * @param position The position
       * @param v The first parameter
       * @param w The second parameter
       * @param x The third parameter
       * @param y The fourth parameter
       * @param z The fifth parameter
       */
      static inline void round2(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z);

      /**
       * Fourth core function
       *
       * @param chunk The chunk to process
       * @param position The position
       * @param v The first parameter
       * @param w The second parameter
       * @param x The third parameter
       * @param y The fourth parameter
       * @param z The fifth parameter
       */
      static inline void round3(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z);

      /**
       * Fifth core function
       *
       * @param chunk The chunk to process
       * @param position The position
       * @param v The first parameter
       * @param w The second parameter
       * @param x The third parameter
       * @param y The fourth parameter
       * @param z The fifth parameter
       */
      static inline void round4(Chunk& chunk,
				const unsigned int position,
				uint32_t& v,
				uint32_t& w,
				uint32_t& x,
				uint32_t& y,
				uint32_t& z);
      // Methods -

    };

  }
}

/**
 * Ouput stream operator definition for MD5.<br>
 * This function preserves the formatting flags.
 *
 * @param out The output stream
 * @param hash The MD5 hash
 *
 * @return The output stream
 */
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator << (std::basic_ostream<CharT, Traits>& out, const anch::crypto::SHA1& hash) {
  std::ios_base::fmtflags flags = out.flags(); // Save current flags
  out << std::hex;
  for(const uint8_t& byte : hash.digest()) {
    out << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(byte);
  }
  out.flags(flags); // Restore flags
  return out;
}

#endif // _ANCH_CRYPTO_SHA1_
