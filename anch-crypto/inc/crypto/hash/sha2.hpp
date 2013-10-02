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
#ifndef _ANCH_CRYPTO_SHA2_H_
#define _ANCH_CRYPTO_SHA2_H_

#include "crypto/hash/hash.hpp"

#include <cstring>

namespace anch {
  namespace crypto {

    /*!
     * \brief SHA2 interface.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<std::size_t O, std::size_t B>
    class SHA2: public Hash<O,B> {

    protected:
      /*!
       * Some SHA2 transformation function
       *
       * \param x first value
       * \param y second value
       * \param z third value
       */
      static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
	return ((x & (y ^ z)) ^ z);
      }

      /*!
       * Some SHA2 transformation function
       *
       * \param x first value
       * \param y second value
       * \param z third value
       */
      static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
	return ((x & (y | z)) | (y & z));
      }

    };

  }
}

#endif // _ANCH_CRYPTO_SHA2_H_
