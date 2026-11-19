/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2020 Vincent Lachenal

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

#include "anch-json-export.hpp"

#include <cstddef>
#include <stdint.h>

namespace anch::json {

  /*!
   * \brief JSON mapping options
   *
   * Options are used to serialization options and deserialization behavior on unexcepted input stream
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct ANCH_JSON_EXPORT MappingOptions {

    // Serialization +
    /*! Serialize empty collection as \c [] or \c {} for map . Default to \c true ; std::optional or pointer can be used */
    bool serialize_empty_col = true;

    /*! Serialize null or empty string as \c null . Default to \c false */
    bool serialize_null = false;

    /*! Serialization indentation number of spaces (\c 0 means no prettifying). Default to \c 0 */
    uint16_t prettify_nbs = 0;

    /*! Check fields constraints ( \c minLength , \c minimum , ... given to \c ObjectMapper::registerField ) before
     * writing values: \ref anch::json::MappingError is raised with \c CONSTRAINT_VIOLATION error code (the output
     * stream contains the JSON written before the error). Default to \c false : usually enabled on client side to
     * not send invalid requests, disabled on server side where responses are built by the server itself.\n
     * Required fields are always written ( \c null , \c [] or \c {} when empty, whatever \c serialize_null and
     * \c serialize_empty_col options are).
     *
     * \since 0.2
     */
    bool serialize_check_constraints = false;
    // Serialization -

    // Deserialization +
    /*! Unknown will not raise error if \c true ; it can be usefull for client side implementation */
    bool deserialize_ignore_unknown_field = false;

    /*! Maximum discard characters before raising error. Use \c -1 value for unlimited discard characters. It can be usefull at development stage */
    int64_t deserialize_max_discard_char = 0;

    /*! Maximum number of characters read from input stream before raising error
     * ( \ref anch::json::ErrorCode::PAYLOAD_TOO_LARGE ). Use \c 0 value for unlimited payload. The length of a string
     * field is limited with its \c maxLength constraint. */
    std::size_t max_payload = 0;

    /*! Check fields constraints ( \c required , \c minLength , \c minimum , ... given to \c ObjectMapper::registerField )
     * after reading values: \ref anch::json::MappingError is raised with \c MISSING_FIELD or \c CONSTRAINT_VIOLATION
     * error code. Default to \c true : usually enabled on server side to refuse invalid requests, disabled on client
     * side where server responses are trusted.
     *
     * \since 0.2
     */
    bool deserialize_check_constraints = true;

    /*! Maximum nesting depth of objects and arrays (root object or array is level \c 1 ) before raising error. It protects parser against stack overflow. Use \c 0 value for unlimited depth. Default to \c 512 */
    std::size_t max_depth = 512;
    // Deserialization -

    /*! Buffer size (used for deserialization for now) */
    std::size_t buffer_size = 1024;

  };

  /*! Default mapping options. It is not a constant so it can be changed in your application initialization. */
  extern ANCH_JSON_EXPORT MappingOptions DEFAULT_MAPPING_OPTIONS;

}  // anch::json
