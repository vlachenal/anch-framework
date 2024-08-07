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

#include "crypto/hash/sha224.hpp"
#include "crypto/hash/sha256.hpp"
#include "crypto/hash/sha384.hpp"
#include "crypto/hash/sha512.hpp"

using anch::crypto::Hash;
using anch::crypto::SHA2;
using anch::crypto::SHA224_256;
using anch::crypto::SHA384_512;

// SHA-224 +
std::array<uint32_t,8> anch::crypto::SHA224_VALUES = { {
    0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939,
    0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4
  } };
using anch::crypto::SHA224_VALUES;
template class SHA224_256<28,SHA224_VALUES>;
template class Hash<28,64>;
template class SHA2<28,64,uint32_t,64,SHA224_VALUES>;
//template SHA224_256<28,SHA224_VALUES>::SHA224_256(const std::string& data);
//template SHA224_256<28,SHA224_VALUES>::SHA224_256(std::istream& stream);
// SHA-224 -

// SHA-256 +
std::array<uint32_t,8> anch::crypto::SHA256_VALUES = { {
    0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
  } };
using anch::crypto::SHA256_VALUES;
template class Hash<32,64>;
template class SHA2<32,64,uint32_t,64,SHA256_VALUES>;
template class SHA224_256<32,SHA256_VALUES>;
//template SHA224_256<32,SHA256_VALUES>::SHA224_256(const std::string& data);
//template SHA224_256<32,SHA256_VALUES>::SHA224_256(std::istream& stream);
// SHA-256 -

// SHA-384 +
std::array<uint64_t,8> anch::crypto::SHA384_VALUES = { {
    0xCBBB9D5DC1059ED8, 0x629A292A367CD507, 0X9159015A3070DD17, 0X152FECD8F70E5939,
    0x67332667FFC00B31, 0x8EB44A8768581511, 0xDB0C2E0D64F98FA7, 0x47B5481DBEFA4FA4
  } };
using anch::crypto::SHA384_VALUES;
template class Hash<48,128>;
template class SHA2<48,128,uint64_t,80,SHA384_VALUES>;
template class SHA384_512<48,SHA384_VALUES>;
template SHA384_512<48,SHA384_VALUES>::SHA384_512(const std::string& data);
template SHA384_512<48,SHA384_VALUES>::SHA384_512(std::istream& stream);
// SHA-384 -

// SHA-512 +
std::array<uint64_t,8> anch::crypto::SHA512_VALUES = { {
    0x6A09E667F3BCC908, 0xBB67AE8584CAA73B, 0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
    0x510E527FADE682D1, 0x9B05688C2B3E6C1F, 0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179
  } };
using anch::crypto::SHA512_VALUES;
template class Hash<64,128>;
template class SHA2<64,128,uint64_t,80,SHA512_VALUES>;
template class SHA384_512<64,SHA512_VALUES>;
template SHA384_512<64,SHA512_VALUES>::SHA384_512(const std::string& data);
template SHA384_512<64,SHA512_VALUES>::SHA384_512(std::istream& stream);;
// SHA-512 -
