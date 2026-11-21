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

#include <exception>

#include "json/format.hpp"

#include "codec/codec.hpp"
#include "codec/formatCodec.hpp"

namespace anch::codec {

  /*!
   * \brief Policy of the JSON codec (anch-codec-json plugin, option \c ANCH_CODEC_JSON )
   *
   * Options of each side are copied from \c anch::json::DEFAULT_MAPPING_OPTIONS on first use, then:
   * - \c SERVER : \c deserialize_check_constraints is \c true , \c serialize_check_constraints is \c false , read
   *   values are limited to 2 MiB ( \c max_payload , when not set in \c anch::json::DEFAULT_MAPPING_OPTIONS ):
   *   larger ones raise \ref ContentTooLarge ( \c 413 in anch-rest servers) ;
   * - \c CLIENT : \c serialize_check_constraints is \c true , \c deserialize_check_constraints is \c false . Read
   *   values are trusted and read with tolerance: unknown fields are ignored ( \c deserialize_ignore_unknown_field
   *   is \c true ) and whitespaces are accepted ( \c deserialize_max_discard_char is \c -1 ).
   *
   * anch-rest servers check the constraints of request bodies (invalid body: \c 400 response with the path of the
   * invalid value) and clients the constraints of request bodies they send ( \c anch::json::MappingError is raised by
   * \c Call::body ).
   *
   * \since 0.3
   */
  template<>
  struct ANCH_CODEC_JSON_EXPORT FormatPolicy<anch::json::Json> {
    /*!
     * Configure the options of a side
     *
     * \param options the options
     * \param side the side
     */
    static void configure(anch::json::MappingOptions& options, anch::codec::Side side);

    /*!
     * Translate \c PAYLOAD_TOO_LARGE mapping errors into \ref ContentTooLarge
     *
     * \param error the error
     */
    static void translate(const std::exception& error);
  };

  /*!
   * \c application/json codec based on anch-json (anch-codec-json plugin, option \c ANCH_CODEC_JSON ): values need
   * a description ( \c anch::Mapping , anch-utils). Options of each side: see
   * \ref FormatPolicy<anch::json::Json> "FormatPolicy".
   *
   * \since 0.3 (\c anch::rest::Json in 0.2)
   */
  using Json = anch::codec::FormatCodec<anch::json::Json>;

}

// Options of each side are instantiated in anch-codec-json (unique in the program)
extern template struct ANCH_CODEC_JSON_EXPORT anch::codec::FormatCodec<anch::json::Json>;
