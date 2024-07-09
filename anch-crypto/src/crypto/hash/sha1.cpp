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
#include "endianness.hpp"

#include <cstring>

#include "uuid.hpp"


using anch::crypto::Hash;
using anch::crypto::SHA1;
using anch::UUID;

template class Hash<20,64>;


typedef union {
  /*! Bytes */
  uint8_t bytes[64];

  /*! Words */
  uint32_t words[16];
} SHA1Chunk;

// Functions +
/*!
 * Swap byte for endianness conversion
 *
 * \param buf The 4-bytes words to process
 * \param count The number of operation to do
 */
inline void
bytesSwap(uint32_t* buf, uint8_t count) {
  if(anch::isLittleEndian()) {
    uint8_t* words = reinterpret_cast<uint8_t*>(buf);
    do {
      anch::byteSwap(*buf,words);
      buf++;
      words += 4;
    } while(--count);
  }
}

/*!
 * Bits rotation of 32 bits integer from a value
 *
 * \param value The integer to process
 * \param shift The number of bits to shift
 */
inline uint32_t
rol32(uint32_t value, unsigned int shift){
  return ((value << shift) | value >> (32 - shift));
}

/*!
 * Compute SHA1 word
 *
 * \param chunk The SHA1 chunk
 * \param position The position
 */
inline uint32_t
word(SHA1Chunk& chunk, unsigned int position) {
  return (chunk.words[position & 0xf] = rol32(chunk.words[(position + 13)  & 0xf]
					      ^ chunk.words[(position + 8) & 0xf]
					      ^ chunk.words[(position + 2) & 0xf]
					      ^ chunk.words[(position)     & 0xf],
					      1));
}

/*!
 * First core function
 *
 * \param chunk The chunk to process
 * \param position The position
 * \param v The first parameter
 * \param w The second parameter
 * \param x The third parameter
 * \param y The fourth parameter
 * \param z The fifth parameter
 */
inline void
round0(SHA1Chunk& chunk,
       const unsigned int position,
       uint32_t& v,
       uint32_t& w,
       uint32_t& x,
       uint32_t& y,
       uint32_t& z) {
  z += ((( w & (x ^ y)) ^ y) + chunk.words[position] + 0x5A827999 + rol32(v, 5));
  w = rol32(w, 30);
}

/*!
 * Second core function
 *
 * \param chunk The chunk to process
 * \param position The position
 * \param v The first parameter
 * \param w The second parameter
 * \param x The third parameter
 * \param y The fourth parameter
 * \param z The fifth parameter
 */
inline void
round1(SHA1Chunk& chunk,
       const unsigned int position,
       uint32_t& v,
       uint32_t& w,
       uint32_t& x,
       uint32_t& y,
       uint32_t& z) {
  z += ((( w & (x ^ y)) ^ y) + word(chunk,position) + 0x5A827999 + rol32(v, 5));
  w = rol32(w, 30);
}

/*!
 * Third core function
 *
 * \param chunk The chunk to process
 * \param position The position
 * \param v The first parameter
 * \param w The second parameter
 * \param x The third parameter
 * \param y The fourth parameter
 * \param z The fifth parameter
 */
inline void
round2(SHA1Chunk& chunk,
       const unsigned int position,
       uint32_t& v,
       uint32_t& w,
       uint32_t& x,
       uint32_t& y,
       uint32_t& z) {
  z += (( w ^ x ^ y) + word(chunk, position) + 0x6ED9EBA1 + rol32(v, 5));
  w = rol32(w, 30);
}

/*!
 * Fourth core function
 *
 * \param chunk The chunk to process
 * \param position The position
 * \param v The first parameter
 * \param w The second parameter
 * \param x The third parameter
 * \param y The fourth parameter
 * \param z The fifth parameter
 */
inline void
round3(SHA1Chunk& chunk,
       const unsigned int position,
       uint32_t& v,
       uint32_t& w,
       uint32_t& x,
       uint32_t& y,
       uint32_t& z) {
  z += (((( w | x) & y) | (w & x)) + word(chunk, position) + 0x8F1BBCDC + rol32(v, 5));
  w = rol32(w, 30);
}

