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
  struct MappingOptions {

    // Serialization +
    /*! Serialize empty character' string as \c "" . Default to \c true ; std::optional or pointer can be used */
    bool serialize_empty_string = true;

    /*! Serialize empty collection as \c [] . Default to \c true ; std::optional or pointer can be used */
    bool serialize_empty_array = true;

    /*! Serialize null or empty string as \c null . Default to \c false */
    bool serialize_null = false;
    // Serialization -

    // Deserialization +
    /*! Unknown will not raise error if \c true ; it can be usefull for client side implementation */
    bool deserialize_ignore_unknown_field = false;

    /*! Maximum discard characters before raising error. Use \c -1 value for unlimited discard characters. It can be usefull at development stage */
    int deserialize_max_discard_char = 0;

    /*! Maximum field characters before raising error. Use \c -1 value for unlimited field's value characters */
    int deserialize_max_field_char = 512;
    // Deserialization -

  };

  /*! Default mapping options. It is not a constant so it can be changed in your application initialization. */
  extern MappingOptions DEFAULT_MAPPING_OPTIONS;

}  // anch::json
