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

#include <string>
#include <optional>

namespace anch::json {

  /*! JSON caracters string delimiter ('"') */
  const char STRING_DELIMITER = '"';

  /*! JSON fields delimiter (',') */
  const char FIELD_SEPARATOR = ',';

  /*! JSON fiel/value delimiter (':') */
  const char FIELD_VALUE_SEPARATOR = ':';

  /*! JSON start object ('{') */
  const char OBJECT_BEGIN = '{';

  /*! JSON end object ('}') */
  const char OBJECT_END = '}';

  /*! JSON start array ('[') */
   const char ARRAY_BEGIN = '[';

  /*! JSON start array (']') */
  const char ARRAY_END =']';

  /*! No field name constant value */
  extern const std::optional<std::string> EMPTY_FIELD;

}  // json::anch
