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
#include "crypto/hash/md5.hpp"

#include <cstring>


using anch::crypto::MD5;
using std::string;


// Constructors +
/**
 * {@link MD5} default constructor
 */
MD5::MD5() {
  // Nothing to do
}

/**
 * {@link MD5} constructor
 *
 * @param data The string data to process
 */
MD5::MD5(const string& data) {
  addData(reinterpret_cast<const uint8_t*>(data.data()), data.length());
  finalize();
}
// Constructors -


// Destructor +
/**
 * {@link MD5} destructor
 */
MD5::~MD5() {
  // Nothing to do
}
// Destructor -


// Methods +
const std::array<uint8_t,16>&
MD5::digest() const {
  return _context.digest;
}


void
MD5::addData(const uint8_t* data, size_t len) {
  uint32_t word;

  // Update byte count
  word = _context.handle[0];
  if((_context.handle[0] = word + len) < word) {
    _context.handle[1]++; // Carry from low to high
  }

  word = 64 - (word & 0x3f); // Space available in _context.input (at least 1)
  if(word > len) {
    std::memcpy(reinterpret_cast<uint8_t*>(&_context.input) + 64 - word, data, len);

  } else {
    // First chunk is an odd size
    std::memcpy(reinterpret_cast<uint8_t*>(&_context.input) + 64 - word, data, word);
    byteSwap(reinterpret_cast<uint32_t*>(&_context.input), 16);
    transform();
    data += word;
    len -= word;

    // Process data in 64-byte chunks
    while(len >= 64) {
      std::memcpy(reinterpret_cast<uint8_t*>(&_context.input), data, 64);
      byteSwap(reinterpret_cast<uint32_t*>(&_context.input), 16);
      transform();
      data += 64;
      len -= 64;
    }

    // Handle any remaining bytes of data.
    std::memcpy(reinterpret_cast<uint8_t*>(&_context.input),
		reinterpret_cast<const uint32_t*>(data), len);
  }
}

void
MD5::finalize() {
  int count = _context.handle[0] & 0x3f; // Number of bytes in _context.input
  uint8_t* p = reinterpret_cast<uint8_t*>(&_context.input) + count;

  // Set the first char of padding to 0x80.  There is always room.
  *p++ = 0x80;

  // Bytes of padding needed to make 56 bytes (-8..55)
  count = 56 - 1 - count;

  if(count < 0) { // Padding forces an extra block
    std::memset(p, 0, count + 8);
    byteSwap(reinterpret_cast<uint32_t*>(&_context.input), 16);
    transform();
    p = reinterpret_cast<uint8_t*>(&_context.input);
    count = 56;
  }
  std::memset(p, 0, count);
  byteSwap(reinterpret_cast<uint32_t*>(&_context.input), 14);

  // Append length in bits and transform
  _context.input[14] = _context.handle[0] << 3;
  _context.input[15] = _context.handle[1] << 3 | _context.handle[0] >> 29;
  transform();

  byteSwap(reinterpret_cast<uint32_t*>(&_context.buffer), 4);
  std::memcpy(reinterpret_cast<uint8_t*>(&_context.digest),
	      reinterpret_cast<uint32_t*>(&_context.buffer), 16);
}

