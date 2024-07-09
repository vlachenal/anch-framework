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
#include <cstring>

#include "endianness.hpp"


namespace anch::crypto {

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  template<const std::array<W,8>& Init>
  SHA2<O,B,W,R,I>::Context<Init>::Context() {
    reset();
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  template<const std::array<W,8>& Init>
  void
  SHA2<O,B,W,R,I>::Context<Init>::reset() {
    state = Init;
    size = 0;
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  SHA2<O,B,W,R,I>::SHA2(): Hash<O,B>(), _context() {
    Hash<O,B>::_digest = &_context.digest;
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  SHA2<O,B,W,R,I>::~SHA2() {
    // Nothing to do
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  const std::array<uint8_t,O>&
  SHA2<O,B,W,R,I>::digest() const {
    return *Hash<O,B>::_digest;
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  void
  SHA2<O,B,W,R,I>::reset() {
    _context.reset();
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  void
  SHA2<O,B,W,R,I>::addData(const uint8_t* data, std::size_t len) {
    uint32_t rest = static_cast<uint32_t>(_context.size & static_cast<uint64_t>(B - 1));

    uint64_t availableData = static_cast<uint64_t>(len) + static_cast<uint64_t>(rest);
    _context.size += len;

    if(availableData < static_cast<uint64_t>(B)) {
      std::memcpy(&_context.buffer[rest], &data[0], len);

    } else {
      uint64_t i = static_cast<uint64_t>(B - rest);
      std::memcpy(&_context.buffer[rest], &data[0], static_cast<uint32_t>(i));
      transform(_context.buffer);

      uint64_t lastI = len - ((len + rest) & static_cast<uint64_t>(B - 1));
      for( ; i < lastI ; i += B) {
	transform(&data[i]);
      }

      std::memcpy(&_context.buffer[0], &data[i], len - i);
    }
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  void
  SHA2<O,B,W,R,I>::finalize() {
    uint64_t messageSize = _context.size;
    uint8_t sizeInBits[sizeof(W) * 2];
    if(anch::isLittleEndian()) {
      anch::byteSwap(messageSize << 3, sizeInBits);
    } else {
      uint64_t tmp = messageSize << 3;
      const uint8_t* words = reinterpret_cast<const uint8_t*>(&tmp);
      for(std::size_t i = 0 ; i < sizeof(W) * 2 ; ++i) {
	sizeInBits[i] = words[i];
      }
    }

    addData((const uint8_t*)"\200", 1);

    uint8_t zero[B];
    std::memset(zero, 0, B);
    if(static_cast<int>(messageSize & (B - 1)) > (B - sizeof(W) * 2 - 1)) {
      addData(zero, B - 1 - static_cast<size_t>(messageSize & (B - 1)));
      addData(zero, B - 8);
    } else {
      addData(zero, B - 1 - 8 - static_cast<size_t>(messageSize & (B - 1)));
    }

    addData(sizeInBits, 8);

    if(anch::isLittleEndian()) {
      for(std::size_t i = 0 ; i < 8 ; i++) {
	anch::byteSwap(_context.state[i], _context.digest.data() + (i * sizeof(W)));
      }
    } else {
      std::memcpy(_context.digest.data(),
		  _context.state.data(),
		  sizeof(W) * O);
    }
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  void
  SHA2<O,B,W,R,I>::transform(const uint8_t* buffer) {
    uint8_t chunkBuffer[B * sizeof(W)];
    std::memcpy(chunkBuffer, buffer, B);

    Chunk* chunk = reinterpret_cast<Chunk*>(&chunkBuffer);
    bytesSwap(chunk->words, B);
    for(std::size_t i = 16 ; i < B ; i++) {
      chunk->words[i] = sigma1(chunk->words[i-2]) + chunk->words[i-7]
	+ sigma0(chunk->words[i-15]) + chunk->words[i-16];
    }

    // Copy state[] to working vars
    W a = _context.state[0];
    W b = _context.state[1];
    W c = _context.state[2];
    W d = _context.state[3];
    W e = _context.state[4];
    W f = _context.state[5];
    W g = _context.state[6];
    W h = _context.state[7];

    const std::array<W,R>& trArray = getTranslationArray();
    for(uint32_t i = 0 ; i < R ; i++) {
      W temp1 = h + SIGMA1(e) + ch(e, f, g) + trArray[i] + chunk->words[i];
      W temp2 = SIGMA0(a) + maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + temp1;
      d = c;
      c = b;
      b = a;
      a = temp1 + temp2;
    }

    // Add the working vars back into state
    _context.state[0] += a;
    _context.state[1] += b;
    _context.state[2] += c;
    _context.state[3] += d;
    _context.state[4] += e;
    _context.state[5] += f;
    _context.state[6] += g;
    _context.state[7] += h;
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  inline void
  SHA2<O,B,W,R,I>::bytesSwap(W* buf, uint8_t count) {
    if(anch::isLittleEndian()) {
      uint8_t* words = reinterpret_cast<uint8_t*>(buf);
      do {
	anch::byteSwap(*buf,words);
	buf++;
	words += sizeof(W);
      } while(--count);
    }
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  inline W
  SHA2<O,B,W,R,I>::ch(W x, W y, W z) {
    return ((x & (y ^ z)) ^ z);
  }

  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  inline W
  SHA2<O,B,W,R,I>::maj(W x, W y, W z) {
    return ((x & (y | z)) | (y & z));
  }

}
