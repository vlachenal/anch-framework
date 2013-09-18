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
#include "crypto/hash/sha1.hpp"

#include <cstring>


using anch::crypto::SHA1;


/*!
 * Swap byte for endianness conversion
 *
 * \param src Source
 * \param dest Destination
 */
template<typename T>
inline void
byteSwap(T src, uint8_t* dest) {
  const uint32_t byteOrderTest = 0x1;
  uint8_t* words = reinterpret_cast<uint8_t*>(&src);
  int size = sizeof(T);
  if(reinterpret_cast<const uint8_t*>(&byteOrderTest)[0] == 1) { // endianess test
    for(int i = 0 ; i < size ; i++) {
      dest[i] = words[size - 1 - i];
    }
  } else {
    std::memcpy(dest, &src, size);
  }
}

/*!
 * Swap byte for endianness conversion
 *
 * \param buf The 4-bytes words to process
 * \param count The number of operation to do
 */
inline void
bytesSwap(uint32_t* buf, uint8_t count) {
  const uint32_t byteOrderTest = 0x01;
  if(reinterpret_cast<const uint8_t*>(&byteOrderTest)[0] == 1) { // endianess test
    uint8_t* words = reinterpret_cast<uint8_t*>(buf);
    do {
      byteSwap(*buf,words);
      buf++;
      words += 4;
    } while(--count);
  }
}

// Constructors +
/*!
 * \ref SHA1 default constructor
 */
SHA1::SHA1() {
  // Nothing to do
}
// Constructors -

// Destructors +
/*!
 * \ref SHA1 destructor
 */
SHA1::~SHA1() {
  // Nothing to do
}
// Destructors -

// Methods +
/*!
 * Get the SHA1 hash result
 *
 * \return the SHA1 hash result
 */
const std::array<uint8_t,20>&
SHA1::digest() const {
  return _context.digest;
}

/*!
 * Transform SHA1 with the current chunk
 *
 * \param buffer The data to process
 */
void
SHA1::transform(const uint8_t* buffer) {
  // Copy state[] to working vars
  uint32_t a = _context.state[0];
  uint32_t b = _context.state[1];
  uint32_t c = _context.state[2];
  uint32_t d = _context.state[3];
  uint32_t e = _context.state[4];

  uint8_t chunkBuffer[64];
  std::memcpy(chunkBuffer, buffer, 64);

  Chunk* chunk = reinterpret_cast<Chunk*>(&chunkBuffer);

  bytesSwap(chunk->words, 16);

  round0(*chunk,  0, a,b,c,d,e);
  round0(*chunk,  1, e,a,b,c,d);
  round0(*chunk,  2, d,e,a,b,c);
  round0(*chunk,  3, c,d,e,a,b);
  round0(*chunk,  4, b,c,d,e,a);
  round0(*chunk,  5, a,b,c,d,e);
  round0(*chunk,  6, e,a,b,c,d);
  round0(*chunk,  7, d,e,a,b,c);
  round0(*chunk,  8, c,d,e,a,b);
  round0(*chunk,  9, b,c,d,e,a);
  round0(*chunk, 10, a,b,c,d,e);
  round0(*chunk, 11, e,a,b,c,d);
  round0(*chunk, 12, d,e,a,b,c);
  round0(*chunk, 13, c,d,e,a,b);
  round0(*chunk, 14, b,c,d,e,a);
  round0(*chunk, 15, a,b,c,d,e);

  round1(*chunk, 16, e,a,b,c,d);
  round1(*chunk, 17, d,e,a,b,c);
  round1(*chunk, 18, c,d,e,a,b);
  round1(*chunk, 19, b,c,d,e,a);

  round2(*chunk, 20, a,b,c,d,e);
  round2(*chunk, 21, e,a,b,c,d);
  round2(*chunk, 22, d,e,a,b,c);
  round2(*chunk, 23, c,d,e,a,b);
  round2(*chunk, 24, b,c,d,e,a);
  round2(*chunk, 25, a,b,c,d,e);
  round2(*chunk, 26, e,a,b,c,d);
  round2(*chunk, 27, d,e,a,b,c);
  round2(*chunk, 28, c,d,e,a,b);
  round2(*chunk, 29, b,c,d,e,a);
  round2(*chunk, 30, a,b,c,d,e);
  round2(*chunk, 31, e,a,b,c,d);
  round2(*chunk, 32, d,e,a,b,c);
  round2(*chunk, 33, c,d,e,a,b);
  round2(*chunk, 34, b,c,d,e,a);
  round2(*chunk, 35, a,b,c,d,e);
  round2(*chunk, 36, e,a,b,c,d);
  round2(*chunk, 37, d,e,a,b,c);
  round2(*chunk, 38, c,d,e,a,b);
  round2(*chunk, 39, b,c,d,e,a);

  round3(*chunk, 40, a,b,c,d,e);
  round3(*chunk, 41, e,a,b,c,d);
  round3(*chunk, 42, d,e,a,b,c);
  round3(*chunk, 43, c,d,e,a,b);
  round3(*chunk, 44, b,c,d,e,a);
  round3(*chunk, 45, a,b,c,d,e);
  round3(*chunk, 46, e,a,b,c,d);
  round3(*chunk, 47, d,e,a,b,c);
  round3(*chunk, 48, c,d,e,a,b);
  round3(*chunk, 49, b,c,d,e,a);
  round3(*chunk, 50, a,b,c,d,e);
  round3(*chunk, 51, e,a,b,c,d);
  round3(*chunk, 52, d,e,a,b,c);
  round3(*chunk, 53, c,d,e,a,b);
  round3(*chunk, 54, b,c,d,e,a);
  round3(*chunk, 55, a,b,c,d,e);
  round3(*chunk, 56, e,a,b,c,d);
  round3(*chunk, 57, d,e,a,b,c);
  round3(*chunk, 58, c,d,e,a,b);
  round3(*chunk, 59, b,c,d,e,a);

  round4(*chunk, 60, a,b,c,d,e);
  round4(*chunk, 61, e,a,b,c,d);
  round4(*chunk, 62, d,e,a,b,c);
  round4(*chunk, 63, c,d,e,a,b);
  round4(*chunk, 64, b,c,d,e,a);
  round4(*chunk, 65, a,b,c,d,e);
  round4(*chunk, 66, e,a,b,c,d);
  round4(*chunk, 67, d,e,a,b,c);
  round4(*chunk, 68, c,d,e,a,b);
  round4(*chunk, 69, b,c,d,e,a);
  round4(*chunk, 70, a,b,c,d,e);
  round4(*chunk, 71, e,a,b,c,d);
  round4(*chunk, 72, d,e,a,b,c);
  round4(*chunk, 73, c,d,e,a,b);
  round4(*chunk, 74, b,c,d,e,a);
  round4(*chunk, 75, a,b,c,d,e);
  round4(*chunk, 76, e,a,b,c,d);
  round4(*chunk, 77, d,e,a,b,c);
  round4(*chunk, 78, c,d,e,a,b);
  round4(*chunk, 79, b,c,d,e,a);

  // Add the working vars back into state
  _context.state[0] += a;
  _context.state[1] += b;
  _context.state[2] += c;
  _context.state[3] += d;
  _context.state[4] += e;
}

