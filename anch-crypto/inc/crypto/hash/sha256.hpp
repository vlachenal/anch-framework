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
#ifndef _ANCH_CRYPTO_SHA256_H_
#define _ANCH_CRYPTO_SHA256_H_

#include "crypto/hash/sha224_256.hpp"

#include <sstream>
#include <fstream>

namespace anch {
  namespace crypto {

    /*! SHA256 initial values */
    extern std::array<uint32_t,8> SHA256_VALUES;
    /*
     * SHA256 defintion
     */
    extern template class Hash<32,64>;
    extern template const std::array<uint8_t,32>& Hash<32,64>::digest(const std::string&);
    extern template const std::array<uint8_t,32>& Hash<32,64>::digest(const std::wstring&);
    extern template const std::array<uint8_t,32>& Hash<32,64>::digest(std::istream&);
    //extern template const std::array<uint8_t,32>& Hash<32,64>::digest(std::wistream& data);
    extern template class SHA2<32,64,uint32_t,64,SHA256_VALUES>;
    extern template class SHA224_256<32,SHA256_VALUES>;
    extern template SHA224_256<32,SHA256_VALUES>::SHA224_256(const std::string& data);
    extern template SHA224_256<32,SHA256_VALUES>::SHA224_256(const std::wstring& data);
    extern template SHA224_256<32,SHA256_VALUES>::SHA224_256(std::istream& stream);
    //extern template SHA224_256<32,SHA256_VALUES>::SHA224_256(std::wistream& stream);
    /*!
     * SHA256 defintion
     */
    using SHA256 = SHA224_256<32,SHA256_VALUES>;

    extern template SHA256 HMAC<SHA256>(const std::string&, const std::string&);

  }
}

#endif // _ANCH_CRYPTO_SHA256_H_
