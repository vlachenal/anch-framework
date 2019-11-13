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
#pragma once

#include "crypto/cipher/blockCipher.hpp"

#include <bitset>
#include <cstring>

#ifdef ANCH_CPU_DETECTION
#include "device/cpu.hpp"
#endif

#ifdef ANCH_CPU_AES
#include <wmmintrin.h>
#include <smmintrin.h>
#endif

#if !defined (ALIGN16)
# if defined (__GNUC__)
# define ALIGN16 __attribute__ ( (aligned (16)))
# else
# define ALIGN16 __declspec (align (16))
# endif
#endif


namespace anch {
  namespace crypto {

    /*! Cipher subsitution box */
    extern const uint8_t ANCH_AES_CIPHER_SBOX[256];

    /*! Decipher subsitution box */
    extern const uint8_t ANCH_AES_DECIPHER_SBOX[256];

    /*! Round constants */
    extern const uint32_t ANCH_AES_RCON[11];

#ifdef ANCH_CPU_AES
    template<std::size_t S>
    struct AesniKey {
      ALIGN16 __m128i cipherKey[S];
      ALIGN16 __m128i decipherKey[S];
    };
#endif

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
#if defined ANCH_CPU_DETECTION || !defined(ANCH_CPU_AES)
      /*! Internal state */
      uint8_t _state[4][4];
#endif

      /*! Expanded key */
      union ExpKey {
#ifdef ANCH_CPU_DETECTION
	uint32_t swKey[4 * (R + 1)];
	anch::crypto::AesniKey<R + 1> hwKey;
#elif ANCH_CPU_AES
	anch::crypto::AesniKey<R + 1> hwKey;
#else
	uint32_t swKey[4 * (R + 1)];
#endif
      };
      ExpKey _expKey;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref AES constructor
       *
       * \param key the encryption key
       */
      AES(const uint8_t key[4*K]);

      /*!
       * \ref AES copy constructor.\n
       * This constructor will copy the expanded key to avoid to compute it another time.
       *
       * \param other the AES to copy
       */
      AES(const AES& other);
      // Constructors -


      // Destructors +
      /*!
       * \ref AES destructor
       */
      virtual ~AES();
      // Destructors -


      // Methods +
    public:
      /*!
       * Cipher a block
       *
       * \param input the block to cipher
       * \param output the ciphered block
       */
      void cipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output);

      /*!
       * Decipher a block
       *
       * \param input the block to decipher
       * \param output the deciphered block
       */
      void decipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output);

    private:
#ifdef ANCH_CPU_AES
      /*!
       * Cipher block using AESNI instructions
       *
       * \param input the block to cipher
       * \param output the output result
       */
      void aesniCipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output);

      /*!
       * Decipher block using AESNI instructions
       *
       * \param input the block to decipher
       * \param output the output result
       */
      void aesniDecipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output);

      __m128i aes128assist(__m128i& temp1, __m128i& temp2);

      void aesni128ExpandKey(uint8_t key[4 * K]);

      void aes192assist(__m128i& temp1, __m128i& temp2, __m128i& temp3);

      void aesni192ExpandKey(uint8_t key[4 * K]);

      void aes256assist1(__m128i& temp1, __m128i& temp2);

      void aes256assist2(__m128i& temp1, __m128i& temp3);

      void aesni256ExpandKey(uint8_t key[4 * K]);

      void aesniExpandKey(uint8_t key[4 * K]);
#endif // ANCH_CPU_AES

#if defined ANCH_CPU_DETECTION || !defined(ANCH_CPU_AES)
      /*!
       * Cipher block using AnCH Framework implementation
       *
       * \param input the block to cipher
       * \param output the output result
       */
      void swCipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output);

      /*!
       * Decipher block using AnCH Framework implementation
       *
       * \param input the block to decipher
       * \param output the output result
       */
      void swDecipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output);

      /*!
       * Key expansion generic algorithm
       *
       * \param key the key to expand
       */
      void expandKey(const uint8_t key[4 * K]);

      /*!
       * Substitutes 32-bits word using substituion box defined in the
       * AES specification.
       *
       * \param word the word to treat
       *
       * \return the result of substitution
       */
      uint32_t subWord(const uint32_t& word);

      /*!
       * Rotate word to 8-bits to left
       *
       * \param word the word to rotate
       *
       * \return the rotated word
       */
      uint32_t rotateWord(const uint32_t& word);

      /*!
       * Substites internal state bytes using substituion box defined in the
       * AES specification.
       */
      void cipherSubBytes();

      /*!
       * Substites internal state bytes using substituion box defined in the
       * AES specification.
       */
      void decipherSubBytes();

      /*!
       * Shift internal state rows (cipher)
       */
      void cipherShiftRows();

      /*!
       * Shift internal state rows (decipher)
       */
      void decipherShiftRows();

      /*!
       * Mix internal by columns (cipher)
       */
      void cipherMixColumns();

      /*!
       * Mix internal by columns (decipher)
       */
      void decipherMixColumns();

      /*!
       * Add round key (cipher)
       *
       * \param round the current round
       */
      void addRoundKey(const uint32_t& round);
