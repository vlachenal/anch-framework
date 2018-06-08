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


using anch::crypto::Hash;
using anch::crypto::MD5;


// Core functions +
class Core1 {
public:
  inline static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
    return (c ^ (a & (b ^ c)));
  }
};

class Core2 {
public:
  inline static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
    return Core1::apply(c, a, b);
  }
};

class Core3 {
public:
  inline static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
    return (a ^ b ^ c);
  }
};

class Core4 {
public:
  inline static uint32_t apply(uint32_t a, uint32_t b, uint32_t c) {
    return (b ^ (a | ~c));
  }
};
// Core functions -


template class Hash<16,64>;

/*!
 * Swap byte
 *
 * \param buf The bytes to process
 * \param count The number of operation to do
 */
inline void
byteSwap(uint32_t* buf, uint8_t count) {
  const uint32_t byteOrderTest = 0x1;
  if(reinterpret_cast<const uint8_t*>(&byteOrderTest)[0] == 0) { // endianess test
    uint8_t* words = reinterpret_cast<uint8_t*>(buf);
    do {
      *buf++ = static_cast<uint32_t>(words[3] << 8 | words[2]) << 16 | static_cast<uint32_t>(words[1] << 8 | words[0]);
      words += 4;
    } while(--count);
  }
}

/*!
 * Call core function transformation
 *
 * \param a The byte to change and the first buffer value
 * \param b The second buffer value
 * \param d The third buffer value
 * \param in The 'offset'
 * \param bits The number of bits to rotate
 */
template<class Core>
static inline void applyCore(uint32_t& a,
			     uint32_t b,
			     uint32_t c,
			     uint32_t d,
			     uint32_t in,
			     int bits) {
  a += Core::apply(b, c, d) + in;
  a = (a << bits | a >> (32 - bits)) + b;
}


// Constructors +
MD5::MD5() {
  // Nothing to do
}

MD5::MD5(const uint8_t* data, std::size_t len) {
  Hash::digest(data, len);
}
// Constructors -


// Destructor +
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
  // Update byte count
  uint32_t word = _context.handle[0];
  if((_context.handle[0] = word + static_cast<uint32_t>(len)) < word) {
    ++_context.handle[1]; // Carry from low to high
  }

  word = 64 - (word & 0x3f); // Space available in _context.input (at least 1)
  if(word > len) {
    std::memcpy(_context.input + 64 - word, data, len);

  } else {
    // First chunk is an odd size
    std::memcpy(reinterpret_cast<uint8_t*>(_context.input) + 64 - word, data, word);
    byteSwap(_context.input, 16);
    transform();
    data += word;
    len -= word;

    // Process data in 64-byte chunks
    while(len >= 64) {
      std::memcpy(reinterpret_cast<uint8_t*>(_context.input), data, 64);
      byteSwap(_context.input, 16);
      transform();
      data += 64;
      len -= 64;
    }

    // Handle any remaining bytes of data.
    std::memcpy(_context.input,
		reinterpret_cast<const uint32_t*>(data), len);
  }
}

void
MD5::finalize() {
  int count = _context.handle[0] & 0x3f; // Number of bytes in _context.input
  uint8_t* p = reinterpret_cast<uint8_t*>(_context.input) + count;

  // Set the first char of padding to 0x80.  There is always room.
  *p++ = 0x80;

  // Bytes of padding needed to make 56 bytes (-8..55)
  count = 56 - 1 - count;

  if(count < 0) { // Padding forces an extra block
    std::memset(p, 0, static_cast<std::size_t>(count) + 8);
    byteSwap(_context.input, 16);
    transform();
    p = reinterpret_cast<uint8_t*>(_context.input);
    count = 56;
  }
  std::memset(p, 0, static_cast<std::size_t>(count));
  byteSwap(_context.input, 14);

  // Append length in bits and transform
  _context.input[14] = _context.handle[0] << 3;
  _context.input[15] = _context.handle[1] << 3 | _context.handle[0] >> 29;
  transform();

  byteSwap(_context.buffer, 4);
  std::memcpy(_context.digest.data(), _context.buffer, 16);
}

