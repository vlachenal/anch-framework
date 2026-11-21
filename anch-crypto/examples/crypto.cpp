/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
//! [example]
#include "crypto/b2t/base64.hpp"
#include "crypto/cipher/aes256.hpp"
#include "crypto/cipher/cbc.hpp"
#include "crypto/hash/sha256.hpp"
#include "crypto/hmac.hpp"
#include "crypto/padding/pkcs5Padding.hpp"

#include <array>
#include <iostream>
#include <sstream>

using anch::crypto::SHA256;

int
main() {
  // Hash: digest is computed by the constructor, operator<< writes it in hexadecimal
  std::cout << SHA256("abc") << std::endl;

  // HMAC with any hash
  std::cout << anch::crypto::HMAC<SHA256>("key", "The quick brown fox jumps over the lazy dog") << std::endl;

  // Base64
  std::string encoded = anch::crypto::Base64::encode(std::string("AnCH framework"));
  std::cout << encoded << " -> " << anch::crypto::Base64::decode(encoded) << std::endl;

  // AES-256 in CBC mode with PKCS#5 padding (streams)
  std::array<uint8_t,16> iv = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  const std::string key = "0123456789abcdef0123456789abcdef"; // 32 bytes
  std::istringstream plain("secret message");
  std::ostringstream ciphered;
  anch::crypto::CBC<anch::crypto::AES256, anch::crypto::PKCS5Padding>(iv).cipher(plain, ciphered, key);
  std::istringstream input(ciphered.str());
  std::ostringstream deciphered;
  anch::crypto::CBC<anch::crypto::AES256, anch::crypto::PKCS5Padding>(iv).decipher(input, deciphered, key);
  std::cout << deciphered.str() << std::endl;
  return 0;
}
//! [example]
