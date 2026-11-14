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

#include <concepts>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>

#include "rest/mediaType.hpp"

namespace anch::rest {

  /*!
   * Side which uses a codec. Codecs can behave according to it: \ref anch::rest::Json checks constraints of values
   * the server receives and of values the client sends.
   *
   * \since 0.2
   */
  enum class Side {
    /*! Server: request bodies are read, responses bodies are written */
    SERVER,
    /*! Client: request bodies are written, responses bodies are read */
    CLIENT
  };

  /*!
   * Codec which serializes \c T values.\n
   * A codec is a type with static functions:
   * - \c mediaType() : the media type it handles
   * - \c serialize(const T&, std::ostream&) : write value
   * - \c deserialize(T&, std::istream&) : read value (\ref anch::rest::Deserializer "Deserializer" ). Errors are reported with exceptions.
   *
   * Codecs can also have richer functions (one \c serialize / \c deserialize form is enough), which server and
   * client use when they are defined (see
   * \ref anch::rest::contentTypeOf , \ref anch::rest::serializeBody and \ref anch::rest::deserializeBody ):
   * - \c contentType(const T&) -> \ref anch::rest::MediaType "MediaType" : \c Content-Type of the serialized
   *   value, computed before serialization (a multipart codec adds its \c boundary parameter) ;
   * - \c serialize(const T&, std::ostream&, anch::rest::Side) or
   *   \c serialize(const T&, std::ostream&, const anch::rest::MediaType&, anch::rest::Side) : the media type is
   *   the one returned by \c contentType() ;
   * - \c deserialize(T&, std::istream&, anch::rest::Side) or
   *   \c deserialize(T&, std::istream&, const anch::rest::MediaType&, anch::rest::Side) : the media type is the
   *   \c Content-Type of the body, with its parameters ( \c charset , \c boundary , ...).
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
	|| requires(const T& value, std::ostream& out, anch::rest::Side side) { C::serialize(value, out, side); }
	|| requires(const T& value, std::ostream& out, const anch::rest::MediaType& contentType, anch::rest::Side side) {
	  C::serialize(value, out, contentType, side);
	});

  /*!
   * Codec which deserializes \c T values (see \ref anch::rest::Serializer "Serializer" )
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
	|| requires(T& value, std::istream& input, anch::rest::Side side) { C::deserialize(value, input, side); }
	|| requires(T& value, std::istream& input, const anch::rest::MediaType& contentType, anch::rest::Side side) {
	  C::deserialize(value, input, contentType, side);
	});

  /*!
   * Get \c Content-Type of a value serialized with codec: its \c contentType(value) function is used when it is
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
  requires anch::rest::Serializer<C, T>
  anch::rest::MediaType contentTypeOf(const T& value);

  /*!
   * Serialize value with codec: its \c serialize(value, out, contentType, side) function is used when it is
   * defined, then \c serialize(value, out, side) , then \c serialize(value, out)
   *
   * \tparam C the codec type
   * \tparam T the value type
   *
   * \param value the value
   * \param out the output stream
   * \param contentType the body media type (from \ref contentTypeOf )
   * \param side the side which serializes value
   *
   * \since 0.2
   */
  template<typename C, typename T>
  requires anch::rest::Serializer<C, T>
  void serializeBody(const T& value, std::ostream& out, const anch::rest::MediaType& contentType, anch::rest::Side side);

  /*!
   * Deserialize value with codec: its \c deserialize(value, input, contentType, side) function is used when it is
   * defined, then \c deserialize(value, input, side) , then \c deserialize(value, input)
   *
   * \tparam C the codec type
   * \tparam T the value type
   *
   * \param value the value to fill
   * \param input the input stream
   * \param contentType the body media type ( \c Content-Type with its parameters)
   * \param side the side which deserializes value
   *
   * \since 0.2
   */
  template<typename C, typename T>
  requires anch::rest::Deserializer<C, T>
  void deserializeBody(T& value, std::istream& input, const anch::rest::MediaType& contentType, anch::rest::Side side);

  /*!
   * \c text/plain codec: serializes values with stream injection operator, deserializes strings.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct Text {

    /*!
     * Media type
     *
     * \return \c text/plain
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Write value
     *
     * \param value the value
     * \param out the output stream
     */
    template<typename T>
    requires requires(std::ostream& out, const T& value) { out << value; }
    static void serialize(const T& value, std::ostream& out);

    /*!
     * Read the whole input
     *
     * \param value the value to fill
     * \param input the input stream
     */
    static void deserialize(std::string& value, std::istream& input);

  };

}

#include "rest/impl/codec.hpp"
