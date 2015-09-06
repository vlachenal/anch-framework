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
#ifndef _ANCH_CRYPTO_AES_H_
#define _ANCH_CRYPTO_AES_H_

#include "crypto/cipher/blockCipher.hpp"

#include <bitset>
#include <cstring>


namespace anch {
  namespace crypto {

    /*! Cipher subsitution box */
    extern const uint8_t CIPHER_SUB_BOX[256];

    /*! Decipher subsitution box */
    extern const uint8_t DECIPHER_SUB_BOX[256];

    /*! Round constants */
    extern const uint32_t RCON[11];

    /*!
     * \brief AES block cipher algorithm implementation.
     *
     * This class only implements the key management and the cipher/decipher of one block.\n
     * See standard specification for more details (http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf).
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    template<std::size_t K, std::size_t R>
    class AES: public anch::crypto::BlockCipher<16> {
      // Attributes +
    private:
      /*! Internal state */
      uint8_t _state[4][4];

      /*! Expanded key */
      uint32_t _expKey[4*(R+1)];
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref AES constructor
       *
       * \param key the encryption key
       */
      AES(const uint8_t key[4*K]): _state(), _expKey() {
	expandKey(key);
      }

      /*!
       * \ref AES copy constructor.\n
       * This constructor will copy the expanded key to avoid to compute it another time.
       *
       * \param other the AES to copy
       */
      AES(const AES& other): _state(), _expKey() {
	std::memcpy(_expKey, other._expKey, 4 * (R + 1) * sizeof(uint32_t));
      }
      // Constructors -


      // Destructors +
      /*!
       * \ref AES destructor
       */
      virtual ~AES() {
	// Nothing to do
      }
      // Destructors -


      // Methods +
    public:
      /*!
       * Cipher a block
       *
       * \param input the block to cipher
       * \param output the ciphered block
       */
      virtual void cipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
	std::memcpy(&_state, input.data(), 16);
	unsigned int round = 0;

	// Initial round +
	addRoundKey(round);
	// Initial round -

	// Rounds +
	for(round = 1 ; round < R ; ++round) {
	  cipherSubBytes();
	  cipherShiftRows();
	  cipherMixColumns();
	  addRoundKey(round);
	}
	// Rounds -

	// Final round +
	cipherSubBytes();
	cipherShiftRows();
	addRoundKey(round);
	// Final round -

	std::memcpy(output.data(), &_state, 16);
      }

      /*!
       * Decipher a block
       *
       * \param input the block to decipher
       * \param output the deciphered block
       */
      virtual void decipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
	std::memcpy(&_state, input.data(), 16);
	unsigned int round = R;

	// Initial round +
	addRoundKey(round);
	decipherShiftRows();
	decipherSubBytes();
	// Initial round -

	// Rounds +
	for(round = R - 1 ; round > 0 ; --round) {
	  addRoundKey(round);
	  decipherMixColumns();
	  decipherShiftRows();
	  decipherSubBytes();
	}
	// Rounds -

	// Final round +
	addRoundKey(round);
	// Final round -

