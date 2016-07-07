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
    /*
     * SHA224 defintion
     */
    extern template class Hash<28,64>;
    extern template const std::array<uint8_t,28>& Hash<28,64>::digest(const std::string& data);
    extern template const std::array<uint8_t,28>& Hash<28,64>::digest(const std::wstring& data);
    extern template const std::array<uint8_t,28>& Hash<28,64>::digest(std::istream& stream);
    //extern template const std::array<uint8_t,28>& Hash<28,64>::digest(std::wistream& stream);
    extern template class SHA2<28,64,uint32_t,64,SHA224_VALUES>;
    extern template SHA224_256<28,SHA224_VALUES>::SHA224_256(const std::string& data);
    extern template SHA224_256<28,SHA224_VALUES>::SHA224_256(const std::wstring& data);
    extern template SHA224_256<28,SHA224_VALUES>::SHA224_256(std::istream& stream);
    //extern template SHA224_256<28,SHA224_VALUES>::SHA224_256(std::wistream& stream);
    /*!
     * SHA224 definition
     */
    using SHA224 = SHA224_256<28,SHA224_VALUES>;

    extern template SHA224 HMAC<SHA224>(const std::string&, const std::string&);

  }
}

#endif // _ANCH_CRYPTO_SHA224_H_