#endif // ANCH_CPU_DETECTION || !ANCH_CPU_AES
      // Methods -

    };

    // Constructors +
    template<std::size_t K, std::size_t R>
    AES<K,R>::AES(const uint8_t key[4*K]): _state(), _expKey() {
#ifdef ANCH_CPU_DETECTION
      if(anch::device::CPU::getInstance().isAES()) {
	uint8_t aesKey[4*K];
	std::memcpy(aesKey, key, 4*K);
	aesniExpandKey(aesKey);
      } else {
	expandKey(key);
      }
#elif ANCH_CPU_AES
      uint8_t aesKey[4*K];
      std::memcpy(aesKey, key, 4*K);
      aesniExpandKey(aesKey);
#else
      expandKey(key);
#endif
    }

    template<std::size_t K, std::size_t R>
    AES<K,R>::AES(const AES& other): _state(), _expKey() {
#ifdef ANCH_CPU_DETECTION
      if(anch::device::CPU::getInstance().isAES()) {
	std::memcpy(_expKey.hwKey.cipherKey, other._expKey.hwKey.cipherKey, (R + 1) * sizeof(__m128i));
	std::memcpy(_expKey.hwKey.decipherKey, other._expKey.hwKey.decipherKey, (R + 1) * sizeof(__m128i));
      } else {
	std::memcpy(_expKey.swKey, other._expKey.swKey, 4 * (R + 1) * sizeof(uint32_t));
      }
#elif ANCH_CPU_AES
      std::memcpy(_expKey.hwKey.cipherKey, other._expKey.hwKey.cipherKey, (R + 1) * sizeof(__m128i));
      std::memcpy(_expKey.hwKey.decipherKey, other._expKey.hwKey.decipherKey, (R + 1) * sizeof(__m128i));
#else
      std::memcpy(_expKey.swKey, other._expKey.swKey, 4 * (R + 1) * sizeof(uint32_t));
#endif
    }
    // Constructors -


    // Destructors +
    template<std::size_t K, std::size_t R>
    AES<K,R>::~AES() {
      // Nothing to do
    }
    // Destructors -


    // Methods +
    template<std::size_t K, std::size_t R>
    void
    AES<K,R>::cipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
#ifdef ANCH_CPU_DETECTION
      if(anch::device::CPU::getInstance().isAES()) {
	aesniCipher(input, output);
      } else {
	swCipher(input, output);
      }
#elif ANCH_CPU_AES
      aesniCipher(input, output);
#else
      swCipher(input, output);
#endif
    }

    template<std::size_t K, std::size_t R>
    void
    AES<K,R>::decipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
#ifdef ANCH_CPU_DETECTION
      if(anch::device::CPU::getInstance().isAES()) {
	aesniDecipher(input, output);
      } else {
	swDecipher(input, output);
      }
#elif ANCH_CPU_AES
      aesniDecipher(input, output);
#else
      swDecipher(input, output);
#endif
      }

