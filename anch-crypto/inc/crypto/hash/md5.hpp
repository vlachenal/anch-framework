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
#ifndef _ANCH_CRYPTO_MD5_H_
#define _ANCH_CRYPTO_MD5_H_

#include <iostream>
#include <ostream>
#include <iomanip>
#include <array>


namespace anch {
  namespace crypto {

    /**
     * MD5 hash algorithm implementation<br>
     * This algorithm is based on Qt framework algorithm.
     *
     * @author Vincent Lachenal
     */
    class MD5 {
    private:
      /**
       * MD5 execution context
       *
       * @author Vincent Lachenal
       */
      struct Context {
	/** Number of bits handle mod 2^64 */
	std::array<uint32_t,2> handle;

	/** Scratch buffer */
	std::array<uint32_t,4> buffer;

	/** Input buffer */
	std::array<uint32_t,16> input;

	/** Actual digest after computing */
	std::array<uint8_t,16> digest;

	/**
	 * {@link Context} default constructor
	 */
	Context() {
	  reset();
	}

	/**
	 * Reset current context
	 */
	void reset() {
	  handle = { {0,0} };
	  buffer = { {0x67452301,0xefcdab89,0x98badcfe,0x10325476}} ;
	}
      };

      // Attributes +
    private:
      /** MD5 context */
      Context _context;
      // Attributes -

      // Constructors +
    public:
      /**
       * {@link MD5} default constructor
       */
      MD5();

      /**
       * {@link MD5} constructor with string
       *
       * @param data The string data to process
       */
      template<class CharT, class Traits, class Allocator>
      MD5(const std::basic_string<CharT,Traits,Allocator>& data) {
	addData(reinterpret_cast<const uint8_t*>(data.data()), data.length());
	finalize();
      }

      /**
       * {@link MD5} constructor with input stream.
       *
       * @param stream The input stream to process
       */
      template<class CharT, class Traits>
      MD5(std::basic_istream<CharT,Traits>& stream) {
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
       * {@link MD5} destructor
       */
      virtual ~MD5();
      // Destructor -

      // Methods +
    public:
      /**
       * Get the MD5 hash result
       *
       * @return the MD5 hash result
       */
      const std::array<uint8_t,16>& digest() const;

    private:
      /**
       * Compute hash for data with the current hash
       *
       * @param data The data to add
       * @param len The data length
       */
      void addData(const uint8_t* data, size_t len);

      /**
       * Finalize hash
       */
      void finalize();

      /**
       * Apply transformation
       */
      void transform();

      /**
       * Call core function transformation
       *
       * @param a The byte to change and the first buffer value
       * @param b The second buffer value
       * @param d The third buffer value
       * @param in The 'offset'
       * @param bits The number of bits to rotate
       */
      template<class Core>
      static inline void transform(uint32_t& a,
				   uint32_t b,
				   uint32_t c,
				   uint32_t d,
				   uint32_t in,
				   int bits) {
	a += Core::apply(b,c,d) + in;
	a = (a << bits | a >> (32 - bits)) + b;
      }

      /**
       * Swap byte
       *
       * @param buf The bytes to process
       * @param words The number of operation to do
       */
      static inline void byteSwap(uint32_t* buf, uint8_t words);
      // Methods -

      // Core functions +
    private:
      class Core1 {
      public:
	inline static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
	  return (c ^ (a & (b ^ c)));
	}
      };

      class Core2 {
      public:
	inline static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
	  return Core1::apply(c,a,b);
	}
      };

      class Core3 {
      public:
	inline static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
	  return (a ^ b ^ c);
	}
      };

      class Core4 {
      public:
	inline static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
	  return (b ^ (a | ~c));
	}
      };
      // Core functions -

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
operator << (std::basic_ostream<CharT, Traits>& out, const anch::crypto::MD5& hash) {
  std::ios_base::fmtflags flags = out.flags(); // Save current flags
  out << std::hex;
  for(const uint8_t& byte : hash.digest()) {
    out << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(byte);
  }
  out.flags(flags); // Restore flags
  return out;
}

#endif // _ANCH_CRYPTO_MD5_H_
