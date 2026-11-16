/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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
/*
 * Base64 fuzzer: decoding with every alphabet and padding option ; decoded data is encoded and decoded again to
 * the same bytes.
 */
#include "crypto/b2t/base64.hpp"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <typeinfo>

using anch::crypto::Base64;

extern "C" int
LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {
  if(size == 0) {
    return 0;
  }
  static const uint8_t OPTIONS[] = {
    static_cast<uint8_t>(Base64::STD | Base64::PADDING), static_cast<uint8_t>(Base64::STD | Base64::NOPADDING),
    static_cast<uint8_t>(Base64::URL | Base64::PADDING), static_cast<uint8_t>(Base64::URL | Base64::NOPADDING)
  };
  const uint8_t options = OPTIONS[data[0] % 4];
  // Exact size buffer: ASan detects any read past the data
  char* input = new char[size - 1];
  std::copy(data + 1, data + size, input);
  std::string decoded;
  bool valid = true;
  try {
    decoded = Base64::decode(input, size - 1, options);
  } catch(const std::bad_cast&) {
    valid = false;
  }
  delete[] input;
  if(valid) {
    // Round trip with the same options
    const std::string encoded = Base64::encode(decoded, options);
    if(Base64::decode(encoded, options) != decoded) {
      std::abort();
    }
  }
  return 0;
}