#ifdef ANCH_CPU_AES
    template<std::size_t K, std::size_t R>
    void
    AES<K,R>::aesniCipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
      __m128i tmp;
      std::size_t j;
      ALIGN16 uint8_t in[16];
      std::memcpy(in, input.data(), 16);
      ALIGN16 __m128i out[2];
      for(std::size_t i = 0 ; i < 2 ; ++i) {
	tmp = _mm_loadu_si128(&(reinterpret_cast<__m128i*>(in)[i]));
	tmp = _mm_xor_si128(tmp, _expKey.hwKey.cipherKey[0]);
	for(j = 1 ; j < R ; ++j) {
	  tmp = _mm_aesenc_si128(tmp, _expKey.hwKey.cipherKey[j]);
	}
	tmp = _mm_aesenclast_si128(tmp, _expKey.hwKey.cipherKey[j]);
	_mm_storeu_si128(&(out[i]), tmp);
      }
      std::memcpy(output.data(), reinterpret_cast<uint8_t*>(out), 16);
    }

    template<std::size_t K, std::size_t R>
    void
    AES<K,R>::aesniDecipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
      __m128i tmp;
      std::size_t j;
      ALIGN16 uint8_t in[16];
      std::memcpy(in, input.data(), 16);
      ALIGN16 __m128i out[2];
      for(std::size_t i = 0 ; i < 2 ; ++i) {
	tmp = _mm_loadu_si128(&(reinterpret_cast<__m128i*>(in)[i]));
	tmp = _mm_xor_si128(tmp, _expKey.hwKey.decipherKey[0]);
	for(j = 1 ; j < R ; ++j) {
	  tmp = _mm_aesdec_si128(tmp, _expKey.hwKey.decipherKey[j]);
	}
	tmp = _mm_aesdeclast_si128(tmp, _expKey.hwKey.decipherKey[j]);
	_mm_storeu_si128(&(out[i]), tmp);
      }
      std::memcpy(output.data(), reinterpret_cast<uint8_t*>(out), 16);
    }

    template<std::size_t K, std::size_t R>
    inline __m128i
    AES<K,R>::aes128assist(__m128i& temp1, __m128i& temp2) {
      __m128i temp3;
      temp2 = _mm_shuffle_epi32(temp2 ,0xff);
      temp3 = _mm_slli_si128(temp1, 0x4);
      temp1 = _mm_xor_si128(temp1, temp3);
      temp3 = _mm_slli_si128(temp3, 0x4);
      temp1 = _mm_xor_si128(temp1, temp3);
      temp3 = _mm_slli_si128(temp3, 0x4);
      temp1 = _mm_xor_si128(temp1, temp3);
      temp1 = _mm_xor_si128(temp1, temp2);
      return temp1;
    }

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::aesni128ExpandKey([[maybe_unused]] uint8_t key[4 * K]) {
      if constexpr (K == 4) {
	  __m128i temp1, temp2;
	  temp1 = _mm_loadu_si128(reinterpret_cast<__m128i*>(key));
	  _expKey.hwKey.cipherKey[0] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x1);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[1] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x2);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[2] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x4);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[3] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x8);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[4] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x10);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[5] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x20);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[6] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x40);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[7] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x80);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[8] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x1b);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[9] = temp1;
	  temp2 = _mm_aeskeygenassist_si128(temp1, 0x36);
	  temp1 = aes128assist(temp1, temp2);
	  _expKey.hwKey.cipherKey[10] = temp1;
	}
    }

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::aes192assist(__m128i& temp1, __m128i& temp2, __m128i& temp3) {
      __m128i temp4;
      temp2 = _mm_shuffle_epi32(temp2, 0x55);
      temp4 = _mm_slli_si128(temp1, 0x4);
      temp1 = _mm_xor_si128(temp1, temp4);
      temp4 = _mm_slli_si128(temp4, 0x4);
      temp1 = _mm_xor_si128(temp1, temp4);
      temp4 = _mm_slli_si128(temp4, 0x4);
      temp1 = _mm_xor_si128(temp1, temp4);
      temp1 = _mm_xor_si128(temp1, temp2);
      temp2 = _mm_shuffle_epi32(temp1, 0xff);
      temp4 = _mm_slli_si128(temp3, 0x4);
      temp3 = _mm_xor_si128(temp3, temp4);
      temp3 = _mm_xor_si128(temp3, temp2);
    }

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::aesni192ExpandKey([[maybe_unused]] uint8_t key[4 * K]) {
      if constexpr (K == 6) {
	  __m128i temp1, temp2, temp3;
	  temp1 = _mm_loadu_si128(reinterpret_cast<__m128i*>(key));
	  temp3 = _mm_loadu_si128(reinterpret_cast<__m128i*>((key + 16)));
	  _expKey.hwKey.cipherKey[0] = temp1;
	  _expKey.hwKey.cipherKey[1] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x1);
	  aes192assist(temp1, temp2, temp3);
	  _expKey.hwKey.cipherKey[1] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(_expKey.hwKey.cipherKey[1]),
										reinterpret_cast<__m128d>(temp1), 0));
	  _expKey.hwKey.cipherKey[2] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp1),
										reinterpret_cast<__m128d>(temp3), 1));
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x2);
	  aes192assist(temp1, temp2, temp3);
	  _expKey.hwKey.cipherKey[3] = temp1;
	  _expKey.hwKey.cipherKey[4] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x4);
	  aes192assist(temp1, temp2, temp3);
	  _expKey.hwKey.cipherKey[4] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(_expKey.hwKey.cipherKey[4]),
										reinterpret_cast<__m128d>(temp1), 0));
	  _expKey.hwKey.cipherKey[5] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp1),
										reinterpret_cast<__m128d>(temp3), 1));
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x8);
	  aes192assist(temp1, temp2, temp3);
	  _expKey.hwKey.cipherKey[6] = temp1;
	  _expKey.hwKey.cipherKey[7] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x10);
	  aes192assist(temp1, temp2, temp3);
	  _expKey.hwKey.cipherKey[7] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(_expKey.hwKey.cipherKey[7]),
										reinterpret_cast<__m128d>(temp1), 0));
	  _expKey.hwKey.cipherKey[8] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp1),
										reinterpret_cast<__m128d>(temp3), 1));
	  temp2 = _mm_aeskeygenassist_si128 (temp3, 0x20);
	  aes192assist(temp1, temp2, temp3);
	  _expKey.hwKey.cipherKey[9] = temp1;
	  _expKey.hwKey.cipherKey[10] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x40);
	  aes192assist(temp1, temp2, temp3);
	  _expKey.hwKey.cipherKey[10] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(_expKey.hwKey.cipherKey[10]),
										 reinterpret_cast<__m128d>(temp1), 0));
	  _expKey.hwKey.cipherKey[11] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp1),
										 reinterpret_cast<__m128d>(temp3), 1));
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x80);
	  aes192assist(temp1, temp2, temp3);
	  _expKey.hwKey.cipherKey[12] = temp1;
	}
    }

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::aes256assist1(__m128i& temp1, __m128i& temp2) {
      __m128i temp4;
      temp2 = _mm_shuffle_epi32(temp2, 0xff);
      temp4 = _mm_slli_si128(temp1, 0x4);
      temp1 = _mm_xor_si128(temp1, temp4);
      temp4 = _mm_slli_si128(temp4, 0x4);
      temp1 = _mm_xor_si128(temp1, temp4);
      temp4 = _mm_slli_si128(temp4, 0x4);
      temp1 = _mm_xor_si128(temp1, temp4);
      temp1 = _mm_xor_si128(temp1, temp2);
    }

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::aes256assist2(__m128i& temp1, __m128i& temp3) {
      __m128i temp2,temp4;
      temp4 = _mm_aeskeygenassist_si128(temp1, 0x0);
      temp2 = _mm_shuffle_epi32(temp4, 0xaa);
      temp4 = _mm_slli_si128(temp3, 0x4);
      temp3 = _mm_xor_si128(temp3, temp4);
      temp4 = _mm_slli_si128(temp4, 0x4);
      temp3 = _mm_xor_si128(temp3, temp4);
      temp4 = _mm_slli_si128(temp4, 0x4);
      temp3 = _mm_xor_si128(temp3, temp4);
      temp3 = _mm_xor_si128(temp3, temp2);
    }

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::aesni256ExpandKey([[maybe_unused]] uint8_t key[4 * K]) {
      if constexpr (K == 8) {
	  __m128i temp1, temp2, temp3;
	  temp1 = _mm_loadu_si128(reinterpret_cast<__m128i*>(key));
	  temp3 = _mm_loadu_si128(reinterpret_cast<__m128i*>(key + 16));
	  _expKey.hwKey.cipherKey[0] = temp1;
	  _expKey.hwKey.cipherKey[1] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x01);
	  aes256assist1(temp1, temp2);
	  _expKey.hwKey.cipherKey[2] = temp1;
	  aes256assist2(temp1, temp3);
	  _expKey.hwKey.cipherKey[3] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x02);
	  aes256assist1(temp1, temp2);
	  _expKey.hwKey.cipherKey[4] = temp1;
	  aes256assist2(temp1, temp3);
	  _expKey.hwKey.cipherKey[5] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x04);
	  aes256assist1(temp1, temp2);
	  _expKey.hwKey.cipherKey[6] = temp1;
	  aes256assist2(temp1, temp3);
	  _expKey.hwKey.cipherKey[7] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x08);
	  aes256assist1(temp1, temp2);
	  _expKey.hwKey.cipherKey[8] = temp1;
	  aes256assist2(temp1, temp3);
	  _expKey.hwKey.cipherKey[9] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x10);
	  aes256assist1(temp1, temp2);
	  _expKey.hwKey.cipherKey[10] = temp1;
	  aes256assist2(temp1, temp3);
	  _expKey.hwKey.cipherKey[11] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x20);
	  aes256assist1(temp1, temp2);
	  _expKey.hwKey.cipherKey[12] = temp1;
	  aes256assist2(temp1, temp3);
	  _expKey.hwKey.cipherKey[13] = temp3;
	  temp2 = _mm_aeskeygenassist_si128(temp3, 0x40);
	  aes256assist1(temp1, temp2);
	  _expKey.hwKey.cipherKey[14] = temp1;
	}
    }

    template<std::size_t K, std::size_t R>
    void
    AES<K,R>::aesniExpandKey(uint8_t key[4 * K]) {
      // Compute cipher key +
      aesni128ExpandKey(key);
      aesni192ExpandKey(key);
      aesni256ExpandKey(key);
      // Compute cipher key -

      // Compute decipher key +
      _expKey.hwKey.decipherKey[R] = _expKey.hwKey.cipherKey[0];
      _expKey.hwKey.decipherKey[R - 1] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[1]);
      _expKey.hwKey.decipherKey[R - 2] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[2]);
      _expKey.hwKey.decipherKey[R - 3] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[3]);
      _expKey.hwKey.decipherKey[R - 4] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[4]);
      _expKey.hwKey.decipherKey[R - 5] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[5]);
      _expKey.hwKey.decipherKey[R - 6] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[6]);
      _expKey.hwKey.decipherKey[R - 7] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[7]);
      _expKey.hwKey.decipherKey[R - 8] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[8]);
      _expKey.hwKey.decipherKey[R - 9] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[9]);
      if constexpr (R > 10) {
	  _expKey.hwKey.decipherKey[R - 10] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[10]);
	  _expKey.hwKey.decipherKey[R - 11] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[11]);
	}
      if constexpr (R > 12) {
	  _expKey.hwKey.decipherKey[R - 12] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[12]);
	  _expKey.hwKey.decipherKey[R - 13] = _mm_aesimc_si128(_expKey.hwKey.cipherKey[13]);
	}
      _expKey.hwKey.decipherKey[0] = _expKey.hwKey.cipherKey[R];
      // Compute decipher key -
    }