	std::memcpy(output.data(), &_state, 16);
      }

    private:
      /*!
       * Key expansion algorithm
       *
       * \param key the key to expand
       */
      void expandKey(const uint8_t key[4*K]) {
	std::memcpy(_expKey, key, 4*K);
	for(std::size_t i = K ; i < 4*(R+1) ; ++i) {
	  uint32_t mod = i % K;
	  if(mod == 0) {
	    _expKey[i] = _expKey[i-K] ^ (subWord(rotateWord(_expKey[i-1])) ^ RCON[i/K]);
	  } else if(K > 6 && mod == 4) {
	    _expKey[i] = _expKey[i-K] ^ subWord(_expKey[i-1]);
	  } else {
	    _expKey[i] = _expKey[i-K] ^ _expKey[i-1];
	  }
	}
      }

      /*!
       * Substitutes 32-bits word using substituion box defined in the
       * AES specification.
       *
       * \param word the word to treat
       *
       * \return the result of substitution
       */
      inline uint32_t subWord(uint32_t word) {
	uint32_t res;
	uint8_t* resBytes = reinterpret_cast<uint8_t*>(&res);
	uint8_t* bytes = reinterpret_cast<uint8_t*>(&word);
	resBytes[0] = CIPHER_SUB_BOX[bytes[0]];
	resBytes[1] = CIPHER_SUB_BOX[bytes[1]];
	resBytes[2] = CIPHER_SUB_BOX[bytes[2]];
	resBytes[3] = CIPHER_SUB_BOX[bytes[3]];
	return res;
      }

      /*!
       * Rotate word to 8-bits to left
       *
       * \param word the word to rotate
       *
       * \return the rotated word
       */
      inline uint32_t rotateWord(uint32_t word) {
	return ((word << 24) | (word >> 8));
      }

      /*!
       * Substites internal state bytes using substituion box defined in the
       * AES specification.
       */
      inline void cipherSubBytes() {
	_state[0][0] = CIPHER_SUB_BOX[_state[0][0]];
	_state[0][1] = CIPHER_SUB_BOX[_state[0][1]];
	_state[0][2] = CIPHER_SUB_BOX[_state[0][2]];
	_state[0][3] = CIPHER_SUB_BOX[_state[0][3]];
	_state[1][0] = CIPHER_SUB_BOX[_state[1][0]];
	_state[1][1] = CIPHER_SUB_BOX[_state[1][1]];
	_state[1][2] = CIPHER_SUB_BOX[_state[1][2]];
	_state[1][3] = CIPHER_SUB_BOX[_state[1][3]];
	_state[2][0] = CIPHER_SUB_BOX[_state[2][0]];
	_state[2][1] = CIPHER_SUB_BOX[_state[2][1]];
	_state[2][2] = CIPHER_SUB_BOX[_state[2][2]];
	_state[2][3] = CIPHER_SUB_BOX[_state[2][3]];
	_state[3][0] = CIPHER_SUB_BOX[_state[3][0]];
	_state[3][1] = CIPHER_SUB_BOX[_state[3][1]];
	_state[3][2] = CIPHER_SUB_BOX[_state[3][2]];
	_state[3][3] = CIPHER_SUB_BOX[_state[3][3]];
      }

      /*!
       * Substites internal state bytes using substituion box defined in the
       * AES specification.
       */
      inline void decipherSubBytes() {
	_state[0][0] = DECIPHER_SUB_BOX[_state[0][0]];
	_state[0][1] = DECIPHER_SUB_BOX[_state[0][1]];
	_state[0][2] = DECIPHER_SUB_BOX[_state[0][2]];
	_state[0][3] = DECIPHER_SUB_BOX[_state[0][3]];
	_state[1][0] = DECIPHER_SUB_BOX[_state[1][0]];
	_state[1][1] = DECIPHER_SUB_BOX[_state[1][1]];
	_state[1][2] = DECIPHER_SUB_BOX[_state[1][2]];
	_state[1][3] = DECIPHER_SUB_BOX[_state[1][3]];
	_state[2][0] = DECIPHER_SUB_BOX[_state[2][0]];
	_state[2][1] = DECIPHER_SUB_BOX[_state[2][1]];
	_state[2][2] = DECIPHER_SUB_BOX[_state[2][2]];
	_state[2][3] = DECIPHER_SUB_BOX[_state[2][3]];
	_state[3][0] = DECIPHER_SUB_BOX[_state[3][0]];
	_state[3][1] = DECIPHER_SUB_BOX[_state[3][1]];
	_state[3][2] = DECIPHER_SUB_BOX[_state[3][2]];
	_state[3][3] = DECIPHER_SUB_BOX[_state[3][3]];
      }

      /*!
       * Shift internal state rows (cipher)
       */
      inline void cipherShiftRows() {
	uint8_t tmp = _state[0][1];
	_state[0][1] = _state[1][1];
	_state[1][1] = _state[2][1];
	_state[2][1] = _state[3][1];
	_state[3][1] = tmp;
	tmp = _state[1][2];
	_state[1][2] = _state[3][2];
	_state[3][2] = tmp;
	tmp = _state[2][2];
	_state[2][2] = _state[0][2];
	_state[0][2] = tmp;
	tmp = _state[3][3];
	_state[3][3] = _state[2][3];
	_state[2][3] = _state[1][3];
	_state[1][3] = _state[0][3];
	_state[0][3] = tmp;
      }

      /*!
       * Shift internal state rows (decipher)
       */
      inline void decipherShiftRows() {
	uint8_t tmp = _state[3][1];
	_state[3][1] = _state[2][1];
	_state[2][1] = _state[1][1];
	_state[1][1] = _state[0][1];
	_state[0][1] = tmp;
	tmp = _state[1][2];
	_state[1][2] = _state[3][2];
	_state[3][2] = tmp;
	tmp = _state[2][2];
	_state[2][2] = _state[0][2];
	_state[0][2] = tmp;
	tmp = _state[0][3];
	_state[0][3] = _state[1][3];
	_state[1][3] = _state[2][3];
	_state[2][3] = _state[3][3];
	_state[3][3] = tmp;
      }

      /*!
       * Mix internal by columns (cipher)
       */
      inline void cipherMixColumns() {
	uint8_t state[4];
	uint8_t state2[4];

	state[0] = _state[0][0];
	state[1] = _state[0][1];
	state[2] = _state[0][2];
	state[3] = _state[0][3];
	state2[0] = (_state[0][0] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[0][0] >> 7));
	state2[1] = (_state[0][1] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[0][1] >> 7));
	state2[2] = (_state[0][2] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[0][2] >> 7));
	state2[3] = (_state[0][3] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[0][3] >> 7));
	_state[0][0] = state2[0] ^ state[3] ^ state[2] ^ state2[1] ^ state[1];
	_state[0][1] = state2[1] ^ state[0] ^ state[3] ^ state2[2] ^ state[2];
	_state[0][2] = state2[2] ^ state[1] ^ state[0] ^ state2[3] ^ state[3];
	_state[0][3] = state2[3] ^ state[2] ^ state[1] ^ state2[0] ^ state[0];

	state[0] = _state[1][0];
	state[1] = _state[1][1];
	state[2] = _state[1][2];
	state[3] = _state[1][3];
	state2[0] = (_state[1][0] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[1][0] >> 7));
	state2[1] = (_state[1][1] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[1][1] >> 7));
	state2[2] = (_state[1][2] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[1][2] >> 7));
	state2[3] = (_state[1][3] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[1][3] >> 7));
	_state[1][0] = state2[0] ^ state[3] ^ state[2] ^ state2[1] ^ state[1];
	_state[1][1] = state2[1] ^ state[0] ^ state[3] ^ state2[2] ^ state[2];
	_state[1][2] = state2[2] ^ state[1] ^ state[0] ^ state2[3] ^ state[3];
	_state[1][3] = state2[3] ^ state[2] ^ state[1] ^ state2[0] ^ state[0];

	state[0] = _state[2][0];
	state[1] = _state[2][1];
	state[2] = _state[2][2];
	state[3] = _state[2][3];
	state2[0] = (_state[2][0] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[2][0] >> 7));
	state2[1] = (_state[2][1] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[2][1] >> 7));
	state2[2] = (_state[2][2] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[2][2] >> 7));
	state2[3] = (_state[2][3] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[2][3] >> 7));
	_state[2][0] = state2[0] ^ state[3] ^ state[2] ^ state2[1] ^ state[1];
	_state[2][1] = state2[1] ^ state[0] ^ state[3] ^ state2[2] ^ state[2];
	_state[2][2] = state2[2] ^ state[1] ^ state[0] ^ state2[3] ^ state[3];
	_state[2][3] = state2[3] ^ state[2] ^ state[1] ^ state2[0] ^ state[0];

	state[0] = _state[3][0];
	state[1] = _state[3][1];
	state[2] = _state[3][2];
	state[3] = _state[3][3];
	state2[0] = (_state[3][0] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[3][0] >> 7));
	state2[1] = (_state[3][1] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[3][1] >> 7));
	state2[2] = (_state[3][2] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[3][2] >> 7));
	state2[3] = (_state[3][3] << 1) ^ (0x1B & (uint8_t)((int8_t)_state[3][3] >> 7));
	_state[3][0] = state2[0] ^ state[3] ^ state[2] ^ state2[1] ^ state[1];
	_state[3][1] = state2[1] ^ state[0] ^ state[3] ^ state2[2] ^ state[2];
	_state[3][2] = state2[2] ^ state[1] ^ state[0] ^ state2[3] ^ state[3];
	_state[3][3] = state2[3] ^ state[2] ^ state[1] ^ state2[0] ^ state[0];
      }

      /*!
       * Mix internal by columns (decipher)
       */
      inline void decipherMixColumns() {
	uint8_t state[4];

	state[0] = _state[0][0];
	state[1] = _state[0][1];
	state[2] = _state[0][2];
	state[3] = _state[0][3];
	_state[0][0] = mult(0x0E, state[0]) ^ mult(0x0B, state[1]) ^ mult(0x0D, state[2]) ^ mult(0x09, state[3]);
	_state[0][1] = mult(0x09, state[0]) ^ mult(0x0E, state[1]) ^ mult(0x0B, state[2]) ^ mult(0x0D, state[3]);
	_state[0][2] = mult(0x0D, state[0]) ^ mult(0x09, state[1]) ^ mult(0x0E, state[2]) ^ mult(0x0B, state[3]);
	_state[0][3] = mult(0x0B, state[0]) ^ mult(0x0D, state[1]) ^ mult(0x09, state[2]) ^ mult(0x0E, state[3]);

	state[0] = _state[1][0];
	state[1] = _state[1][1];
	state[2] = _state[1][2];
	state[3] = _state[1][3];
	_state[1][0] = mult(0x0E, state[0]) ^ mult(0x0B, state[1]) ^ mult(0x0D, state[2]) ^ mult(0x09, state[3]);
	_state[1][1] = mult(0x09, state[0]) ^ mult(0x0E, state[1]) ^ mult(0x0B, state[2]) ^ mult(0x0D, state[3]);
	_state[1][2] = mult(0x0D, state[0]) ^ mult(0x09, state[1]) ^ mult(0x0E, state[2]) ^ mult(0x0B, state[3]);
	_state[1][3] = mult(0x0B, state[0]) ^ mult(0x0D, state[1]) ^ mult(0x09, state[2]) ^ mult(0x0E, state[3]);

	state[0] = _state[2][0];
	state[1] = _state[2][1];
	state[2] = _state[2][2];
	state[3] = _state[2][3];
	_state[2][0] = mult(0x0E, state[0]) ^ mult(0x0B, state[1]) ^ mult(0x0D, state[2]) ^ mult(0x09, state[3]);
	_state[2][1] = mult(0x09, state[0]) ^ mult(0x0E, state[1]) ^ mult(0x0B, state[2]) ^ mult(0x0D, state[3]);
	_state[2][2] = mult(0x0D, state[0]) ^ mult(0x09, state[1]) ^ mult(0x0E, state[2]) ^ mult(0x0B, state[3]);
	_state[2][3] = mult(0x0B, state[0]) ^ mult(0x0D, state[1]) ^ mult(0x09, state[2]) ^ mult(0x0E, state[3]);

	state[0] = _state[3][0];
	state[1] = _state[3][1];
	state[2] = _state[3][2];
	state[3] = _state[3][3];
	_state[3][0] = mult(0x0E, state[0]) ^ mult(0x0B, state[1]) ^ mult(0x0D, state[2]) ^ mult(0x09, state[3]);
	_state[3][1] = mult(0x09, state[0]) ^ mult(0x0E, state[1]) ^ mult(0x0B, state[2]) ^ mult(0x0D, state[3]);
	_state[3][2] = mult(0x0D, state[0]) ^ mult(0x09, state[1]) ^ mult(0x0E, state[2]) ^ mult(0x0B, state[3]);
	_state[3][3] = mult(0x0B, state[0]) ^ mult(0x0D, state[1]) ^ mult(0x09, state[2]) ^ mult(0x0E, state[3]);
      }

      /*!
       * Galois Field multiplication implementation
       *
       * \param a the first operand
       * \param b the second operand
       *
       * \return the multiplication result
       */
      inline uint8_t mult(uint8_t a, uint8_t b) {
	uint8_t res = 0;
	uint8_t highBitSet = 0;
	mult(a, b, res, highBitSet);
	mult(a, b, res, highBitSet);
	mult(a, b, res, highBitSet);
	mult(a, b, res, highBitSet);
	mult(a, b, res, highBitSet);
	mult(a, b, res, highBitSet);
	mult(a, b, res, highBitSet);
	mult(a, b, res, highBitSet);
	return res;
      }

      /*!
       * Galois Field multiplication implementation single round
       *
       * \param a the first operand
       * \param b the second operand
       * \param res the result
       * \param highBitSet the high bit set
       */
      inline void mult(uint8_t& a, uint8_t& b, uint8_t& res, uint8_t& highBitSet) {
	if((b & 1) != 0) {
	  res ^= a;
	}
	highBitSet = a & 0x80;
	a <<= 1;
	if(highBitSet != 0) {
	  a ^= 0x1b; // x^8 + x^4 + x^3 + x + 1
	}
	b >>= 1;
      }

      /*!
       * Add round key (cipher)
       *
       * \param round the current round
       */
      inline void addRoundKey(unsigned int round) {
	uint32_t* key = _expKey + 4 * round;
	uint32_t* state = reinterpret_cast<uint32_t*>(_state);
	state[0] ^= key[0];
	state[1] ^= key[1];
	state[2] ^= key[2];
	state[3] ^= key[3];
      }
      // Methods -

    };

  }
}

#endif // _ANCH_CRYPTO_AES_H_
