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
    extern const uint8_t ANCH_AES_CIPHER_SBOX[256];

    /*! Decipher subsitution box */
    extern const uint8_t ANCH_AES_DECIPHER_SBOX[256];

    /*! Round constants */
    extern const uint32_t ANCH_AES_RCON[11];

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
      void cipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
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
      void decipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
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
       * Key expansion generic algorithm
       *
       * \param key the key to expand
       */
      void expandKey(const uint8_t key[4*K]) {
	std::memcpy(_expKey, key, 4*K);
	for(std::size_t i = K ; i < 4*(R+1) ; ++i) {
	  uint32_t mod = i % K;
	  if(mod == 0) {
	    _expKey[i] = _expKey[i-K] ^ (subWord(rotateWord(_expKey[i-1])) ^ ANCH_AES_RCON[i/K]);
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
      inline uint32_t subWord(const uint32_t& word) {
	uint32_t res;
	uint8_t* resBytes = reinterpret_cast<uint8_t*>(&res);
	const uint8_t* const bytes = reinterpret_cast<const uint8_t* const>(&word);
	resBytes[0] = ANCH_AES_CIPHER_SBOX[bytes[0]];
	resBytes[1] = ANCH_AES_CIPHER_SBOX[bytes[1]];
	resBytes[2] = ANCH_AES_CIPHER_SBOX[bytes[2]];
	resBytes[3] = ANCH_AES_CIPHER_SBOX[bytes[3]];
	return res;
      }

      /*!
       * Rotate word to 8-bits to left
       *
       * \param word the word to rotate
       *
       * \return the rotated word
       */
      inline uint32_t rotateWord(const uint32_t& word) {
	return ((word << 24) | (word >> 8));
      }

      /*!
       * Substites internal state bytes using substituion box defined in the
       * AES specification.
       */
      inline void cipherSubBytes() {
	_state[0][0] = ANCH_AES_CIPHER_SBOX[_state[0][0]];
	_state[0][1] = ANCH_AES_CIPHER_SBOX[_state[0][1]];
	_state[0][2] = ANCH_AES_CIPHER_SBOX[_state[0][2]];
	_state[0][3] = ANCH_AES_CIPHER_SBOX[_state[0][3]];
	_state[1][0] = ANCH_AES_CIPHER_SBOX[_state[1][0]];
	_state[1][1] = ANCH_AES_CIPHER_SBOX[_state[1][1]];
	_state[1][2] = ANCH_AES_CIPHER_SBOX[_state[1][2]];
	_state[1][3] = ANCH_AES_CIPHER_SBOX[_state[1][3]];
	_state[2][0] = ANCH_AES_CIPHER_SBOX[_state[2][0]];
	_state[2][1] = ANCH_AES_CIPHER_SBOX[_state[2][1]];
	_state[2][2] = ANCH_AES_CIPHER_SBOX[_state[2][2]];
	_state[2][3] = ANCH_AES_CIPHER_SBOX[_state[2][3]];
	_state[3][0] = ANCH_AES_CIPHER_SBOX[_state[3][0]];
	_state[3][1] = ANCH_AES_CIPHER_SBOX[_state[3][1]];
	_state[3][2] = ANCH_AES_CIPHER_SBOX[_state[3][2]];
	_state[3][3] = ANCH_AES_CIPHER_SBOX[_state[3][3]];
      }

      /*!
       * Substites internal state bytes using substituion box defined in the
       * AES specification.
       */
      inline void decipherSubBytes() {
	_state[0][0] = ANCH_AES_DECIPHER_SBOX[_state[0][0]];
	_state[0][1] = ANCH_AES_DECIPHER_SBOX[_state[0][1]];
	_state[0][2] = ANCH_AES_DECIPHER_SBOX[_state[0][2]];
	_state[0][3] = ANCH_AES_DECIPHER_SBOX[_state[0][3]];
	_state[1][0] = ANCH_AES_DECIPHER_SBOX[_state[1][0]];
	_state[1][1] = ANCH_AES_DECIPHER_SBOX[_state[1][1]];
	_state[1][2] = ANCH_AES_DECIPHER_SBOX[_state[1][2]];
	_state[1][3] = ANCH_AES_DECIPHER_SBOX[_state[1][3]];
	_state[2][0] = ANCH_AES_DECIPHER_SBOX[_state[2][0]];
	_state[2][1] = ANCH_AES_DECIPHER_SBOX[_state[2][1]];
	_state[2][2] = ANCH_AES_DECIPHER_SBOX[_state[2][2]];
	_state[2][3] = ANCH_AES_DECIPHER_SBOX[_state[2][3]];
	_state[3][0] = ANCH_AES_DECIPHER_SBOX[_state[3][0]];
	_state[3][1] = ANCH_AES_DECIPHER_SBOX[_state[3][1]];
	_state[3][2] = ANCH_AES_DECIPHER_SBOX[_state[3][2]];
	_state[3][3] = ANCH_AES_DECIPHER_SBOX[_state[3][3]];
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

	state[0] = _state[0][0];
	state[1] = _state[0][1];
	state[2] = _state[0][2];
	state[3] = _state[0][3];
	_state[0][0] = ANCH_GALOIS_MULT2[state[0]] ^ ANCH_GALOIS_MULT3[state[1]] ^ state[2] ^ state[3];
	_state[0][1] = state[0] ^ ANCH_GALOIS_MULT2[state[1]] ^ ANCH_GALOIS_MULT3[state[2]] ^ state[3];
	_state[0][2] = state[0] ^ state[1] ^ ANCH_GALOIS_MULT2[state[2]] ^ ANCH_GALOIS_MULT3[state[3]];
	_state[0][3] = ANCH_GALOIS_MULT3[state[0]] ^ state[1] ^ state[2] ^ ANCH_GALOIS_MULT2[state[3]];

	state[0] = _state[1][0];
	state[1] = _state[1][1];
	state[2] = _state[1][2];
	state[3] = _state[1][3];
	_state[1][0] = ANCH_GALOIS_MULT2[state[0]] ^ ANCH_GALOIS_MULT3[state[1]] ^ state[2] ^ state[3];
	_state[1][1] = state[0] ^ ANCH_GALOIS_MULT2[state[1]] ^ ANCH_GALOIS_MULT3[state[2]] ^ state[3];
	_state[1][2] = state[0] ^ state[1] ^ ANCH_GALOIS_MULT2[state[2]] ^ ANCH_GALOIS_MULT3[state[3]];
	_state[1][3] = ANCH_GALOIS_MULT3[state[0]] ^ state[1] ^ state[2] ^ ANCH_GALOIS_MULT2[state[3]];

	state[0] = _state[2][0];
	state[1] = _state[2][1];
	state[2] = _state[2][2];
	state[3] = _state[2][3];
	_state[2][0] = ANCH_GALOIS_MULT2[state[0]] ^ ANCH_GALOIS_MULT3[state[1]] ^ state[2] ^ state[3];
	_state[2][1] = state[0] ^ ANCH_GALOIS_MULT2[state[1]] ^ ANCH_GALOIS_MULT3[state[2]] ^ state[3];
	_state[2][2] = state[0] ^ state[1] ^ ANCH_GALOIS_MULT2[state[2]] ^ ANCH_GALOIS_MULT3[state[3]];
	_state[2][3] = ANCH_GALOIS_MULT3[state[0]] ^ state[1] ^ state[2] ^ ANCH_GALOIS_MULT2[state[3]];

	state[0] = _state[3][0];
	state[1] = _state[3][1];
	state[2] = _state[3][2];
	state[3] = _state[3][3];
	_state[3][0] = ANCH_GALOIS_MULT2[state[0]] ^ ANCH_GALOIS_MULT3[state[1]] ^ state[2] ^ state[3];
	_state[3][1] = state[0] ^ ANCH_GALOIS_MULT2[state[1]] ^ ANCH_GALOIS_MULT3[state[2]] ^ state[3];
	_state[3][2] = state[0] ^ state[1] ^ ANCH_GALOIS_MULT2[state[2]] ^ ANCH_GALOIS_MULT3[state[3]];
	_state[3][3] = ANCH_GALOIS_MULT3[state[0]] ^ state[1] ^ state[2] ^ ANCH_GALOIS_MULT2[state[3]];
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
	_state[0][0] = ANCH_GALOIS_MULT14[state[0]] ^ ANCH_GALOIS_MULT11[state[1]] ^ ANCH_GALOIS_MULT13[state[2]] ^ ANCH_GALOIS_MULT9[state[3]];
	_state[0][1] = ANCH_GALOIS_MULT9[state[0]] ^ ANCH_GALOIS_MULT14[state[1]] ^ ANCH_GALOIS_MULT11[state[2]] ^ ANCH_GALOIS_MULT13[state[3]];
	_state[0][2] = ANCH_GALOIS_MULT13[state[0]] ^ ANCH_GALOIS_MULT9[state[1]] ^ ANCH_GALOIS_MULT14[state[2]] ^ ANCH_GALOIS_MULT11[state[3]];
	_state[0][3] = ANCH_GALOIS_MULT11[state[0]] ^ ANCH_GALOIS_MULT13[state[1]] ^ ANCH_GALOIS_MULT9[state[2]] ^ ANCH_GALOIS_MULT14[state[3]];

	state[0] = _state[1][0];
	state[1] = _state[1][1];
	state[2] = _state[1][2];
	state[3] = _state[1][3];
	_state[1][0] = ANCH_GALOIS_MULT14[state[0]] ^ ANCH_GALOIS_MULT11[state[1]] ^ ANCH_GALOIS_MULT13[state[2]] ^ ANCH_GALOIS_MULT9[state[3]];
	_state[1][1] = ANCH_GALOIS_MULT9[state[0]] ^ ANCH_GALOIS_MULT14[state[1]] ^ ANCH_GALOIS_MULT11[state[2]] ^ ANCH_GALOIS_MULT13[state[3]];
	_state[1][2] = ANCH_GALOIS_MULT13[state[0]] ^ ANCH_GALOIS_MULT9[state[1]] ^ ANCH_GALOIS_MULT14[state[2]] ^ ANCH_GALOIS_MULT11[state[3]];
	_state[1][3] = ANCH_GALOIS_MULT11[state[0]] ^ ANCH_GALOIS_MULT13[state[1]] ^ ANCH_GALOIS_MULT9[state[2]] ^ ANCH_GALOIS_MULT14[state[3]];

	state[0] = _state[2][0];
	state[1] = _state[2][1];
	state[2] = _state[2][2];
	state[3] = _state[2][3];
	_state[2][0] = ANCH_GALOIS_MULT14[state[0]] ^ ANCH_GALOIS_MULT11[state[1]] ^ ANCH_GALOIS_MULT13[state[2]] ^ ANCH_GALOIS_MULT9[state[3]];
	_state[2][1] = ANCH_GALOIS_MULT9[state[0]] ^ ANCH_GALOIS_MULT14[state[1]] ^ ANCH_GALOIS_MULT11[state[2]] ^ ANCH_GALOIS_MULT13[state[3]];
	_state[2][2] = ANCH_GALOIS_MULT13[state[0]] ^ ANCH_GALOIS_MULT9[state[1]] ^ ANCH_GALOIS_MULT14[state[2]] ^ ANCH_GALOIS_MULT11[state[3]];
	_state[2][3] = ANCH_GALOIS_MULT11[state[0]] ^ ANCH_GALOIS_MULT13[state[1]] ^ ANCH_GALOIS_MULT9[state[2]] ^ ANCH_GALOIS_MULT14[state[3]];

	state[0] = _state[3][0];
	state[1] = _state[3][1];
	state[2] = _state[3][2];
	state[3] = _state[3][3];
	_state[3][0] = ANCH_GALOIS_MULT14[state[0]] ^ ANCH_GALOIS_MULT11[state[1]] ^ ANCH_GALOIS_MULT13[state[2]] ^ ANCH_GALOIS_MULT9[state[3]];
	_state[3][1] = ANCH_GALOIS_MULT9[state[0]] ^ ANCH_GALOIS_MULT14[state[1]] ^ ANCH_GALOIS_MULT11[state[2]] ^ ANCH_GALOIS_MULT13[state[3]];
	_state[3][2] = ANCH_GALOIS_MULT13[state[0]] ^ ANCH_GALOIS_MULT9[state[1]] ^ ANCH_GALOIS_MULT14[state[2]] ^ ANCH_GALOIS_MULT11[state[3]];
	_state[3][3] = ANCH_GALOIS_MULT11[state[0]] ^ ANCH_GALOIS_MULT13[state[1]] ^ ANCH_GALOIS_MULT9[state[2]] ^ ANCH_GALOIS_MULT14[state[3]];
      }

      /*!
       * Add round key (cipher)
       *
       * \param round the current round
       */
      inline void addRoundKey(const uint32_t& round) {
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
