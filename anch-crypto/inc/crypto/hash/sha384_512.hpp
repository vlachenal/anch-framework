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
    class SHA384_512: public SHA2<O,128,uint64_t,80,I> {

      // Constructors +
    public:
      /*!
       * \ref SHA384_512 default constructor
       */
      SHA384_512(): SHA2<O,128,uint64_t,80,I>() {
	// Nothing to do
      }

      /*!
       * \ref SHA384_512 constructor with string
       *
       * \param data The string data to process
       */
      template<class CharT, class Traits, class Allocator>
      SHA384_512(const std::basic_string<CharT,Traits,Allocator>& data):
	SHA2<O,128,uint64_t,80,I>() {
	Hash<O,128>::digest(data);
      }

      /*!
       * \ref SHA384_512 constructor with input stream.
       *
       * \param stream The input stream to process
       */
      template<class CharT, class Traits>
      SHA384_512(std::basic_istream<CharT,Traits>& stream):
	SHA2<O,128,uint64_t,80,I>() {
	Hash<O,128>::digest(stream);
      }

      /*!
       * \ref SHA384_512 constructor with data bytes.
       *
       * \param data the data bytes
       * \param len the dataa length
       */
      SHA384_512(const uint8_t* data, std::size_t len):
	SHA2<O,128,uint64_t,80,I>() {
	Hash<O,128>::digest(data, len);
      }
      // Constructors -


      // Destructor +
      /*!
       * \ref SHA384_512 destructor
       */
      virtual ~SHA384_512() {
	// Nothing to do
      }
      // Destructor -


      // Methods +
    private:
      /*!
       * SHA384/512 translation array getter.
       *
       * \return the SHA384/512 translation array
       */
      virtual const std::array<uint64_t,80>& getTranslationArray() const {
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
       * SHA384/512 first sigma function
       *
       * \param word the word to transform
       */
      virtual uint64_t SIGMA0(uint64_t word) const {
	return (SHA2<O,128,uint64_t,80,I>::rotateRight(28, word)
		^ SHA2<O,128,uint64_t,80,I>::rotateRight(34, word)
		^ SHA2<O,128,uint64_t,80,I>::rotateRight(39, word));
      }

      /*!
       * SHA384/512 second sigma function
       *
       * \param word the word to transform
       */
      virtual uint64_t SIGMA1(uint64_t word) const {
	return (SHA2<O,128,uint64_t,80,I>::rotateRight(14, word)
		^ SHA2<O,128,uint64_t,80,I>::rotateRight(18, word)
		^ SHA2<O,128,uint64_t,80,I>::rotateRight(41, word));
      }

      /*!
       * SHA384/512 first sigma function
       *
       * \param word the word to transform
       */
      virtual uint64_t sigma0(uint64_t word) const {
	return (SHA2<O,128,uint64_t,80,I>::rotateRight(1, word)
		^ SHA2<O,128,uint64_t,80,I>::rotateRight(8, word)
		^ SHA2<O,128,uint64_t,80,I>::shiftRight(7, word));
      }

      /*!
       * SHA384/512 second sigma function
       *
       * \param word the word to transform
       */
      virtual uint64_t sigma1(uint64_t word) const {
	return (SHA2<O,128,uint64_t,80,I>::rotateRight(19, word)
		^ SHA2<O,128,uint64_t,80,I>::rotateRight(61, word)
		^ SHA2<O,128,uint64_t,80,I>::shiftRight(6, word));
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_SHA384_512_H_
