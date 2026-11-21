/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include <cstddef>
#include <istream>
#include <ostream>
#include <string>

#include "codec/codec.hpp"
#include "codec/text.hpp"

#include "rest/bodyStream.hpp"
#include "rest/mediaType.hpp"

/*!
 * \file rest/codec.hpp
 * \deprecated removed in 0.4: codecs have moved to anch-codec, use \c codec/codec.hpp , \c codec/text.hpp and
 *             \c rest/bodyStream.hpp
 */

namespace anch::rest {

  /*! \deprecated removed in 0.4, use \ref anch::codec::Side */
  using Side [[deprecated("removed in 0.4, use anch::codec::Side")]] = anch::codec::Side;

  /*! \deprecated removed in 0.4, use \ref anch::codec::ContentTooLarge */
  using ContentTooLarge [[deprecated("removed in 0.4, use anch::codec::ContentTooLarge")]] = anch::codec::ContentTooLarge;

  /*! \deprecated removed in 0.4, use \ref anch::codec::Text */
  using Text [[deprecated("removed in 0.4, use anch::codec::Text")]] = anch::codec::Text;

  /*! \deprecated removed in 0.4, use \ref anch::codec::Serializer */
  template<typename C, typename T>
  concept Serializer [[deprecated("removed in 0.4, use anch::codec::Serializer")]] = anch::codec::Serializer<C, T>;

  /*! \deprecated removed in 0.4, use \ref anch::codec::Deserializer */
  template<typename C, typename T>
  concept Deserializer [[deprecated("removed in 0.4, use anch::codec::Deserializer")]] = anch::codec::Deserializer<C, T>;

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::readContent
   *
   * \param input the input stream
   * \param maxSize the maximum content size in bytes
   *
   * \return the content
   */
  [[deprecated("removed in 0.4, use anch::codec::readContent")]]
  inline std::string
  readContent(std::istream& input, std::size_t maxSize) {
    return anch::codec::readContent(input, maxSize);
  }



  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::contentTypeOf
   *
   * \param value the value
   *
   * \return the media type
   */
  template<typename C, typename T>
  requires anch::codec::Serializer<C, T>
  [[deprecated("removed in 0.4, use anch::codec::contentTypeOf")]]
  anch::codec::MediaType
  contentTypeOf(const T& value) {
    return anch::codec::contentTypeOf<C>(value);
  }

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::serialize
   *
   * \param value the value
   * \param out the output stream
   * \param contentType the content media type
   * \param side the side which serializes value
   */
  template<typename C, typename T>
  requires anch::codec::Serializer<C, T>
  [[deprecated("removed in 0.4, use anch::codec::serialize")]]
  void
  serializeBody(const T& value, std::ostream& out, const anch::codec::MediaType& contentType, anch::codec::Side side) {
    anch::codec::serialize<C>(value, out, contentType, side);
  }

  /*!
   * \deprecated removed in 0.4, use \ref anch::codec::deserialize
   *
   * \param value the value to fill
   * \param input the input stream
   * \param contentType the content media type
   * \param side the side which deserializes value
   */
  template<typename C, typename T>
  requires anch::codec::Deserializer<C, T>
  [[deprecated("removed in 0.4, use anch::codec::deserialize")]]
  void
  deserializeBody(T& value, std::istream& input, const anch::codec::MediaType& contentType, anch::codec::Side side) {
    anch::codec::deserialize<C>(value, input, contentType, side);
  }

}
