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
#ifndef _ANCH_CRYPTO_B2T_BASE64_H_
#define _ANCH_CRYPTO_B2T_BASE64_H_

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
      template<class CharT, class Traits>
      static void encode(std::basic_istream<CharT,Traits>& data,
			 std::basic_ostream<CharT,Traits>& output)
	noexcept {
	if(data) {
	  char buffer[1024];
	  while(!data.eof()) {
	    data.read(buffer, 1024);
	    encode(reinterpret_cast<uint8_t*>(buffer), static_cast<uint64_t>(data.gcount()), output);
	  }
	}
      }

      /*!
       * Encode data in base64.
       *
       * \param data the data stream to encode
       *
       * \return the base64 encoded data
       */
      template<class CharT, class Traits>
      static std::string encode(std::basic_istream<CharT,Traits>& data)	noexcept {
	std::ostringstream out;
	encode(data, out);
	return out.str();
      }

      /*!
       * Encode data in base64.
       *
       * \param data the data string to encode
       * \param output the stream to write in
       */
      template<class CharT, class Traits, class Allocator>
      static void encode(const std::basic_string<CharT,Traits,Allocator>& data,
			 std::basic_ostream<CharT,Traits>& output) noexcept {
	encode(reinterpret_cast<const uint8_t*>(data.data()), data.length(), output);
      }

      /*!
       * Encode data in base64.
       *
       * \param data the data string to encode
       *
       * \return the base64 encoded data
       */
      template<class CharT, class Traits, class Allocator>
      static std::string encode(const std::basic_string<CharT,Traits,Allocator>& data)
	noexcept {
	std::ostringstream out;
	encode(data, out);
	return out.str();
      }

      /*!
       * Encode data in base64.
       *
       * \param data the data bytes to encode
       * \param length the data length
       * \param output the stream to write in
       */
      template<class CharT, class Traits>
      static void encode(const uint8_t* data, uint64_t length,
			 std::basic_ostream<CharT,Traits>& output)
	noexcept {
	char buffer[5];
	buffer[4] = '\0';
	while(length > 0) {
	  uint8_t read = encode(data, length, buffer);
	  output << buffer;
	  data += read;
	  length -= read;
	}
      }

      /*!
       * Encode data in base64.
       *
       * \param data the data bytes to encode
       * \param length the data length
       *
       * \return the base64 encoded data
       */
      static std::string encode(const uint8_t* data, uint64_t length) noexcept {
	std::ostringstream out;
	encode(data, length, out);
	return out.str();
      }

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
      template<class CharT, class Traits>
      static void decode(std::istream& data,
			 std::basic_ostream<CharT,Traits>& output)
	noexcept {
	if(data) {
	  char buffer[1024];
	  while(!data.eof()) {
	    data.read(buffer, 1024);
	    decode(buffer, static_cast<uint64_t>(data.gcount()), output);
	  }
	}
      }

      /*!
       * Decode data in base64.
       *
       * \param data the data stream to decode
       *
       * \return the base64 decoded data
       */
      static std::string decode(std::istream& data) noexcept {
	std::ostringstream out;
	decode(data, out);
	return out.str();
      }

      /*!
       * Decode data in base64.
       *
       * \param data the data string to decode
       * \param output the stream to write in
       */
      template<class CharT, class Traits>
      static void decode(const std::string& data,
			 std::basic_ostream<CharT,Traits>& output) noexcept {
	decode(data.data(), static_cast<uint64_t>(data.length()), output);
      }

      /*!
       * Decode data in base64.
       *
       * \param data the data string to decode
       *
       * \return the base64 decoded data
       */
      static std::string decode(const std::string& data)
	noexcept {
	std::ostringstream out;
	decode(data, out);
	return out.str();
      }

      /*!
       * Decode data in base64.
       *
       * \param data the data bytes to decode
       * \param length the data length
       * \param output the stream to write in
       */
      template<class CharT, class Traits>
      static void decode(const char* data, uint64_t length,
			 std::basic_ostream<CharT,Traits>& output)
	noexcept {
	uint8_t buffer[4];
	buffer[3] = '\0';
	while(length > 0) {
	  memset(buffer, '\0', 3);
	  decode(data, length, buffer);
	  output << buffer;
	  data += 4;
	  length -= 4;
	}
      }

      /*!
       * Decode data in base64.
       *
       * \param data the data bytes to decode
       * \param length the data length
       *
       * \return the base64 decoded data
       */
      static std::string decode(const char* data, uint64_t length)
	noexcept {
	std::ostringstream out;
	decode(data, length, out);
	return out.str();
      }

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

#endif // _ANCH_CRYPTO_B2T_BASE64_H_
