/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#ifndef ANCH_JSON_MAX_DISCARD_CHAR
#define ANCH_JSON_MAX_DISCARD_CHAR 128
#endif

#ifndef ANCH_JSON_MAX_FIELD_CHAR
#define ANCH_JSON_MAX_FIELD_CHAR 1024
#endif

namespace anch {
  namespace json {

    /*!
     * \brief JSON mapping options
     *
     * Options are used to serialization options and deserialization behavior on unexcepted input stream
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    struct MappingOptions {

      // Serialization +
      /*! Serialize empty character' string as null. Default to \c true ; std::optional or pointer can be used */
      bool serialize_empty_string = true;

      /*! Serialize empty collection as null. Default to \c true ; std::optional or pointer can be used */
      bool serialize_empty_array = true;
      // Serialization -

      // Deserialization +
      /*! Unknown will not raise error if \c true ; it can be usefull for client side implementation */
      bool deserialize_ignore_unknown_field = false;

      /*! Maximum discard characters before raising error. Value can be override at compile time with -DANCH_JSON_MAX_DISCARD_CHAR or at runtime. */
      int deserialize_max_discard_char = ANCH_JSON_MAX_DISCARD_CHAR;

      /*! Maximum field characters before raising error. Value can be override at compile time with -DANCH_JSON_MAX_FIELD_CHAR or at runtime. */
      int deserialize_field_char = ANCH_JSON_MAX_FIELD_CHAR;
      // Deserialization -

    };

    /*! Default mapping options. It is not a constant so it can be changed in your application initialization. */
    extern MappingOptions DEFAULT_MAPPING_OPTIONS;

  }  // json
}  // anch