/*!
 * Fifth core function
 *
 * \param chunk The chunk to process
 * \param position The position
 * \param v The first parameter
 * \param w The second parameter
 * \param x The third parameter
 * \param y The fourth parameter
 * \param z The fifth parameter
 */
inline void
round4(SHA1Chunk& chunk,
       const unsigned int position,
       uint32_t& v,
       uint32_t& w,
       uint32_t& x,
       uint32_t& y,
       uint32_t& z) {
  z += ((w ^ x ^ y) + word(chunk, position) + 0xCA62C1D6 + rol32(v, 5));
  w = rol32(w, 30);
}
// Functions -

SHA1::Context::Context() {
  reset();
}

void
SHA1::Context::reset() {
  state = { {0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476,0xC3D2E1F0} };
  size = 0;
}


// Constructors +
SHA1::SHA1() {
  // Nothing to do
}

SHA1::SHA1(const std::string& data) {
  Hash::digest(data);
}

SHA1::SHA1(std::istream& stream) {
  Hash::digest(stream);
}

SHA1::SHA1(const uint8_t* data, std::size_t len) {
  Hash::digest(data, len);
}
// Constructors -

// Destructors +
SHA1::~SHA1() {
  // Nothing to do
}
// Destructors -

// Methods +
const std::array<uint8_t,20>&
SHA1::digest() const {
  return _context.digest;
}

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

  SHA1Chunk* chunk = reinterpret_cast<SHA1Chunk*>(&chunkBuffer);

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

void
SHA1::reset() {
  _context.reset();
}

void
SHA1::finalize() {
  uint64_t messageSize = _context.size;
  uint8_t sizeInBits[8];
  if(anch::isLittleEndian()) {
    anch::byteSwap(messageSize << 3, sizeInBits);
  } else {
    uint64_t tmp = messageSize << 3;
    std::memcpy(sizeInBits, &tmp, 8);
  }

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

  if(anch::isLittleEndian()) {
    anch::byteSwap(_context.state[0], _context.digest.data());
    anch::byteSwap(_context.state[1], _context.digest.data() + 4);
    anch::byteSwap(_context.state[2], _context.digest.data() + 8);
    anch::byteSwap(_context.state[3], _context.digest.data() + 12);
    anch::byteSwap(_context.state[4], _context.digest.data() + 16);
  } else {
    std::memcpy(_context.digest.data(),
		_context.state.data(),
		sizeof(uint32_t) * _context.state.size());
  }
}
// Methods -

UUID
generateSHA1UUID(const std::string& data) {
  SHA1 hash(data);
  const std::array<uint8_t,20>& digest = hash.digest();

  // Timestamp +
  uint32_t lowTime = digest[0]
    + (static_cast<uint32_t>(digest[1]) << 8)
    + (static_cast<uint32_t>(digest[2]) << 16)
    + (static_cast<uint32_t>(digest[3]) << 24);
  uint16_t midTime = static_cast<uint16_t>(digest[4] + (static_cast<uint32_t>(digest[5]) << 8));
  uint16_t highTime = static_cast<uint16_t>(digest[6] + (static_cast<uint32_t>(digest[7]) << 8));
  highTime = highTime & UUID::TIME_HIGH_MASK;
  // Timestamp -

  // Sequence +
  uint16_t clockSeqLow = digest[9];
  uint16_t clockSeqHighRes = digest[8];
  clockSeqHighRes |= 0x80;
  // Sequence -

  // Node +
  uint64_t node = digest[10]
    + (static_cast<uint64_t>(digest[11]) << 8)
    + (static_cast<uint64_t>(digest[12]) << 16)
    + (static_cast<uint64_t>(digest[13]) << 24)
    + (static_cast<uint64_t>(digest[14]) << 32)
    + (static_cast<uint64_t>(digest[15]) << 40);
  // Node -

  return UUID(lowTime, midTime, highTime, clockSeqLow, clockSeqHighRes, node, UUID::Version::SHA1_HASH);
}

void
anch::crypto::registerSHA1UUIDProvider() {
  UUID::registerProvider(UUID::Version::SHA1_HASH, &generateSHA1UUID);
}
