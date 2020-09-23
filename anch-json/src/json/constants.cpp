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
#include "json/constants.hpp"

const char anch::json::STRING_DELIMITER = '"';
const char anch::json::FIELD_SEPARATOR = ',';
const char anch::json::FIELD_VALUE_SEPARATOR = ':';
const char anch::json::OBJECT_BEGIN = '{';
const char anch::json::OBJECT_END = '}';
const char anch::json::ARRAY_BEGIN = '[';
const char anch::json::ARRAY_END = ']';
const std::optional<std::string> anch::json::EMPTY_FIELD = std::optional<std::string>();
