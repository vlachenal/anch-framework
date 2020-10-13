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
#include <string_view>
#include <set>
#include <vector>
#include <list>
#include <ostream>
#include <sstream>

#ifdef ANCH_UUID
#include "uuid.hpp"
#endif

#ifdef ANCH_DATE
#include "date/date.hpp"
#endif

#include "json/constants.hpp"
#include "json/parser.hpp"


using anch::json::JSONPrimitiveMapper;

template<typename T>
bool
serializeStringArray(const T& array, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << anch::json::ARRAY_BEGIN;
  for(auto iter = array.begin() ; iter != array.end() ; ++iter) {
    if(iter != array.begin()) {
      out << anch::json::FIELD_SEPARATOR;
    }
    out << anch::json::STRING_DELIMITER << *iter << anch::json::STRING_DELIMITER;
  }
  out << anch::json::ARRAY_END;
  return true;
}

void
deserializeNonNull(std::string& value, std::istream& input) {
  // Look for '"'
  int current = input.get();
  if(current != anch::json::STRING_DELIMITER) {
    throw 2;
  }
  std::ostringstream buffer;
  while(input) {
    // \todo max char in value ...
    current = input.get();
    if(current == '\\') {
      if(!input) {
	throw 0;
      }
      int next = input.get();
      if(next == 'u') { // unicode sequence
	buffer << '\\' << static_cast<char>(next); // insert '\' to keep unicode sequence intact
      } else {
	buffer << static_cast<char>(next);
      }
    } else if(current == anch::json::STRING_DELIMITER) {
      break;
    } else {
      buffer << static_cast<char>(current);
    }
  }
  if(!input && current != anch::json::STRING_DELIMITER) {
    throw 4096;
  }
  value = buffer.str();
}

// JSONPrimitiveMapper specialization for std::string +
template<>
JSONPrimitiveMapper<std::string>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<std::string>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<std::string>::serialize(const std::string& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << anch::json::STRING_DELIMITER << value << anch::json::STRING_DELIMITER;
  return true;
}

template<>
bool
JSONPrimitiveMapper<std::string>::serialize(const std::string* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<std::string>::serialize(const std::optional<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<std::string>::serialize(const std::vector<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeStringArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<std::string>::serialize(const std::list<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeStringArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<std::string>::serialize(const std::set<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeStringArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<std::string>::deserialize(std::string& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<std::string>::deserialize(std::optional<std::string>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    std::string parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<std::string>::deserialize(std::string* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new std::string();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<std::string>::deserialize(std::vector<std::string>& value, std::istream& input) {
  anch::json::deserializeArray<std::string>(input,
					    [&value](const std::string& str) -> void { value.push_back(str); },
					    std::function<void(std::string&,std::istream&)>(deserializeNonNull));
}

template<>
void
JSONPrimitiveMapper<std::string>::deserialize(std::list<std::string>& value, std::istream& input) {
  anch::json::deserializeArray<std::string>(input,
					    [&value](const std::string& str) -> void { value.push_back(str); },
					    std::function<void(std::string&,std::istream&)>(deserializeNonNull));
}

template<>
void
JSONPrimitiveMapper<std::string>::deserialize(std::set<std::string>& value, std::istream& input) {
  anch::json::deserializeArray<std::string>(input,
					    [&value](const std::string& str) -> void { value.insert(str); },
					    std::function<void(std::string&,std::istream&)>(deserializeNonNull));
}

template class JSONPrimitiveMapper<std::string>;
// JSONPrimitiveMapper specialization for std::string -

// JSONPrimitiveMapper specialization for std::string_view +
template<>
JSONPrimitiveMapper<std::string_view>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<std::string_view>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<std::string_view>::serialize(const std::string_view& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << anch::json::STRING_DELIMITER << value << anch::json::STRING_DELIMITER;
  return true;
}

template<>
bool
JSONPrimitiveMapper<std::string_view>::serialize(const std::string_view* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<std::string_view>::serialize(const std::optional<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<std::string_view>::serialize(const std::vector<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeStringArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<std::string_view>::serialize(const std::list<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeStringArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<std::string_view>::serialize(const std::set<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeStringArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<std::string_view>::deserialize(std::string_view& value, std::istream& input) = delete;

template<>
void
JSONPrimitiveMapper<std::string_view>::deserialize(std::optional<std::string_view>& value, std::istream& input) = delete;

template<>
void
JSONPrimitiveMapper<std::string_view>::deserialize(std::string_view* value, std::istream& input) = delete;

template<>
void
JSONPrimitiveMapper<std::string_view>::deserialize(std::vector<std::string_view>& value, std::istream& input) = delete;

template<>
void
JSONPrimitiveMapper<std::string_view>::deserialize(std::list<std::string_view>& value, std::istream& input) = delete;

template<>
void
JSONPrimitiveMapper<std::string_view>::deserialize(std::set<std::string_view>& value, std::istream& input) = delete;

template class JSONPrimitiveMapper<std::string_view>;
// JSONPrimitiveMapper specialization for std::string_view -

// JSONPrimitiveMapper specialization for anch::UUID +
#ifdef ANCH_UUID
#endif
// JSONPrimitiveMapper specialization for anch::UUID -

// JSONPrimitiveMapper specialization for anch::date::Date +
#ifdef ANCH_DATE
#endif
// JSONPrimitiveMapper specialization for anch::date::Date -