#endif // ANCH_CPU_AES

#if defined ANCH_CPU_DETECTION || !defined(ANCH_CPU_AES)
    template<std::size_t K, std::size_t R>
    void
    AES<K,R>::swCipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
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

    template<std::size_t K, std::size_t R>
    void
    AES<K,R>::swDecipher(const std::array<uint8_t,16>& input, std::array<uint8_t,16>& output) {
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

    template<std::size_t K, std::size_t R>
    void
    AES<K,R>::expandKey(const uint8_t key[4 * K]) {
      std::memcpy(_expKey.swKey, key, 4 * K);
      for(std::size_t i = K ; i < 4 * (R + 1) ; ++i) {
	uint32_t mod = static_cast<uint32_t>(i % K);
	if(mod == 0) {
	  _expKey.swKey[i] = _expKey.swKey[i - K] ^ (subWord(rotateWord(_expKey.swKey[i - 1])) ^ ANCH_AES_RCON[i / K]);
	} else if(K > 6 && mod == 4) {
	  _expKey.swKey[i] = _expKey.swKey[i - K] ^ subWord(_expKey.swKey[i - 1]);
	} else {
	  _expKey.swKey[i] = _expKey.swKey[i - K] ^ _expKey.swKey[i - 1];
	}
      }
    }

    template<std::size_t K, std::size_t R>
    inline uint32_t
    AES<K,R>::subWord(const uint32_t& word) {
      uint32_t res;
      uint8_t* resBytes = reinterpret_cast<uint8_t*>(&res);
      const uint8_t* const bytes = reinterpret_cast<const uint8_t* const>(&word);
      resBytes[0] = ANCH_AES_CIPHER_SBOX[bytes[0]];
      resBytes[1] = ANCH_AES_CIPHER_SBOX[bytes[1]];
      resBytes[2] = ANCH_AES_CIPHER_SBOX[bytes[2]];
      resBytes[3] = ANCH_AES_CIPHER_SBOX[bytes[3]];
      return res;
    }

    template<std::size_t K, std::size_t R>
    inline uint32_t
    AES<K,R>::rotateWord(const uint32_t& word) {
      return ((word << 24) | (word >> 8));
    }

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::cipherSubBytes() {
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

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::decipherSubBytes() {
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

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::cipherShiftRows() {
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

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::decipherShiftRows() {
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

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::cipherMixColumns() {
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

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::decipherMixColumns() {
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

    template<std::size_t K, std::size_t R>
    inline void
    AES<K,R>::addRoundKey(const uint32_t& round) {
      uint32_t* key = _expKey.swKey + 4 * round;
      uint32_t* state = reinterpret_cast<uint32_t*>(_state);
      state[0] ^= key[0];
      state[1] ^= key[1];
      state[2] ^= key[2];
      state[3] ^= key[3];
    }
#endif // ANCH_CPU_DETECTION || !ANCH_CPU_AES
      // Methods -

  }
}
