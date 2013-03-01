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
     * MD5 hash algorithm implementation
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
       * {@link MD5} constructor
       *
       * @param data The string data to process
       */
      MD5(const std::string& data);
      // Constructors -

      // Destructor +
      /**
       * {@link MD5} destructor
       */
      virtual ~MD5();
      // Destructor -

      // Methods +
    public:
      const std::array<uint8_t,16>& digest() const;

    private:
      void addData(const uint8_t* data, size_t len);

      void finalize();

      void transform();

      template<class T>
      inline void transform(uint32_t& a,
			    uint32_t b,
			    uint32_t c,
			    uint32_t d,
			    uint32_t in,
			    int bits) {
	a += T::apply(b,c,d) + in;
	a = (a << bits | a >> (32 - bits)) + b;
      }

      inline void byteSwap(uint32_t* buf, uint8_t words) {
	const uint32_t byteOrderTest = 0x1;
	if(((char*)&byteOrderTest)[0] == 0) {
	  uint8_t* p = (uint8_t*)buf;
	  do {
	    *buf++ = (uint32_t)((uint8_t)p[3] << 8 | p[2]) << 16 | ((uint8_t)p[1] << 8 | p[0]);
	    p += 4;
	  } while(--words);
	}
      }
      // Methods -

      // Inner classes +
    private:
      class Step1 {
      public:
	static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
	  return (c ^ (a & (b ^ c)));
	}
      };

      class Step2 {
      public:
	static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
	  return Step1::apply(c,a,b);
	}
      };

      class Step3 {
      public:
	static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
	  return (a ^ b ^ c);
	}
      };

      class Step4 {
      public:
	static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
	  return (b ^ (a | ~c));
	}
      };
      // Inner classes -

    };

  }
}

template<typename charT, class traits>
std::basic_ostream<charT, traits>&
operator << (std::basic_ostream<charT, traits>& out, const anch::crypto::MD5& hash) {
  out << std::hex;
  for(const uint8_t& byte : hash.digest()) {
    out << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(byte);
  }
  return out;
}

#endif // _ANCH_CRYPTO_MD5_H_
