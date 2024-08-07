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

  template<typename H> H HMAC(const std::string&, const std::string&);

  /*!
   * \brief SHA1 hash algorithm implementation
   *
   * This algorithm is based on Qt framework algorithm.
   *
   * \author Vincent Lachenal
   */
  class SHA1: public Hash<20,64> {

    friend SHA1 anch::crypto::HMAC<SHA1>(const std::string&, const std::string&);

  private:
    /*!
     * SHA1 context
     *
     * \author Vincent Lachenal
     */
    struct Context {
      /*! State */
      std::array<uint32_t,5> state;

      /*! Message size */
      uint64_t size;

      /*! Buffer */
      std::array<uint8_t,64> buffer;

      /*! Digest */
      std::array<uint8_t,20> digest;

      /*!
       * \ref SHA1 \ref Context constructor
       */
      Context();

      /*!
       * Initialize SHA1 context
       */
      void reset();

    };

    // Attributes +
  private:
    /*! SHA1 context */
    Context _context;
    // Attributes -


    // Constructors +
  public:
    /*!
     * \ref SHA1 default constructor
     */
    SHA1();

    /*!
     * \ref SHA1 constructor with string
     *
     * \param data The string data to process
     */
    SHA1(const std::string& data);

    /*!
     * \ref SHA1 constructor with input stream.
     *
     * \param stream The input stream to process
     */
    SHA1(std::istream& stream);

  private:
    /*!
     * \ref SHA1 constructor with data bytes.
     *
     * \param data the data bytes
     * \param len the data length
     */
    SHA1(const uint8_t* data, std::size_t len);
    // Constructors -


    // Destructor +
  public:
    /*!
     * \ref SHA1 destructor
     */
    virtual ~SHA1();
    // Destructor -


    // Methods +
  public:
    /*!
     * Get the SHA1 hash result
     *
     * \return the SHA1 hash result
     */
    virtual const std::array<uint8_t,20>& digest() const override;

  protected:
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

  private:
    /*!
     * Transform SHA1 with the current chunk
     *
     * \param buffer The data to process
     */
    void transform(const uint8_t* buffer);
    // Methods -

  };

  extern template class Hash<20,64>;
  /*! HMAC SHA1 definition */
  extern template SHA1 HMAC<SHA1>(const std::string&, const std::string&);

  /*!
   * Register SHA1 UUID provider
   */
  void registerSHA1UUIDProvider();

}
