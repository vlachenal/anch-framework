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
#ifndef _ANCH_CRYPTO_HASH_H_
#define _ANCH_CRYPTO_HASH_H_

#include <iostream>
#include <ostream>
#include <iomanip>
#include <array>


namespace anch {
  namespace crypto {

    /*!
     * \brief Hash algorithm abstract class.
     *
     * N is the hash output size in bytes.\n
     * B is the hash block size in bytes (used in HMAC algorithm).
     *
     * \author Vincent Lachenal
     */
    template<std::size_t O, std::size_t B>
    class Hash {

      // Methods +
    public:
      /*!
       * Get the hash result
       *
       * \return the hash result
       */
      virtual const std::array<uint8_t,O>& digest() const = 0;

      /*!
       * Reset current hash context and compute hash for string
       *
       * \param data the string to hash
       *
       * \return the hash result
       */
      template<class CharT, class Traits, class Allocator>
      const std::array<uint8_t,O>& digest(const std::basic_string<CharT,Traits,Allocator>& data) {
	reset();
	addData(reinterpret_cast<const uint8_t*>(data.data()), data.length());
	finalize();
	return digest();
      }

      /*!
       * Reset current hash context and compute hash for string
       *
       * \param data the string to hash
       *
       * \return the hash result
       */
      template<std::size_t N>
      const std::array<uint8_t,O>& digest(const std::array<uint8_t,N>& data) {
	reset();
	addData(data.data(), N);
	finalize();
	return digest();
      }

      /*!
       * Reset current hash context and compute hash for string
       *
       * \param data the string to hash
       * \param len the data length
       *
       * \return the hash result
       */
      const std::array<uint8_t,O>& digest(const uint8_t* data, std::size_t len) {
	reset();
	addData(data, len);
	finalize();
	return digest();
      }

      /*!
       * Reset current hash context and compute hash for stream
       *
       * \param stream the stream to hash
       *
       * \return the hash result
       */
      template<class CharT, class Traits>
      const std::array<uint8_t,O>& digest(std::basic_istream<CharT,Traits>& stream) {
	reset();
	if(stream) {
	  char data[1024];
	  while(!stream.eof()) {
	    stream.read(data, 1024);
	    addData(reinterpret_cast<uint8_t*>(data), static_cast<std::size_t>(stream.gcount()));
	  }
	  finalize();
	}
	return digest();
      }

    protected:
      /*!
       * Reset hash context
       */
      virtual void reset() = 0;

      /*!
       * Compute hash for data with the current hash
       *
       * \param data The data to add
       * \param len The data length
       */
      virtual void addData(const uint8_t* data, std::size_t len) = 0;

      /*!
       * Reset hash context
       */
      virtual void finalize() = 0;
      // Methods -


      // Accessors +
    public:
      /*!
       * Hash output size getter
       *
       * \return the output size
       */
      inline static constexpr std::size_t getOutputSize() {
	return O;
      }

      /*!
       * Hash block size getter
       *
       * \return the block size
       */
      inline static constexpr std::size_t getBlockSize() {
	return B;
      }
      // Accessors -

    };

  }
}

/*!
 * Ouput stream operator definition for every hash algorithm.\n
 * This function preserves the formatting flags.
 *
 * \param out The output stream
 * \param hash The hash
 *
 * \return The output stream
 */
template<class CharT, class Traits, std::size_t O, std::size_t B>
std::basic_ostream<CharT, Traits>&
operator << (std::basic_ostream<CharT, Traits>& out, const anch::crypto::Hash<O,B>& hash) {
  std::ios_base::fmtflags flags = out.flags(); // Save current flags
  out << std::hex;
  for(const uint8_t& byte : hash.digest()) {
    out << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(byte);
  }
  out.flags(flags); // Restore flags
  return out;
}

#endif // _ANCH_CRYPTO_HASH_H_
