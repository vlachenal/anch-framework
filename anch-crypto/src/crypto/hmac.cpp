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
#include "crypto/hmac.hpp"
#include "crypto/hash/md5.hpp"
#include "crypto/hash/sha1.hpp"
#include "crypto/hash/sha224.hpp"
#include "crypto/hash/sha256.hpp"
#include "crypto/hash/sha384.hpp"
#include "crypto/hash/sha512.hpp"

using anch::crypto::HMAC;
using anch::crypto::Hash;
using anch::crypto::MD5;
using anch::crypto::SHA1;
using anch::crypto::SHA224;
using anch::crypto::SHA256;
using anch::crypto::SHA384;
using anch::crypto::SHA512;

template MD5 HMAC<MD5>(const std::string&, const std::string&);
template SHA1 HMAC<SHA1>(const std::string&, const std::string&);
template SHA224 HMAC<SHA224>(const std::string&, const std::string&);
template SHA256 HMAC<SHA256>(const std::string&, const std::string&);
template SHA384 HMAC<SHA384>(const std::string&, const std::string&);
template SHA512 HMAC<SHA512>(const std::string&, const std::string&);