void
MD5::transform() {
  uint32_t a = _context.buffer[0];
  uint32_t b = _context.buffer[1];
  uint32_t c = _context.buffer[2];
  uint32_t d = _context.buffer[3];

  applyCore<Core1>(a, b, c, d, _context.input[0] + 0xd76aa478, 7);
  applyCore<Core1>(d, a, b, c, _context.input[1] + 0xe8c7b756, 12);
  applyCore<Core1>(c, d, a, b, _context.input[2] + 0x242070db, 17);
  applyCore<Core1>(b, c, d, a, _context.input[3] + 0xc1bdceee, 22);
  applyCore<Core1>(a, b, c, d, _context.input[4] + 0xf57c0faf, 7);
  applyCore<Core1>(d, a, b, c, _context.input[5] + 0x4787c62a, 12);
  applyCore<Core1>(c, d, a, b, _context.input[6] + 0xa8304613, 17);
  applyCore<Core1>(b, c, d, a, _context.input[7] + 0xfd469501, 22);
  applyCore<Core1>(a, b, c, d, _context.input[8] + 0x698098d8, 7);
  applyCore<Core1>(d, a, b, c, _context.input[9] + 0x8b44f7af, 12);
  applyCore<Core1>(c, d, a, b, _context.input[10] + 0xffff5bb1, 17);
  applyCore<Core1>(b, c, d, a, _context.input[11] + 0x895cd7be, 22);
  applyCore<Core1>(a, b, c, d, _context.input[12] + 0x6b901122, 7);
  applyCore<Core1>(d, a, b, c, _context.input[13] + 0xfd987193, 12);
  applyCore<Core1>(c, d, a, b, _context.input[14] + 0xa679438e, 17);
  applyCore<Core1>(b, c, d, a, _context.input[15] + 0x49b40821, 22);

  applyCore<Core2>(a, b, c, d, _context.input[1] + 0xf61e2562, 5);
  applyCore<Core2>(d, a, b, c, _context.input[6] + 0xc040b340, 9);
  applyCore<Core2>(c, d, a, b, _context.input[11] + 0x265e5a51, 14);
  applyCore<Core2>(b, c, d, a, _context.input[0] + 0xe9b6c7aa, 20);
  applyCore<Core2>(a, b, c, d, _context.input[5] + 0xd62f105d, 5);
  applyCore<Core2>(d, a, b, c, _context.input[10] + 0x02441453, 9);
  applyCore<Core2>(c, d, a, b, _context.input[15] + 0xd8a1e681, 14);
  applyCore<Core2>(b, c, d, a, _context.input[4] + 0xe7d3fbc8, 20);
  applyCore<Core2>(a, b, c, d, _context.input[9] + 0x21e1cde6, 5);
  applyCore<Core2>(d, a, b, c, _context.input[14] + 0xc33707d6, 9);
  applyCore<Core2>(c, d, a, b, _context.input[3] + 0xf4d50d87, 14);
  applyCore<Core2>(b, c, d, a, _context.input[8] + 0x455a14ed, 20);
  applyCore<Core2>(a, b, c, d, _context.input[13] + 0xa9e3e905, 5);
  applyCore<Core2>(d, a, b, c, _context.input[2] + 0xfcefa3f8, 9);
  applyCore<Core2>(c, d, a, b, _context.input[7] + 0x676f02d9, 14);
  applyCore<Core2>(b, c, d, a, _context.input[12] + 0x8d2a4c8a, 20);

  applyCore<Core3>(a, b, c, d, _context.input[5] + 0xfffa3942, 4);
  applyCore<Core3>(d, a, b, c, _context.input[8] + 0x8771f681, 11);
  applyCore<Core3>(c, d, a, b, _context.input[11] + 0x6d9d6122, 16);
  applyCore<Core3>(b, c, d, a, _context.input[14] + 0xfde5380c, 23);
  applyCore<Core3>(a, b, c, d, _context.input[1] + 0xa4beea44, 4);
  applyCore<Core3>(d, a, b, c, _context.input[4] + 0x4bdecfa9, 11);
  applyCore<Core3>(c, d, a, b, _context.input[7] + 0xf6bb4b60, 16);
  applyCore<Core3>(b, c, d, a, _context.input[10] + 0xbebfbc70, 23);
  applyCore<Core3>(a, b, c, d, _context.input[13] + 0x289b7ec6, 4);
  applyCore<Core3>(d, a, b, c, _context.input[0] + 0xeaa127fa, 11);
  applyCore<Core3>(c, d, a, b, _context.input[3] + 0xd4ef3085, 16);
  applyCore<Core3>(b, c, d, a, _context.input[6] + 0x04881d05, 23);
  applyCore<Core3>(a, b, c, d, _context.input[9] + 0xd9d4d039, 4);
  applyCore<Core3>(d, a, b, c, _context.input[12] + 0xe6db99e5, 11);
  applyCore<Core3>(c, d, a, b, _context.input[15] + 0x1fa27cf8, 16);
  applyCore<Core3>(b, c, d, a, _context.input[2] + 0xc4ac5665, 23);

  applyCore<Core4>(a, b, c, d, _context.input[0] + 0xf4292244, 6);
  applyCore<Core4>(d, a, b, c, _context.input[7] + 0x432aff97, 10);
  applyCore<Core4>(c, d, a, b, _context.input[14] + 0xab9423a7, 15);
  applyCore<Core4>(b, c, d, a, _context.input[5] + 0xfc93a039, 21);
  applyCore<Core4>(a, b, c, d, _context.input[12] + 0x655b59c3, 6);
  applyCore<Core4>(d, a, b, c, _context.input[3] + 0x8f0ccc92, 10);
  applyCore<Core4>(c, d, a, b, _context.input[10] + 0xffeff47d, 15);
  applyCore<Core4>(b, c, d, a, _context.input[1] + 0x85845dd1, 21);
  applyCore<Core4>(a, b, c, d, _context.input[8] + 0x6fa87e4f, 6);
  applyCore<Core4>(d, a, b, c, _context.input[15] + 0xfe2ce6e0, 10);
  applyCore<Core4>(c, d, a, b, _context.input[6] + 0xa3014314, 15);
  applyCore<Core4>(b, c, d, a, _context.input[13] + 0x4e0811a1, 21);
  applyCore<Core4>(a, b, c, d, _context.input[4] + 0xf7537e82, 6);
  applyCore<Core4>(d, a, b, c, _context.input[11] + 0xbd3af235, 10);
  applyCore<Core4>(c, d, a, b, _context.input[2] + 0x2ad7d2bb, 15);
  applyCore<Core4>(b, c, d, a, _context.input[9] + 0xeb86d391, 21);

  _context.buffer[0] += a;
  _context.buffer[1] += b;
  _context.buffer[2] += c;
  _context.buffer[3] += d;
}
// Methods -
