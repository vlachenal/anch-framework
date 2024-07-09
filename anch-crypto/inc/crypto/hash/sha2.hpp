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

#include "crypto/hash/hash.hpp"

namespace anch::crypto {

  /*!
   * \brief SHA2 abstract class.
   *
   * \ref SHA2 contains algorithms for SHA224/256/384/512.\n
   * The children have to define the translation array and the sigma
   * transformation functions.\n
   * The template parameters are:
   * - O: the output size in byte
   * - B: the treatment block size in byte
   * - W: the word type (32 bits or 64 bits)
   * - R: the number of round for each chunk
   * - I: the initial state
   *
   * SHA224/256/384/512 are implemented in this library.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<std::size_t O, std::size_t B, typename W, uint32_t R, const std::array<W,8>& I>
  class SHA2: public Hash<O,B> {

  protected:
    /*!
     * \ref SHA2 chunk
     *
     * \author Vincent Lachenal
     */
    typedef union {
      /*! Bytes */
      uint8_t bytes[B * sizeof(W)];

      /*! Words */
      W words[B];
    } Chunk;

    /*!
     * \ref SHA2 context
     *
     * \author Vincent Lachenal
     */
    template<const std::array<W,8>& Init>
    struct Context {
      /*! State */
      std::array<W,8> state;

      /*! Message size */
      uint64_t size;

      /*! Buffer */
      uint8_t buffer[B];

      /*! Digest */
      std::array<uint8_t,O> digest;

      /*!
       * \ref SHA2 \ref Context constructor
       */
      Context();

      /*!
       * Initialize \ref SHA2 context
       */
      void reset();

    };

    // Attributes +
  protected:
    /*! \ref SHA2 context */
    Context<I> _context;
    // Attributes -


    // Constructors +
  public:
    /*!
     * \ref SHA2 default constructor
     */
    SHA2();
    // Constructors -

  public:
    // Destructor +
    /*!
     * \ref SHA2 destructor
     */
    virtual ~SHA2();
    // Destructor -


    // Methods +
  public:
    /*!
     * Get the SHA2 hash result
     *
     * \return the SHA2 hash result
     */
    virtual const std::array<uint8_t,O>& digest() const override;

  protected:
    /*!
     * Shift n bits word from n bits to right
     *
     * \param bits the number of bits to shift
     * \param word the n bits word to shift
     */
    static constexpr inline W shiftRight(uint8_t bits, W word) {
      return (word >> bits);
    }

    /*!
     * Rotate n bits word from n bits to left
     *
     * \param bits the number of bits to rotate
     * \param word the n bits word to rotate
     */
    static constexpr inline W rotateLeft(uint8_t bits, W word) {
      return ((word << bits) | (word >> (sizeof(W) * 8 - bits)));
    }

    /*!
     * Rotate n bits word from n bits to right
     *
     * \param bits the number of bits to rotate
     * \param word the n bits word to rotate
     */
    static constexpr inline W rotateRight(uint8_t bits, W word) {
      return ((word >> bits) | (word << (sizeof(W) * 8 - bits)));
    }

  private:
    /*!
     * Reset hash context
     */
    virtual void reset() override;

    /*!
     * Compute hash for data with the current hash
     *
     * \param data The data to add
     * \param len The data length
     */
    virtual void addData(const uint8_t* data, std::size_t len) override;

    /*!
     * Finalize hash
     */
    virtual void finalize() override;

    /*!
     * \ref SHA2 translation array getter.
     *
     * \return the \ref SHA2 translation array
     */
    virtual const std::array<W,R>& getTranslationArray() const = 0;

    /*!
     * Transform SHA2 with the current chunk
     *
     * \param buffer The data to process
     */
    void transform(const uint8_t* buffer);

    /*!
     * Swap byte for endianness conversion
     *
     * \param buf The 4-bytes words to process
     * \param count The number of operation to do
     */
    static void bytesSwap(W* buf, uint8_t count);

    /*!
     * Some SHA2 transformation function
     *
     * \param x first value
     * \param y second value
     * \param z third value
     */
    static W ch(W x, W y, W z);

    /*!
     * Some SHA2 transformation function
     *
     * \param x first value
     * \param y second value
     * \param z third value
     */
    static W maj(W x, W y, W z);

    /*!
     * \ref SHA2 first sigma function
     *
     * \param word the word to transform
     */
    virtual W SIGMA0(W word) const = 0;

    /*!
     * \ref SHA2 second sigma function
     *
     * \param word the word to transform
     */
    virtual W SIGMA1(W word) const = 0;

    /*!
     * \ref SHA2 first sigma function
     *
     * \param word the word to transform
     */
    virtual W sigma0(W word) const = 0;

    /*!
     * \ref SHA2 second sigma function
     *
     * \param word the word to transform
     */
    virtual W sigma1(W word) const = 0;
    // Methods -

  };

}

#include "crypto/hash/impl/sha2.hpp"
