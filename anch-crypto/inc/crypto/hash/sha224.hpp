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
#ifndef _ANCH_CRYPTO_SHA224_H_
#define _ANCH_CRYPTO_SHA224_H_

#include "crypto/hash/sha224_256.hpp"

namespace anch {
  namespace crypto {

    /*! SHA224 initial values */
    extern std::array<uint32_t,8> SHA224_VALUES;
    /*!
     * SHA224 defintion
     */
    using SHA224 = SHA224_256<28,SHA224_VALUES>;

  }
}

#endif // _ANCH_CRYPTO_SHA224_H_
