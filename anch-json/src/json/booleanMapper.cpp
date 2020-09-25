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
#include "json/mapper.hpp"

#include <string>
#include <set>
#include <vector>
#include <list>
#include <ostream>

using anch::json::JSONPrimitiveMapper;

template<typename T>
bool
serializeBooleanArray(const T& array, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << anch::json::ARRAY_BEGIN;
  for(auto iter = array.begin() ; iter != array.end() ; ++iter) {
    out << (*iter ? "true" : "false") << anch::json::FIELD_SEPARATOR; // \todo last ','
  }
  out << anch::json::ARRAY_END;
  return true;
}

// JSONPrimitiveMapper specialization for bool +
template<>
JSONPrimitiveMapper<bool>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<bool>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const bool& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << (value ? "true" : "false");
  return true;
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const bool* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << (*value ? "true" : "false");
  return true;
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const std::optional<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << (value.value() ? "true" : "false");
  return true;
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const std::vector<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeBooleanArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const std::list<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeBooleanArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const std::set<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeBooleanArray(value, out, field);
}
template class JSONPrimitiveMapper<bool>;
