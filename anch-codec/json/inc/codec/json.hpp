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

#include "anch-codec-json-export.hpp"

#include <istream>
#include <ostream>
#include <string_view>

#include "json/json.hpp"
#include "json/mappingOptions.hpp"

#include "codec/codec.hpp"

namespace anch::codec {

  /*!
   * \c application/json codec based on anch-json (anch-codec-json plugin, option \c ANCH_CODEC_JSON ): values need
   * an anch-json mapper (\c anch::json::registerObject ).\n
   * \n
   * anch-rest servers and clients use mapping options of their side ( \ref options ): fields constraints (
   * \c anch::json::required , \c anch::json::minLength , ...) are checked on values the server receives (invalid
   * request body: \c 400 response with the path of the invalid value) and on values the client sends (invalid
   * request is not sent: \c anch::json::MappingError is raised by \c Call::body ). Server responses are not
   * checked by the server (a response which violates constraints is still sent) nor by the client (server responses
   * are trusted).
   *
   * \author Vincent Lachenal
   *
   * \since 0.3 (\c anch::rest::Json in 0.2)
   */
  struct ANCH_CODEC_JSON_EXPORT Json {

    /*!
     * Media type
     *
     * \return \c application/json
     */
    static std::string_view mediaType() noexcept;

    /*!
     * Mapping options of a side. They are copied from \c anch::json::DEFAULT_MAPPING_OPTIONS on first use, then
     * constraints checks are set:
     * - server: \c deserialize_check_constraints is \c true , \c serialize_check_constraints is \c false , request
     *   bodies are limited to 2 MiB ( \c max_payload , when not set in \c anch::json::DEFAULT_MAPPING_OPTIONS ):
     *   larger ones raise \ref ContentTooLarge ( \c 413 );
     * - client: \c serialize_check_constraints is \c true , \c deserialize_check_constraints is \c false . Server
     *   responses are trusted and read with tolerance: unknown fields are ignored
     *   ( \c deserialize_ignore_unknown_field is \c true ) and whitespaces are accepted
     *   ( \c deserialize_max_discard_char is \c -1 ).
     *
     * Options can be changed at application initialization (they are not protected against concurrent changes).
     *
     * \param side the side
     *
     * \return the mapping options of the side
     *
     * \since 0.2
     */
    static anch::json::MappingOptions& options(anch::codec::Side side);

    /*!
     * Serialize value
     *
     * \param value the value
     * \param out the output stream
     */
    template<typename T>
    requires (!anch::codec::ExternalSchema<T>::value)
    static void serialize(const T& value, std::ostream& out);

    /*!
     * Serialize value with side mapping options
     *
     * \param value the value
     * \param out the output stream
     * \param side the side
     *
     * \throw anch::json::MappingError constraint violation (when constraints are checked on serialization)
     *
     * \since 0.2
     */
    template<typename T>
    requires (!anch::codec::ExternalSchema<T>::value)
    static void serialize(const T& value, std::ostream& out, anch::codec::Side side);

    /*!
     * Deserialize value
     *
     * \param value the value to fill
     * \param input the input stream
     *
     * \throw ContentTooLarge input is larger than \c max_payload option
     * \throw anch::json::MappingError on invalid JSON
     */
    template<typename T>
    requires (!anch::codec::ExternalSchema<T>::value)
    static void deserialize(T& value, std::istream& input);

    /*!
     * Deserialize value with side mapping options
     *
     * \param value the value to fill
     * \param input the input stream
     * \param side the side
     *
     * \throw ContentTooLarge input is larger than \c max_payload option
     * \throw anch::json::MappingError on invalid JSON or constraint violation (when constraints are checked on
     *        deserialization)
     *
     * \since 0.2
     */
    template<typename T>
    requires (!anch::codec::ExternalSchema<T>::value)
    static void deserialize(T& value, std::istream& input, anch::codec::Side side);

  private:
    /*!
     * Serialize value with mapping options
     *
     * \param value the value
     * \param out the output stream
     * \param options the mapping options
     */
    template<typename T>
    static void serialize(const T& value, std::ostream& out, const anch::json::MappingOptions& options);

    /*!
     * Deserialize value with mapping options
     *
     * \param value the value to fill
     * \param input the input stream
     * \param options the mapping options
     */
    template<typename T>
    static void deserialize(T& value, std::istream& input, const anch::json::MappingOptions& options);

  };

}

#include "codec/impl/json.hpp"
