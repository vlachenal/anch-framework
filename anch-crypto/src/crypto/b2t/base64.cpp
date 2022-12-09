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
#include "crypto/b2t/base64.hpp"

using anch::crypto::Base64;


const uint8_t Base64::STD = 0b00000001;
const uint8_t Base64::URL = 0b00000010;
const uint8_t Base64::PADDING = 0b00000100;
const uint8_t Base64::NOPADDING = 0b00001000;

const char BASE64_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/";

const char BASE64_URL_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789-_";

const char BASE64_PADDING = '=';


// Constructors +
/*!
 * \ref Base64 private constructor
 */
Base64::Base64() {
  // Nothing to do
}
// Constructors -


// Destructor +
/*!
 * \ref Base64 destructor
 */
Base64::~Base64() {
  // Nothing to do
}
// Destructor -


// Encoding methods +
uint8_t
encode(const uint8_t* data, uint64_t length, char* buffer, const char alphabet[64], bool padding) noexcept {
  uint8_t read = 0;
  buffer[0] = alphabet[data[0] >> 2];
  if(length >= 3) {
    read = 3;
    buffer[1] = alphabet[((data[0] & 3) << 4) | (data[1] >> 4)];
    buffer[2] = alphabet[((data[1] & 0x0F) << 2) | (data[2] >> 6)];
    buffer[3] = alphabet[data[2] & 0x3F];

  } else if(length == 1) {
    read = 1;
    buffer[1] = alphabet[(data[0] & 3) << 4];
    if(padding) {
      buffer[2] = BASE64_PADDING;
      buffer[3] = BASE64_PADDING;
    }

  } else {
    read = 2;
    buffer[1] = alphabet[((data[0] & 3) << 4) | (data[1] >> 4)];
    buffer[2] = alphabet[(data[1] & 0x0F) << 2];
    if(padding) {
      buffer[3] = BASE64_PADDING;
    }
  }
  return read;
}

void
Base64::encode(std::istream& data, std::ostream& output, uint8_t options) noexcept {
  if(data) {
    char buffer[1024];
    while(!data.eof()) {
      data.read(buffer, 1024);
      encode(reinterpret_cast<uint8_t*>(buffer), static_cast<uint64_t>(data.gcount()), output, options);
    }
  }
}

std::string
Base64::encode(std::istream& data, uint8_t options) noexcept {
  std::ostringstream out;
  encode(data, out, options);
  return out.str();
}

void
Base64::encode(const std::string& data, std::ostream& output, uint8_t options) noexcept {
  encode(reinterpret_cast<const uint8_t*>(data.data()), data.length(), output, options);
}

std::string
Base64::encode(const std::string& data, uint8_t options) noexcept {
  std::ostringstream out;
  encode(data, out, options);
  return out.str();
}

void
Base64::encode(const uint8_t* data, uint64_t length, std::ostream& output, uint8_t options) noexcept {
  const char* alphabet = (options & Base64::STD) == Base64::STD ? BASE64_ALPHABET : BASE64_URL_ALPHABET;
  bool padding = (options & Base64::PADDING) == Base64::PADDING;
  char buffer[5];
  buffer[4] = '\0';
  while(length > 0) {
    uint8_t read = ::encode(data, length, buffer, alphabet, padding);
    output << buffer;
    data += read;
    length -= read;
  }
}

std::string
Base64::encode(const uint8_t* data, uint64_t length, uint8_t options) noexcept {
  std::ostringstream out;
  encode(data, length, out, options);
  return out.str();
}
// Encoding methods -

// Decoding methods +
void
decode(const char* data, uint64_t length, uint8_t buffer[4], const char alphabet[64]) {
  uint8_t current;
  char bytes[4];
  uint8_t read;
  for(read = 0 ; read < 4 && read < length && data[read] != '='; read++) {
    current = static_cast<uint8_t>(data[read]);
    if(current >= 'A' && current <= 'Z') {
      bytes[read] = static_cast<char>(current - 'A');
    } else if(current >= 'a' && current <= 'z') {
      bytes[read] = static_cast<char>(current + 26 - 'a');
    } else if(current >= '0' && current <= '9') {
      bytes[read] = static_cast<char>(current + 52 - '0');
    } else if((current == '+' && alphabet == BASE64_ALPHABET) || (current == '-' && alphabet == BASE64_URL_ALPHABET)) {
      bytes[read] = static_cast<char>(62);
    } else if((current == '/' && alphabet == BASE64_ALPHABET) || (current == '_' && alphabet == BASE64_URL_ALPHABET)) {
      bytes[read] = static_cast<char>(63);
    } else {
      throw std::bad_cast();
    }
  }
  if(read > 0) {
    buffer[0] = static_cast<uint8_t>((bytes[0] << 2) | (bytes[1] >> 4));
    if(read > 2) {
      buffer[1] = static_cast<uint8_t>((bytes[1] << 4) | (bytes[2] >> 2));
      if(read > 3) {
	buffer[2] = static_cast<uint8_t>((bytes[2] << 6) | bytes[3]);
      }
    }
  }
}

void
Base64::decode(std::istream& data, std::ostream& output, uint8_t options) {
  if(data) {
    char buffer[1024];
    while(!data.eof()) {
      data.read(buffer, 1024);
      decode(buffer, static_cast<uint64_t>(data.gcount()), output, options);
    }
  }
}

std::string
Base64::decode(std::istream& data, uint8_t options) {
  std::ostringstream out;
  decode(data, out, options);
  return out.str();
}

void
Base64::decode(const std::string& data, std::ostream& output, uint8_t options) {
  decode(data.data(), static_cast<uint64_t>(data.length()), output, options);
}

std::string
Base64::decode(const std::string& data, uint8_t options) {
  std::ostringstream out;
  decode(data, out, options);
  return out.str();
}

void
Base64::decode(const char* data, uint64_t length, std::ostream& output, uint8_t options) {
  const char* alphabet = (options & Base64::STD) == Base64::STD ? BASE64_ALPHABET : BASE64_URL_ALPHABET;
  uint8_t buffer[4];
  buffer[3] = '\0';
  while(length > 0) {
    memset(buffer, '\0', 3);
    ::decode(data, length, buffer, alphabet);
    output << buffer;
    data += 4;
    length -= 4;
  }
}

std::string
Base64::decode(const char* data, uint64_t length, uint8_t options) {
  std::ostringstream out;
  decode(data, length, out, options);
  return out.str();
}
// Decoding methods -
