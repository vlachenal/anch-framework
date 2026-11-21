/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "anch-crypto-export.hpp"

#include "crypto/cipher/bcModOp.hpp"

namespace anch::crypto {

  /*!
   * \brief Counter implementation.
   *
   * CTR mode is also known as ICM (integer counter mode) and SIC (segmented integer counter).\n
   * The nonce is the initial counter block, incremented as a big endian integer for each block (NIST SP 800-38A,
   * compatible with OpenSSL CTR mode). A nonce must never be used twice with the same key.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename Cipher>
  class ANCH_CRYPTO_EXPORT CTR: public BlockCipherModeOfOperation<CTR<Cipher>,Cipher> {

    // Attributes +
  private:
    /*! Initialization vector */
    std::array<uint8_t,Cipher::getBlockSize()> _nonce;
    // Attributes -


    // Constructors +
  public:
    /*!
     * \ref CTR constructor
     *
     * \param nonce the nonce
     * \param nbThread the maximum number of thread to run in parallel (default to 1).
     *                 If is set to 0, it will be set to the number of CPU if found (1 otherwise).
     */
    CTR(const std::array<uint8_t,Cipher::getBlockSize()>& nonce, unsigned int nbThread = 1);
    // Constructors -


    // Destructor +
    /*!
     * \ref CTR destructor
     */
    virtual ~CTR();
    // Destructor -


    // Methods +
  protected:
    /*!
     * Cipher a block.\n
     * This method will handle initialization vector management.
     *
     * \param input the input block to cipher
     * \param nbRead the size of block which has been read
     * \param output the output block
     * \param index the block index
     * \param cipher the cipher instance
     *
     * \return the number of bytes to write
     */
    virtual std::size_t cipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
				    std::streamsize nbRead,
				    std::array<uint8_t,Cipher::getBlockSize()>& output,
				    uint64_t index,
				    Cipher& cipher) override;

    /*!
     * Decipher a block.\n
     * This method will handle initialization vector management.
     *
     * \param input the input block to decipher
     * \param nbRead the size of block which has been read
     * \param output the output block
     * \param index the block index
     * \param cipher the cipher instance
     *
     * \return the number of bytes to write
     */
    virtual std::size_t decipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
				      std::array<uint8_t,Cipher::getBlockSize()>&,
				      std::streamsize nbRead,
				      bool,
				      std::array<uint8_t,Cipher::getBlockSize()>& output,
				      uint64_t index,
				      Cipher& cipher) override;

    /*!
     * Reset block cipher mode of operation context
     *
     * \return the initial context
     */
    virtual const std::array<uint8_t,Cipher::getBlockSize()>& reset() override;

  private:
    /*!
     * Compute the counter block of a block: the nonce is the initial counter block, it is incremented by the block
     * index as a big endian integer of the block size, modulo 2^(8 * block size) (standard incrementing function of
     * NIST SP 800-38A, as OpenSSL)
     *
     * \param ctxtVect the counter block initialized with the nonce
     * \param index the block index
     */
    static void setCounter(std::array<uint8_t,Cipher::getBlockSize()>& ctxtVect, uint64_t index);
    // Methods -

  };

  // Constructors +
  template<typename Cipher>
  CTR<Cipher>::CTR(const std::array<uint8_t,Cipher::getBlockSize()>& nonce, unsigned int nbThread):
    BlockCipherModeOfOperation<CTR<Cipher>,Cipher>(true, true, nbThread),
    _nonce(nonce) {
    // Nothing to do
  }
  // Constructors -


  // Destructor +
  template<typename Cipher>
  CTR<Cipher>::~CTR() {
    // Nothing to do
  }
  // Destructor -


  // Methods +
  template<typename Cipher>
  std::size_t
  CTR<Cipher>::cipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
			   std::streamsize nbRead,
			   std::array<uint8_t,Cipher::getBlockSize()>& output,
			   uint64_t index,
			   Cipher& cipher) {
    std::array<uint8_t,Cipher::getBlockSize()> ctxtVect(_nonce);
    setCounter(ctxtVect, index);
    std::array<uint8_t,Cipher::getBlockSize()> data;
    cipher.cipher(ctxtVect, data);
    for(std::size_t i = 0 ; i < static_cast<std::size_t>(nbRead) ; ++i) {
      output[i] = input[i] ^ data[i];
    }
    return static_cast<std::size_t>(nbRead);
  }

  template<typename Cipher>
  std::size_t
  CTR<Cipher>::decipherBlock(std::array<uint8_t,Cipher::getBlockSize()>& input,
			     std::array<uint8_t,Cipher::getBlockSize()>&,
			     std::streamsize nbRead,
			     bool,
			     std::array<uint8_t,Cipher::getBlockSize()>& output,
			     uint64_t index,
			     Cipher& cipher) {
    std::array<uint8_t,Cipher::getBlockSize()> ctxtVect(_nonce);
    setCounter(ctxtVect, index);
    std::array<uint8_t,Cipher::getBlockSize()> data;
    cipher.cipher(ctxtVect, data);
    for(std::size_t i = 0 ; i < static_cast<std::size_t>(nbRead) ; ++i) {
      output[i] = input[i] ^ data[i];
    }
    return static_cast<std::size_t>(nbRead);
  }

  template<typename Cipher>
  const std::array<uint8_t,Cipher::getBlockSize()>&
  CTR<Cipher>::reset() {
    return _nonce;
  }

  template<typename Cipher>
  inline void
  CTR<Cipher>::setCounter(std::array<uint8_t,Cipher::getBlockSize()>& ctxtVect, uint64_t index) {
    // Add index to the big endian counter, from the last byte, with carry
    uint64_t carry = index;
    for(std::size_t i = Cipher::getBlockSize() ; i > 0 && carry != 0 ; --i) {
      uint64_t sum = ctxtVect[i - 1] + (carry & 0xff);
      ctxtVect[i - 1] = static_cast<uint8_t>(sum & 0xff);
      carry = (carry >> 8) + (sum >> 8);
    }
  }
  // Methods -

}  // anch::crypto