void
MD5::transform() {
  uint32_t a = _context.buffer[0];
  uint32_t b = _context.buffer[1];
  uint32_t c = _context.buffer[2];
  uint32_t d = _context.buffer[3];

  transform<MD5::Step1>(a, b, c, d, _context.input[0] + 0xd76aa478, 7);
  transform<MD5::Step1>(d, a, b, c, _context.input[1] + 0xe8c7b756, 12);
  transform<MD5::Step1>(c, d, a, b, _context.input[2] + 0x242070db, 17);
  transform<MD5::Step1>(b, c, d, a, _context.input[3] + 0xc1bdceee, 22);
  transform<MD5::Step1>(a, b, c, d, _context.input[4] + 0xf57c0faf, 7);
  transform<MD5::Step1>(d, a, b, c, _context.input[5] + 0x4787c62a, 12);
  transform<MD5::Step1>(c, d, a, b, _context.input[6] + 0xa8304613, 17);
  transform<MD5::Step1>(b, c, d, a, _context.input[7] + 0xfd469501, 22);
  transform<MD5::Step1>(a, b, c, d, _context.input[8] + 0x698098d8, 7);
  transform<MD5::Step1>(d, a, b, c, _context.input[9] + 0x8b44f7af, 12);
  transform<MD5::Step1>(c, d, a, b, _context.input[10] + 0xffff5bb1, 17);
  transform<MD5::Step1>(b, c, d, a, _context.input[11] + 0x895cd7be, 22);
  transform<MD5::Step1>(a, b, c, d, _context.input[12] + 0x6b901122, 7);
  transform<MD5::Step1>(d, a, b, c, _context.input[13] + 0xfd987193, 12);
  transform<MD5::Step1>(c, d, a, b, _context.input[14] + 0xa679438e, 17);
  transform<MD5::Step1>(b, c, d, a, _context.input[15] + 0x49b40821, 22);

  transform<MD5::Step2>(a, b, c, d, _context.input[1] + 0xf61e2562, 5);
  transform<MD5::Step2>(d, a, b, c, _context.input[6] + 0xc040b340, 9);
  transform<MD5::Step2>(c, d, a, b, _context.input[11] + 0x265e5a51, 14);
  transform<MD5::Step2>(b, c, d, a, _context.input[0] + 0xe9b6c7aa, 20);
  transform<MD5::Step2>(a, b, c, d, _context.input[5] + 0xd62f105d, 5);
  transform<MD5::Step2>(d, a, b, c, _context.input[10] + 0x02441453, 9);
  transform<MD5::Step2>(c, d, a, b, _context.input[15] + 0xd8a1e681, 14);
  transform<MD5::Step2>(b, c, d, a, _context.input[4] + 0xe7d3fbc8, 20);
  transform<MD5::Step2>(a, b, c, d, _context.input[9] + 0x21e1cde6, 5);
  transform<MD5::Step2>(d, a, b, c, _context.input[14] + 0xc33707d6, 9);
  transform<MD5::Step2>(c, d, a, b, _context.input[3] + 0xf4d50d87, 14);
  transform<MD5::Step2>(b, c, d, a, _context.input[8] + 0x455a14ed, 20);
  transform<MD5::Step2>(a, b, c, d, _context.input[13] + 0xa9e3e905, 5);
  transform<MD5::Step2>(d, a, b, c, _context.input[2] + 0xfcefa3f8, 9);
  transform<MD5::Step2>(c, d, a, b, _context.input[7] + 0x676f02d9, 14);
  transform<MD5::Step2>(b, c, d, a, _context.input[12] + 0x8d2a4c8a, 20);

  transform<MD5::Step3>(a, b, c, d, _context.input[5] + 0xfffa3942, 4);
  transform<MD5::Step3>(d, a, b, c, _context.input[8] + 0x8771f681, 11);
  transform<MD5::Step3>(c, d, a, b, _context.input[11] + 0x6d9d6122, 16);
  transform<MD5::Step3>(b, c, d, a, _context.input[14] + 0xfde5380c, 23);
  transform<MD5::Step3>(a, b, c, d, _context.input[1] + 0xa4beea44, 4);
  transform<MD5::Step3>(d, a, b, c, _context.input[4] + 0x4bdecfa9, 11);
  transform<MD5::Step3>(c, d, a, b, _context.input[7] + 0xf6bb4b60, 16);
  transform<MD5::Step3>(b, c, d, a, _context.input[10] + 0xbebfbc70, 23);
  transform<MD5::Step3>(a, b, c, d, _context.input[13] + 0x289b7ec6, 4);
  transform<MD5::Step3>(d, a, b, c, _context.input[0] + 0xeaa127fa, 11);
  transform<MD5::Step3>(c, d, a, b, _context.input[3] + 0xd4ef3085, 16);
  transform<MD5::Step3>(b, c, d, a, _context.input[6] + 0x04881d05, 23);
  transform<MD5::Step3>(a, b, c, d, _context.input[9] + 0xd9d4d039, 4);
  transform<MD5::Step3>(d, a, b, c, _context.input[12] + 0xe6db99e5, 11);
  transform<MD5::Step3>(c, d, a, b, _context.input[15] + 0x1fa27cf8, 16);
  transform<MD5::Step3>(b, c, d, a, _context.input[2] + 0xc4ac5665, 23);

  transform<MD5::Step4>(a, b, c, d, _context.input[0] + 0xf4292244, 6);
  transform<MD5::Step4>(d, a, b, c, _context.input[7] + 0x432aff97, 10);
  transform<MD5::Step4>(c, d, a, b, _context.input[14] + 0xab9423a7, 15);
  transform<MD5::Step4>(b, c, d, a, _context.input[5] + 0xfc93a039, 21);
  transform<MD5::Step4>(a, b, c, d, _context.input[12] + 0x655b59c3, 6);
  transform<MD5::Step4>(d, a, b, c, _context.input[3] + 0x8f0ccc92, 10);
  transform<MD5::Step4>(c, d, a, b, _context.input[10] + 0xffeff47d, 15);
  transform<MD5::Step4>(b, c, d, a, _context.input[1] + 0x85845dd1, 21);
  transform<MD5::Step4>(a, b, c, d, _context.input[8] + 0x6fa87e4f, 6);
  transform<MD5::Step4>(d, a, b, c, _context.input[15] + 0xfe2ce6e0, 10);
  transform<MD5::Step4>(c, d, a, b, _context.input[6] + 0xa3014314, 15);
  transform<MD5::Step4>(b, c, d, a, _context.input[13] + 0x4e0811a1, 21);
  transform<MD5::Step4>(a, b, c, d, _context.input[4] + 0xf7537e82, 6);
  transform<MD5::Step4>(d, a, b, c, _context.input[11] + 0xbd3af235, 10);
  transform<MD5::Step4>(c, d, a, b, _context.input[2] + 0x2ad7d2bb, 15);
  transform<MD5::Step4>(b, c, d, a, _context.input[9] + 0xeb86d391, 21);

  _context.buffer[0] += a;
  _context.buffer[1] += b;
  _context.buffer[2] += c;
  _context.buffer[3] += d;
}
// Methods -
