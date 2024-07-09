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

#include <ostream>
#include <array>
#include <cstdint>


namespace anch::crypto {

  /*!
   * \brief Hash algorithm abstract class.
   *
   * N is the hash output size in bytes.\n
   * B is the hash block size in bytes (used in HMAC algorithm).
   *
   * \author Vincent Lachenal
   */
  template<std::size_t O, std::size_t B>
  class Hash {

  protected:
    /*! Digest */
    std::array<uint8_t,O>* _digest;

    // Methods +
  public:
    /*!
     * Get the hash result
     *
     * \return the hash result
     */
    virtual const std::array<uint8_t,O>& digest() const = 0;

    /*!
     * Reset current hash context and compute hash for string
     *
     * \param data the string to hash
     *
     * \return the hash result
     */
    const std::array<uint8_t,O>& digest(const std::string& data);

    /*!
     * Reset current hash context and compute hash for string
     *
     * \param data the string to hash
     *
     * \return the hash result
     */
    template<std::size_t N>
    const std::array<uint8_t,O>& digest(const std::array<uint8_t,N>& data);

    /*!
     * Reset current hash context and compute hash for string
     *
     * \param data the string to hash
     * \param len the data length
     *
     * \return the hash result
     */
    const std::array<uint8_t,O>& digest(const uint8_t* data, std::size_t len);

    /*!
     * Reset current hash context and compute hash for stream
     *
     * \param stream the stream to hash
     *
     * \return the hash result
     */
    const std::array<uint8_t,O>& digest(std::istream& stream);

  protected:
    /*!
     * Reset hash context
     */
    virtual void reset() = 0;

    /*!
     * Compute hash for data with the current hash
     *
     * \param data The data to add
     * \param len The data length
     */
    virtual void addData(const uint8_t* data, std::size_t len) = 0;

    /*!
     * Reset hash context
     */
    virtual void finalize() = 0;
    // Methods -


    // Accessors +
  public:
    /*!
     * Hash output size getter
     *
     * \return the output size
     */
    inline static constexpr std::size_t getOutputSize() {
      return O;
    }

    /*!
     * Hash block size getter
     *
     * \return the block size
     */
    inline static constexpr std::size_t getBlockSize() {
      return B;
    }
    // Accessors -

  };

}

/*!
 * Ouput stream operator definition for every hash algorithm.\n
 * This function preserves the formatting flags.
 *
 * \param out The output stream
 * \param hash The hash
 *
 * \return The output stream
 */
template<std::size_t O, std::size_t B>
std::ostream&
operator << (std::ostream& out, const anch::crypto::Hash<O,B>& hash);

#include "crypto/hash/impl/hash.hpp"
