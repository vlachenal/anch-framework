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

#include "anch-codec-export.hpp"

#include <concepts>
#include <cstddef>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include "codec/mediaType.hpp"

namespace anch::codec {

  /*!
   * Trust policy of a codec user: what is checked and bounded when values are read and written.\n
   * \c SERVER reads data it does not control and trusts what it builds ; \c CLIENT checks what it sends and trusts
   * what it reads. Codecs can behave according to it: \ref anch::codec::Json checks constraints of values read with
   * \c SERVER and of values written with \c CLIENT , and bounds what \c SERVER reads.\n
   * anch-rest servers use \c SERVER and clients \c CLIENT ; a message consumer reads with \c SERVER and a producer
   * writes with \c CLIENT .
   *
   * \since 0.2
   */
  enum class Side {
    /*! Read values are checked and bounded, written values are trusted (HTTP server, message consumer) */
    SERVER,
    /*! Written values are checked, read values are trusted (HTTP client, message producer) */
    CLIENT
  };

  /*!
   * Content is larger than a codec limit. anch-rest servers answer \c 413 (Content Too Large) when it is raised by
   * a request body deserialization.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_CODEC_EXPORT ContentTooLarge: public std::length_error {
  public:
    /*! Use \c std::length_error constructors */
    using std::length_error::length_error;
  };

  /*!
   * Read whole content
   *
   * \param input the input stream
   * \param maxSize the maximum content size in bytes
   *
   * \return the content
   *
   * \throw ContentTooLarge when content is larger than \c maxSize
   *
   * \since 0.2
   */
  std::string readContent(std::istream& input, std::size_t maxSize);

  /*!
   * Maximum size of contents read in memory by codecs which have no limit of their own (Protobuf, Protobuf JSON,
   * Thrift), to change before reading contents (\c 0 for no limit).\n
   * Default: 2 MiB for \c SERVER , as JSON and form bodies ; no limit for \c CLIENT (anch-rest responses are
   * bounded by transports: \c TransportConfig::maxResponseSize ).
   *
   * \param side the side
   *
   * \return the maximum size in bytes
   *
   * \since 0.3
   */
  ANCH_CODEC_EXPORT std::size_t& maxBodySize(anch::codec::Side side);

  /*!
   * Read whole content with the limit of a side (\ref maxBodySize )
   *
   * \param input the input stream
   * \param side the side
   *
   * \return the content
   *
   * \throw ContentTooLarge when content is larger than \ref maxBodySize
   *
   * \since 0.3
   */
  std::string readBody(std::istream& input, anch::codec::Side side);

  /*!
   * Codec which serializes \c T values.\n
   * A codec is a type with static functions:
   * - \c mediaType() : the media type it handles
   * - \c serialize(const T&, std::ostream&) : write value
   * - \c deserialize(T&, std::istream&) : read value (\ref anch::codec::Deserializer "Deserializer" ). Errors are
   *   reported with exceptions.
   *
   * Codecs can also have richer functions (one \c serialize / \c deserialize form is enough), which are used when
   * they are defined (see \ref anch::codec::contentTypeOf , \ref anch::codec::serialize and
   * \ref anch::codec::deserialize ):
   * - \c contentType(const T&) -> \ref anch::codec::MediaType "MediaType" : content type of the serialized value,
   *   computed before serialization (a multipart codec adds its \c boundary parameter) ;
   * - \c serialize(const T&, std::ostream&, anch::codec::Side) or
   *   \c serialize(const T&, std::ostream&, const anch::codec::MediaType&, anch::codec::Side) : the media type is
   *   the one returned by \c contentType() ;
   * - \c deserialize(T&, std::istream&, anch::codec::Side) or
   *   \c deserialize(T&, std::istream&, const anch::codec::MediaType&, anch::codec::Side) : the media type is the
   *   content type of the read value, with its parameters ( \c charset , \c boundary , ...).
   *
   * anch-rest client also uses \c stream(const T&, const anch::codec::MediaType&, anch::codec::Side) ->
   * \c anch::rest::BodyStream when it is defined: it sends the stream instead of serializing the value in memory.
   *
   * \tparam C the codec type
   * \tparam T the value type
   *
   * \since 0.2
   */
  template<typename C, typename T>
  concept Serializer = requires {
    { C::mediaType() } -> std::convertible_to<std::string_view>;
  } && (requires(const T& value, std::ostream& out) { C::serialize(value, out); }
	|| requires(const T& value, std::ostream& out, anch::codec::Side side) { C::serialize(value, out, side); }
	|| requires(const T& value, std::ostream& out, const anch::codec::MediaType& contentType, anch::codec::Side side) {
	  C::serialize(value, out, contentType, side);
	});

  /*!
   * Codec which deserializes \c T values (see \ref anch::codec::Serializer "Serializer" )
   *
   * \tparam C the codec type
   * \tparam T the value type
   *
   * \since 0.2
   */
  template<typename C, typename T>
  concept Deserializer = requires {
    { C::mediaType() } -> std::convertible_to<std::string_view>;
  } && (requires(T& value, std::istream& input) { C::deserialize(value, input); }
	|| requires(T& value, std::istream& input, anch::codec::Side side) { C::deserialize(value, input, side); }
	|| requires(T& value, std::istream& input, const anch::codec::MediaType& contentType, anch::codec::Side side) {
	  C::deserialize(value, input, contentType, side);
	});

  /*!
   * Get content type of a value serialized with codec: its \c contentType(value) function is used when it is
   * defined, \c mediaType() otherwise
   *
   * \tparam C the codec type
   * \tparam T the value type
   *
   * \param value the value
   *
   * \return the media type
   *
   * \throw std::invalid_argument when codec media type is invalid
   *
   * \since 0.2
   */
  template<typename C, typename T>
  requires anch::codec::Serializer<C, T>
  anch::codec::MediaType contentTypeOf(const T& value);

  /*!
   * Serialize value with codec: its \c serialize(value, out, contentType, side) function is used when it is
   * defined, then \c serialize(value, out, side) , then \c serialize(value, out)
   *
   * \tparam C the codec type
   * \tparam T the value type
   *
   * \param value the value
   * \param out the output stream
   * \param contentType the content media type (from \ref contentTypeOf )
   * \param side the side which serializes value
   *
   * \since 0.3 (\c anch::rest::serializeBody in 0.2)
   */
  template<typename C, typename T>
  requires anch::codec::Serializer<C, T>
  void serialize(const T& value, std::ostream& out, const anch::codec::MediaType& contentType, anch::codec::Side side);

  /*!
   * Deserialize value with codec: its \c deserialize(value, input, contentType, side) function is used when it is
   * defined, then \c deserialize(value, input, side) , then \c deserialize(value, input)
   *
   * \tparam C the codec type
   * \tparam T the value type
   *
   * \param value the value to fill
   * \param input the input stream
   * \param contentType the content media type, with its parameters
   * \param side the side which deserializes value
   *
   * \since 0.3 (\c anch::rest::deserializeBody in 0.2)
   */
  template<typename C, typename T>
  requires anch::codec::Deserializer<C, T>
  void deserialize(T& value, std::istream& input, const anch::codec::MediaType& contentType, anch::codec::Side side);

}

#include "codec/impl/codec.hpp"
