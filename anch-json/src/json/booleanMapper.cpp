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
#include "json/primitiveMapper.hpp"

#include <string>
#include <set>
#include <vector>
#include <list>
#include <ostream>
#include <istream>
#include <sstream>

#include "json/parser.hpp"

using anch::json::JSONPrimitiveMapper;

inline
void
serializeValue(const bool& value, std::ostream& out) {
  out << (value ? "true" : "false");
}

inline
bool
isNumericChar(int car) {
  return car == 't'
    || car == 'r'
    || car == 'u'
    || car == 'e'
    || car == 'f'
    || car == 'a'
    || car == 'l'
    || car == 's'
    ;
}

void
deserializeValue(bool& value, std::istream& input) {
  std::ostringstream buffer;
  int current;
  while(input) {
    // \todo max char in value ...
    current = input.peek();
    if(!isNumericChar(current)) {
      break;
    }
    input.get();
    buffer << static_cast<char>(current);
  }
  std::string strVal = buffer.str();
  if(strVal == "true") {
    value = true;
  } else if(strVal == "false") {
    value = false;
  } else {
    throw 64; // \todo error
  }
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
  serializeValue(value, out);
  return true;
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const bool* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const std::optional<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const std::vector<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, field);
  return true;
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const std::list<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, field);
  return true;
}

template<>
bool
JSONPrimitiveMapper<bool>::serialize(const std::set<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, field);
  return true;
}

template<>
void
JSONPrimitiveMapper<bool>::deserialize(bool& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeValue(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<bool>::deserialize(std::optional<bool>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    bool parsed;
    deserializeValue(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<bool>::deserialize(bool* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new bool();
    deserializeValue(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<bool>::deserialize(std::vector<bool>& value, std::istream& input) {
  anch::json::deserializeArray<bool>(input,
				     [&value](const bool& str) -> void { value.push_back(str); },
				     std::function<void(bool&,std::istream&)>(deserializeValue));
}

template<>
void
JSONPrimitiveMapper<bool>::deserialize(std::list<bool>& value, std::istream& input) {
  anch::json::deserializeArray<bool>(input,
				     [&value](const bool& str) -> void { value.push_back(str); },
				     std::function<void(bool&,std::istream&)>(deserializeValue));
}

template<>
void
JSONPrimitiveMapper<bool>::deserialize(std::set<bool>& value, std::istream& input) {
  anch::json::deserializeArray<bool>(input,
				     [&value](const bool& str) -> void { value.insert(str); },
				     std::function<void(bool&,std::istream&)>(deserializeValue));
}

template class JSONPrimitiveMapper<bool>;
