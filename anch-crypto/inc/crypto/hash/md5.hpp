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

#include "crypto/hash/hash.hpp"

#include <cstring>


namespace anch {
  namespace crypto {

    template<typename H> H HMAC(const std::string&, const std::string&);

    /*!
     * \brief MD5 hash algorithm implementation
     *
     * This algorithm is based on Qt framework algorithm.
     *
     * \author Vincent Lachenal
     */
    class MD5: public Hash<16,64> {

      friend MD5 anch::crypto::HMAC<MD5>(const std::string&, const std::string&);

    private:
      /*!
       * MD5 execution context
       *
       * \author Vincent Lachenal
       */
      struct Context {
	/*! Number of bits handle mod 2^64 */
	uint32_t handle[2];

	/*! Scratch buffer */
	uint32_t buffer[4];

	/*! Input buffer */
	uint32_t input[16];

	/*! Actual digest after computing */
	std::array<uint8_t,16> digest;

	/*!
	 * \ref Context default constructor
	 */
	Context() {
	  reset();
	}

	/*!
	 * Reset current context
	 */
	void reset() {
	  static uint32_t HANDLE_INIT[2] = { 0, 0 };
	  std::memcpy(handle, HANDLE_INIT, 2 * sizeof(uint32_t));
	  static uint32_t BUFFER_INIT[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, };
	  std::memcpy(buffer, BUFFER_INIT, 4 * sizeof(uint32_t));
	}
      };

      // Attributes +
    private:
      /*! MD5 context */
      Context _context;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref MD5 default constructor
       */
      MD5();

      /*!
       * \ref MD5 constructor with string
       *
       * \param data The string data to process
       */
      template<class CharT, class Traits, class Allocator>
      MD5(const std::basic_string<CharT,Traits,Allocator>& data) {
	Hash::digest(data);
      }

      /*!
       * \ref MD5 constructor with input stream.
       *
       * \param stream The input stream to process
       */
      template<class CharT, class Traits>
      MD5(std::basic_istream<CharT,Traits>& stream) {
	Hash::digest(stream);
      }

    private:
      /*!
       * \ref MD5 constructor with bytes
       *
       * \param data the data bytes
       * \param len the data length
       */
      MD5(const uint8_t* data, std::size_t len) {
      	Hash::digest(data, len);
      }
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref MD5 destructor
       */
      virtual ~MD5();
      // Destructor -

      // Methods +
    public:
      /*!
       * Get the MD5 hash result
       *
       * \return the MD5 hash result
       */
      virtual const std::array<uint8_t,16>& digest() const override;

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
       * Apply transformation
       */
      void transform();

      /*!
       * Call core function transformation
       *
       * \param a The byte to change and the first buffer value
       * \param b The second buffer value
       * \param d The third buffer value
       * \param in The 'offset'
       * \param bits The number of bits to rotate
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
	  return Core1::apply(c, a, b);
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

    extern template class Hash<16,64>;
    extern template MD5 HMAC<MD5>(const std::string&, const std::string&);

  }
}

#endif // _ANCH_CRYPTO_MD5_H_
