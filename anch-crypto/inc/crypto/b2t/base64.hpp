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

#include <iostream>
#include <sstream>
#include <cstdint>

#include <string.h>

namespace anch::crypto {

  /*!
   * \brief Base64 algorithm implementation.
   *
   * \ref Base64 is a common algorithm to encode binary data into text.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class Base64 {
    // Attributes +
  public:
    /*! Base64 classic aplhabet flag */
    static const uint8_t STD;

    /*! Base64 URL aplhabet flag */
    static const uint8_t URL;

    /*! Base64 padding with '=' flag */
    static const uint8_t PADDING;

    /*! Base64 no padding flag */
    static const uint8_t NOPADDING;
    // Attributes -

    // Constructors +
  private:
    /*!
     * \ref Base64 private constructor
     */
    Base64();
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Base64 destructor
     */
    virtual ~Base64();
    // Destructor -


    // Encoding methods +
  public:
    /*!
     * Encode data in base64.
     *
     * \param data the data stream to encode
     * \param output the stream to write in
     * \param options the encoding options
     */
    static void encode(std::istream& data, std::ostream& output, uint8_t options = STD | PADDING) noexcept;

    /*!
     * Encode data in base64.
     *
     * \param data the data stream to encode
     * \param options the encoding options
     *
     * \return the base64 encoded data
     */
    static std::string encode(std::istream& data, uint8_t options = STD | PADDING) noexcept;

    /*!
     * Encode data in base64.
     *
     * \param data the data string to encode
     * \param output the stream to write in
     * \param options the encoding options
     */
    static void encode(const std::string& data, std::ostream& output, uint8_t options = STD | PADDING) noexcept;

    /*!
     * Encode data in base64.
     *
     * \param data the data string to encode
     * \param options the encoding options
     *
     * \return the base64 encoded data
     */
    static std::string encode(const std::string& data, uint8_t options = STD | PADDING) noexcept;

    /*!
     * Encode data in base64.
     *
     * \param data the data bytes to encode
     * \param length the data length
     * \param output the stream to write in
     * \param options the encoding options
     */
    static void encode(const uint8_t* data, uint64_t length, std::ostream& output, uint8_t options = STD | PADDING) noexcept;

    /*!
     * Encode data in base64.
     *
     * \param data the data bytes to encode
     * \param length the data length
     * \param options the encoding options
     *
     * \return the base64 encoded data
     */
    static std::string encode(const uint8_t* data, uint64_t length, uint8_t options = STD | PADDING) noexcept;

    /*!
     * Encode data in base64 URI.
     *
     * \param data the data stream to encode
     * \param output the stream to write in
     */
    static void encodeURI(std::istream& data, std::ostream& output) noexcept;

    /*!
     * Encode data in base64 URI.
     *
     * \param data the data stream to encode
     *
     * \return the base64 encoded data
     */
    static std::string encodeURI(std::istream& data) noexcept;

    /*!
     * Encode data in base64 URI.
     *
     * \param data the data string to encode
     * \param output the stream to write in
     */
    static void encodeURI(const std::string& data, std::ostream& output) noexcept;

    /*!
     * Encode data in base64 URI.
     *
     * \param data the data string to encode
     *
     * \return the base64 encoded data
     */
    static std::string encodeURI(const std::string& data) noexcept;

    /*!
     * Encode data in base64 URI.
     *
     * \param data the data bytes to encode
     * \param length the data length
     * \param output the stream to write in
     */
    static void encodeURI(const uint8_t* data, uint64_t length, std::ostream& output) noexcept;

    /*!
     * Encode data in base64 URI.
     *
     * \param data the data bytes to encode
     * \param length the data length
     *
     * \return the base64 encoded data
     */
    static std::string encodeURI(const uint8_t* data, uint64_t length) noexcept;
    // Encoding methods -


    // Decoding methods +
  public:
    /*!
     * Decode data in base64.
     *
     * \param data the data stream to decode
     * \param output the stream to write in
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static void decode(std::istream& data, std::ostream& output, uint8_t options = STD | PADDING);

    /*!
     * Decode data in base64.
     *
     * \param data the data stream to decode
     *
     * \return the base64 decoded data
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static std::string decode(std::istream& data, uint8_t options = STD | PADDING);

    /*!
     * Decode data in base64.
     *
     * \param data the data string to decode
     * \param output the stream to write in
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static void decode(const std::string& data, std::ostream& output, uint8_t options = STD | PADDING);

    /*!
     * Decode data in base64.
     *
     * \param data the data string to decode
     *
     * \return the base64 decoded data
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static std::string decode(const std::string& data, uint8_t options = STD | PADDING);

    /*!
     * Decode data in base64.
     *
     * \param data the data bytes to decode
     * \param length the data length
     * \param output the stream to write in
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static void decode(const char* data, uint64_t length, std::ostream& output, uint8_t options = STD | PADDING);

    /*!
     * Decode data in base64.
     *
     * \param data the data bytes to decode
     * \param length the data length
     *
     * \return the base64 decoded data
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static std::string decode(const char* data, uint64_t length, uint8_t options = STD | PADDING);

    /*!
     * Decode data in base64.
     *
     * \param data the data stream to decode
     * \param output the stream to write in
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static void decodeURI(std::istream& data, std::ostream& output);

    /*!
     * Decode data in base64.
     *
     * \param data the data stream to decode
     *
     * \return the base64 decoded data
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static std::string decodeURI(std::istream& data);

    /*!
     * Decode data in base64.
     *
     * \param data the data string to decode
     * \param output the stream to write in
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static void decodeURI(const std::string& data, std::ostream& output);

    /*!
     * Decode data in base64.
     *
     * \param data the data string to decode
     *
     * \return the base64 decoded data
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static std::string decodeURI(const std::string& data);

    /*!
     * Decode data in base64.
     *
     * \param data the data bytes to decode
     * \param length the data length
     * \param output the stream to write in
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static void decodeURI(const char* data, uint64_t length, std::ostream& output);

    /*!
     * Decode data in base64.
     *
     * \param data the data bytes to decode
     * \param length the data length
     *
     * \return the base64 decoded data
     *
     * \throw std::bad_cast invalid base 64 stream
     */
    static std::string decodeURI(const char* data, uint64_t length);
    // Decoding methods -

  };

}

#include "crypto/b2t/impl/base64.hpp"