/*!
 * Compute hash for data with the current hash
 *
 * \param data The data to add
 * \param len The data length
 */
void
SHA1::addData(const uint8_t* data, size_t len) {
  uint32_t rest = static_cast<uint32_t>(_context.size & static_cast<uint64_t>(63));

  uint64_t availableData = static_cast<uint64_t>(len) + static_cast<uint64_t>(rest);
  _context.size += len;

  if (availableData < static_cast<uint64_t>(64)) {
    std::memcpy(&_context.buffer[rest], &data[0], len);

  } else {
    uint64_t i = static_cast<uint64_t>(64 - rest);
    std::memcpy(&_context.buffer[rest], &data[0], static_cast<uint32_t>(i));
    transform(_context.buffer.data());

    uint64_t lastI = len - ((len + rest) & static_cast<uint64_t>(63));
    for( ; i < lastI ; i += 64) {
      transform(&data[i]);
    }

    std::memcpy(&_context.buffer[0], &data[i], len - i);
  }
}

/*!
 * Finalize hash
 */
void
SHA1::finalize() {
  uint64_t messageSize = _context.size;
  uint8_t sizeInBits[8];
  byteSwap(messageSize << 3, sizeInBits);

  addData((const uint8_t*)"\200", 1);

  uint8_t zero[64];
  std::memset(zero, 0, 64);
  if (static_cast<int>(messageSize & 63) > 56 - 1) {
    addData(zero, 64 - 1 - static_cast<size_t>(messageSize & 63));
    addData(zero, 64 - 8);
  } else {
    addData(zero, 64 - 1 - 8 - static_cast<size_t>(messageSize & 63));
  }

  addData(sizeInBits, 8);

  byteSwap(_context.state[0], _context.digest.data());
  byteSwap(_context.state[1], _context.digest.data() + 4);
  byteSwap(_context.state[2], _context.digest.data() + 8);
  byteSwap(_context.state[3], _context.digest.data() + 12);
  byteSwap(_context.state[4], _context.digest.data() + 16);
}
// Methods -
