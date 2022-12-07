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

#include <string.h>

namespace anch {
  namespace crypto {

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
    private:
      /*! \ref Base64 alphabet */
      const static char _alphabet[];

      /*! \ref Base64 padding character */
      const static char _padding;
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
       */
      static void encode(std::istream& data, std::ostream& output) noexcept;

      /*!
       * Encode data in base64.
       *
       * \param data the data stream to encode
       *
       * \return the base64 encoded data
       */
      static std::string encode(std::istream& data) noexcept;

      /*!
       * Encode data in base64.
       *
       * \param data the data string to encode
       * \param output the stream to write in
       */
      static void encode(const std::string& data, std::ostream& output) noexcept;

      /*!
       * Encode data in base64.
       *
       * \param data the data string to encode
       *
       * \return the base64 encoded data
       */
      static std::string encode(const std::string& data) noexcept;

      /*!
       * Encode data in base64.
       *
       * \param data the data bytes to encode
       * \param length the data length
       * \param output the stream to write in
       */
      static void encode(const uint8_t* data, uint64_t length, std::ostream& output) noexcept;

      /*!
       * Encode data in base64.
       *
       * \param data the data bytes to encode
       * \param length the data length
       *
       * \return the base64 encoded data
       */
      static std::string encode(const uint8_t* data, uint64_t length) noexcept;

    private:
      /*!
       * Encode block in base64.
       *
       * \param data the block address to encode
       * \param length the data length
       * \param buffer the buffer to write in
       *
       * \return the number of read bytes
       */
      static uint8_t encode(const uint8_t* data, uint64_t length, char* buffer) noexcept;
      // Encoding methods -


      // Decoding methods +
    public:
      /*!
       * Decode data in base64.
       *
       * \param data the data stream to decode
       * \param output the stream to write in
       */
      static void decode(std::istream& data, std::ostream& output) noexcept;

      /*!
       * Decode data in base64.
       *
       * \param data the data stream to decode
       *
       * \return the base64 decoded data
       */
      static std::string decode(std::istream& data) noexcept;

      /*!
       * Decode data in base64.
       *
       * \param data the data string to decode
       * \param output the stream to write in
       */
      static void decode(const std::string& data, std::ostream& output) noexcept;

      /*!
       * Decode data in base64.
       *
       * \param data the data string to decode
       *
       * \return the base64 decoded data
       */
      static std::string decode(const std::string& data) noexcept;

      /*!
       * Decode data in base64.
       *
       * \param data the data bytes to decode
       * \param length the data length
       * \param output the stream to write in
       */
      static void decode(const char* data, uint64_t length, std::ostream& output) noexcept;

      /*!
       * Decode data in base64.
       *
       * \param data the data bytes to decode
       * \param length the data length
       *
       * \return the base64 decoded data
       */
      static std::string decode(const char* data, uint64_t length) noexcept;

    private:
      /*!
       * Encode block in base64.
       *
       * \param data the block address to encode
       * \param length the data length
       * \param buffer the buffer to write in
       *
       * \return the number of read bytes
       */
      static void decode(const char* data, uint64_t length, uint8_t buffer[4]) noexcept;
      // Decoding methods -

    };

  }
}
