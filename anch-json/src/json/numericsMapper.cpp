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
#include <functional>

#include "json/parser.hpp"
#include "convert.hpp"


using anch::json::JSONPrimitiveMapper;

template<typename T>
bool
serializeNumericArray(const T& array, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << anch::json::ARRAY_BEGIN;
  for(auto iter = array.begin() ; iter != array.end() ; ++iter) {
    if(iter != array.begin()) {
      out << anch::json::FIELD_SEPARATOR;
    }
    out << *iter;
  }
  out << anch::json::ARRAY_END;
  return true;
}

inline
bool
isNumericChar(int car) {
  return car == '0'
    || car == '1'
    || car == '2'
    || car == '3'
    || car == '4'
    || car == '5'
    || car == '6'
    || car == '7'
    || car == '8'
    || car == '9'
    || car == '-'
    || car == '.'
    || car == 'a'
    || car == 'A'
    || car == 'b'
    || car == 'B'
    || car == 'c'
    || car == 'C'
    || car == 'd'
    || car == 'D'
    || car == 'e' // for hexa or exponential
    || car == 'E' // for hexa or exponential
    || car == 'f'
    || car == 'F'
    || car == 'x' // for hexadecimal format
    ;
}

template<typename T>
void
deserializeNonNull(T& value, std::istream& input) {
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
  value = anch::convert<T>(buffer.str());
}

// JSONPrimitiveMapper specialization for uint64_t +
template<>
JSONPrimitiveMapper<uint64_t>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<uint64_t>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<uint64_t>::serialize(const uint64_t& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<uint64_t>::serialize(const uint64_t* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint64_t>::serialize(const std::optional<uint64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<uint64_t>::serialize(const std::vector<uint64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint64_t>::serialize(const std::list<uint64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint64_t>::serialize(const std::set<uint64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<uint64_t>::deserialize(uint64_t& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<uint64_t>::deserialize(std::optional<uint64_t>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    uint64_t parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<uint64_t>::deserialize(uint64_t* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new uint64_t();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<uint64_t>::deserialize(std::vector<uint64_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint64_t>(input,
					 [&value](const uint64_t& str) -> void { value.push_back(str); },
					 std::function<void(uint64_t&,std::istream&)>(deserializeNonNull<uint64_t>));
}

template<>
void
JSONPrimitiveMapper<uint64_t>::deserialize(std::list<uint64_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint64_t>(input,
					 [&value](const uint64_t& str) -> void { value.push_back(str); },
					 std::function<void(uint64_t&,std::istream&)>(deserializeNonNull<uint64_t>));
}

template<>
void
JSONPrimitiveMapper<uint64_t>::deserialize(std::set<uint64_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint64_t>(input,
					 [&value](const uint64_t& str) -> void { value.insert(str); },
					 std::function<void(uint64_t&,std::istream&)>(deserializeNonNull<uint64_t>));
}

template class JSONPrimitiveMapper<uint64_t>;
// JSONPrimitiveMapper specialization for uint64_t -

// JSONPrimitiveMapper specialization for int64_t +
template<>
JSONPrimitiveMapper<int64_t>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<int64_t>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<int64_t>::serialize(const int64_t& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<int64_t>::serialize(const int64_t* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int64_t>::serialize(const std::optional<int64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<int64_t>::serialize(const std::vector<int64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int64_t>::serialize(const std::list<int64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int64_t>::serialize(const std::set<int64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<int64_t>::deserialize(int64_t& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<int64_t>::deserialize(std::optional<int64_t>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    int64_t parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<int64_t>::deserialize(int64_t* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new int64_t();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<int64_t>::deserialize(std::vector<int64_t>& value, std::istream& input) {
  anch::json::deserializeArray<int64_t>(input,
					[&value](const int64_t& str) -> void { value.push_back(str); },
					std::function<void(int64_t&,std::istream&)>(deserializeNonNull<int64_t>));
}

template<>
void
JSONPrimitiveMapper<int64_t>::deserialize(std::list<int64_t>& value, std::istream& input) {
  anch::json::deserializeArray<int64_t>(input,
					[&value](const int64_t& str) -> void { value.push_back(str); },
					std::function<void(int64_t&,std::istream&)>(deserializeNonNull<int64_t>));
}

template<>
void
JSONPrimitiveMapper<int64_t>::deserialize(std::set<int64_t>& value, std::istream& input) {
  anch::json::deserializeArray<int64_t>(input,
					[&value](const int64_t& str) -> void { value.insert(str); },
					std::function<void(int64_t&,std::istream&)>(deserializeNonNull<int64_t>));
}

template class JSONPrimitiveMapper<int64_t>;
// JSONPrimitiveMapper specialization for int64_t -

// JSONPrimitiveMapper specialization for uint32_t +
template<>
JSONPrimitiveMapper<uint32_t>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<uint32_t>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<uint32_t>::serialize(const uint32_t& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<uint32_t>::serialize(const uint32_t* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint32_t>::serialize(const std::optional<uint32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<uint32_t>::serialize(const std::vector<uint32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint32_t>::serialize(const std::list<uint32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint32_t>::serialize(const std::set<uint32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<uint32_t>::deserialize(uint32_t& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<uint32_t>::deserialize(std::optional<uint32_t>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    uint32_t parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<uint32_t>::deserialize(uint32_t* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new uint32_t();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<uint32_t>::deserialize(std::vector<uint32_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint32_t>(input,
					 [&value](const uint32_t& str) -> void { value.push_back(str); },
					 std::function<void(uint32_t&,std::istream&)>(deserializeNonNull<uint32_t>));
}

template<>
void
JSONPrimitiveMapper<uint32_t>::deserialize(std::list<uint32_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint32_t>(input,
					 [&value](const uint32_t& str) -> void { value.push_back(str); },
					 std::function<void(uint32_t&,std::istream&)>(deserializeNonNull<uint32_t>));
}

template<>
void
JSONPrimitiveMapper<uint32_t>::deserialize(std::set<uint32_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint32_t>(input,
					 [&value](const uint32_t& str) -> void { value.insert(str); },
					 std::function<void(uint32_t&,std::istream&)>(deserializeNonNull<uint32_t>));
}

template class JSONPrimitiveMapper<uint32_t>;
// JSONPrimitiveMapper specialization for uint32_t -

// JSONPrimitiveMapper specialization for int32_t +
template<>
JSONPrimitiveMapper<int32_t>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<int32_t>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<int32_t>::serialize(const int32_t& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<int32_t>::serialize(const int32_t* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int32_t>::serialize(const std::optional<int32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<int32_t>::serialize(const std::vector<int32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int32_t>::serialize(const std::list<int32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int32_t>::serialize(const std::set<int32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<int32_t>::deserialize(int32_t& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<int32_t>::deserialize(std::optional<int32_t>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    int32_t parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<int32_t>::deserialize(int32_t* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new int32_t();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<int32_t>::deserialize(std::vector<int32_t>& value, std::istream& input) {
  anch::json::deserializeArray<int32_t>(input,
					[&value](const int32_t& str) -> void { value.push_back(str); },
					std::function<void(int32_t&,std::istream&)>(deserializeNonNull<int32_t>));
}

template<>
void
JSONPrimitiveMapper<int32_t>::deserialize(std::list<int32_t>& value, std::istream& input) {
  anch::json::deserializeArray<int32_t>(input,
					[&value](const int32_t& str) -> void { value.push_back(str); },
					std::function<void(int32_t&,std::istream&)>(deserializeNonNull<int32_t>));
}

template<>
void
JSONPrimitiveMapper<int32_t>::deserialize(std::set<int32_t>& value, std::istream& input) {
  anch::json::deserializeArray<int32_t>(input,
					[&value](const int32_t& str) -> void { value.insert(str); },
					std::function<void(int32_t&,std::istream&)>(deserializeNonNull<int32_t>));
}

template class JSONPrimitiveMapper<int32_t>;
// JSONPrimitiveMapper specialization for int32_t -

// JSONPrimitiveMapper specialization for uint16_t +
template<>
JSONPrimitiveMapper<uint16_t>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<uint16_t>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<uint16_t>::serialize(const uint16_t& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<uint16_t>::serialize(const uint16_t* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint16_t>::serialize(const std::optional<uint16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<uint16_t>::serialize(const std::vector<uint16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint16_t>::serialize(const std::list<uint16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint16_t>::serialize(const std::set<uint16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<uint16_t>::deserialize(uint16_t& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<uint16_t>::deserialize(std::optional<uint16_t>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    uint16_t parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<uint16_t>::deserialize(uint16_t* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new uint16_t();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<uint16_t>::deserialize(std::vector<uint16_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint16_t>(input,
					 [&value](const uint16_t& str) -> void { value.push_back(str); },
					 std::function<void(uint16_t&,std::istream&)>(deserializeNonNull<uint16_t>));
}

template<>
void
JSONPrimitiveMapper<uint16_t>::deserialize(std::list<uint16_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint16_t>(input,
					 [&value](const uint16_t& str) -> void { value.push_back(str); },
					 std::function<void(uint16_t&,std::istream&)>(deserializeNonNull<uint16_t>));
}

template<>
void
JSONPrimitiveMapper<uint16_t>::deserialize(std::set<uint16_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint16_t>(input,
					 [&value](const uint16_t& str) -> void { value.insert(str); },
					 std::function<void(uint16_t&,std::istream&)>(deserializeNonNull<uint16_t>));
}

template class JSONPrimitiveMapper<uint16_t>;
// JSONPrimitiveMapper specialization for uint16_t -

// JSONPrimitiveMapper specialization for int16_t +
template<>
JSONPrimitiveMapper<int16_t>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<int16_t>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<int16_t>::serialize(const int16_t& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<int16_t>::serialize(const int16_t* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int16_t>::serialize(const std::optional<int16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<int16_t>::serialize(const std::vector<int16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int16_t>::serialize(const std::list<int16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int16_t>::serialize(const std::set<int16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<int16_t>::deserialize(int16_t& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<int16_t>::deserialize(std::optional<int16_t>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    int16_t parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<int16_t>::deserialize(int16_t* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new int16_t();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<int16_t>::deserialize(std::vector<int16_t>& value, std::istream& input) {
  anch::json::deserializeArray<int16_t>(input,
					[&value](const int16_t& str) -> void { value.push_back(str); },
					std::function<void(int16_t&,std::istream&)>(deserializeNonNull<int16_t>));
}

template<>
void
JSONPrimitiveMapper<int16_t>::deserialize(std::list<int16_t>& value, std::istream& input) {
  anch::json::deserializeArray<int16_t>(input,
					[&value](const int16_t& str) -> void { value.push_back(str); },
					std::function<void(int16_t&,std::istream&)>(deserializeNonNull<int16_t>));
}

template<>
void
JSONPrimitiveMapper<int16_t>::deserialize(std::set<int16_t>& value, std::istream& input) {
  anch::json::deserializeArray<int16_t>(input,
					[&value](const int16_t& str) -> void { value.insert(str); },
					std::function<void(int16_t&,std::istream&)>(deserializeNonNull<int16_t>));
}

template class JSONPrimitiveMapper<int16_t>;
// JSONPrimitiveMapper specialization for int16_t -

// JSONPrimitiveMapper specialization for uint8_t +
template<>
JSONPrimitiveMapper<uint8_t>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<uint8_t>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<uint8_t>::serialize(const uint8_t& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<uint8_t>::serialize(const uint8_t* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint8_t>::serialize(const std::optional<uint8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<uint8_t>::serialize(const std::vector<uint8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint8_t>::serialize(const std::list<uint8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<uint8_t>::serialize(const std::set<uint8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<uint8_t>::deserialize(uint8_t& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<uint8_t>::deserialize(std::optional<uint8_t>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    uint8_t parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<uint8_t>::deserialize(uint8_t* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new uint8_t();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<uint8_t>::deserialize(std::vector<uint8_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint8_t>(input,
					[&value](const uint8_t& str) -> void { value.push_back(str); },
					std::function<void(uint8_t&,std::istream&)>(deserializeNonNull<uint8_t>));
}

template<>
void
JSONPrimitiveMapper<uint8_t>::deserialize(std::list<uint8_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint8_t>(input,
					[&value](const uint8_t& str) -> void { value.push_back(str); },
					std::function<void(uint8_t&,std::istream&)>(deserializeNonNull<uint8_t>));
}

template<>
void
JSONPrimitiveMapper<uint8_t>::deserialize(std::set<uint8_t>& value, std::istream& input) {
  anch::json::deserializeArray<uint8_t>(input,
					[&value](const uint8_t& str) -> void { value.insert(str); },
					std::function<void(uint8_t&,std::istream&)>(deserializeNonNull<uint8_t>));
}

template class JSONPrimitiveMapper<uint8_t>;
// JSONPrimitiveMapper specialization for uint8_t -

// JSONPrimitiveMapper specialization for int8_t +
template<>
JSONPrimitiveMapper<int8_t>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<int8_t>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<int8_t>::serialize(const int8_t& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<int8_t>::serialize(const int8_t* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int8_t>::serialize(const std::optional<int8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<int8_t>::serialize(const std::vector<int8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int8_t>::serialize(const std::list<int8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<int8_t>::serialize(const std::set<int8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<int8_t>::deserialize(int8_t& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<int8_t>::deserialize(std::optional<int8_t>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    int8_t parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<int8_t>::deserialize(int8_t* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new int8_t();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<int8_t>::deserialize(std::vector<int8_t>& value, std::istream& input) {
  anch::json::deserializeArray<int8_t>(input,
				       [&value](const int8_t& str) -> void { value.push_back(str); },
				       std::function<void(int8_t&,std::istream&)>(deserializeNonNull<int8_t>));
}

template<>
void
JSONPrimitiveMapper<int8_t>::deserialize(std::list<int8_t>& value, std::istream& input) {
  anch::json::deserializeArray<int8_t>(input,
				       [&value](const int8_t& str) -> void { value.push_back(str); },
				       std::function<void(int8_t&,std::istream&)>(deserializeNonNull<int8_t>));
}

template<>
void
JSONPrimitiveMapper<int8_t>::deserialize(std::set<int8_t>& value, std::istream& input) {
  anch::json::deserializeArray<int8_t>(input,
				       [&value](const int8_t& str) -> void { value.insert(str); },
				       std::function<void(int8_t&,std::istream&)>(deserializeNonNull<int8_t>));
}

template class JSONPrimitiveMapper<int8_t>;
// JSONPrimitiveMapper specialization for int8_t -

// JSONPrimitiveMapper specialization for float +
template<>
JSONPrimitiveMapper<float>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<float>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<float>::serialize(const float& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<float>::serialize(const float* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<float>::serialize(const std::optional<float>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<float>::serialize(const std::vector<float>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<float>::serialize(const std::list<float>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<float>::serialize(const std::set<float>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<float>::deserialize(float& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<float>::deserialize(std::optional<float>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    float parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<float>::deserialize(float* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new float();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<float>::deserialize(std::vector<float>& value, std::istream& input) {
  anch::json::deserializeArray<float>(input,
				      [&value](const float& str) -> void { value.push_back(str); },
				      std::function<void(float&,std::istream&)>(deserializeNonNull<float>));
}

template<>
void
JSONPrimitiveMapper<float>::deserialize(std::list<float>& value, std::istream& input) {
  anch::json::deserializeArray<float>(input,
				      [&value](const float& str) -> void { value.push_back(str); },
				      std::function<void(float&,std::istream&)>(deserializeNonNull<float>));
}

template<>
void
JSONPrimitiveMapper<float>::deserialize(std::set<float>& value, std::istream& input) {
  anch::json::deserializeArray<float>(input,
				      [&value](const float& str) -> void { value.insert(str); },
				      std::function<void(float&,std::istream&)>(deserializeNonNull<float>));
}

template class JSONPrimitiveMapper<float>;
// JSONPrimitiveMapper specialization for float -

// JSONPrimitiveMapper specialization for double +
template<>
JSONPrimitiveMapper<double>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<double>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<double>::serialize(const double& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<double>::serialize(const double* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<double>::serialize(const std::optional<double>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<double>::serialize(const std::vector<double>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<double>::serialize(const std::list<double>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<double>::serialize(const std::set<double>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<double>::deserialize(double& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<double>::deserialize(std::optional<double>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    double parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<double>::deserialize(double* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new double();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<double>::deserialize(std::vector<double>& value, std::istream& input) {
  anch::json::deserializeArray<double>(input,
				       [&value](const double& str) -> void { value.push_back(str); },
				       std::function<void(double&,std::istream&)>(deserializeNonNull<double>));
}

template<>
void
JSONPrimitiveMapper<double>::deserialize(std::list<double>& value, std::istream& input) {
  anch::json::deserializeArray<double>(input,
				       [&value](const double& str) -> void { value.push_back(str); },
				       std::function<void(double&,std::istream&)>(deserializeNonNull<double>));
}

template<>
void
JSONPrimitiveMapper<double>::deserialize(std::set<double>& value, std::istream& input) {
  anch::json::deserializeArray<double>(input,
				       [&value](const double& str) -> void { value.insert(str); },
				       std::function<void(double&,std::istream&)>(deserializeNonNull<double>));
}

template class JSONPrimitiveMapper<double>;
// JSONPrimitiveMapper specialization for double -

// JSONPrimitiveMapper specialization for long double +
template<>
JSONPrimitiveMapper<long double>::JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
JSONPrimitiveMapper<long double>::~JSONPrimitiveMapper() {
  // Nothing to do
}

template<>
bool
JSONPrimitiveMapper<long double>::serialize(const long double& value, std::ostream& out, const std::optional<std::string>& field) {
  if(field.has_value()) {
    out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
  }
  out << value;
  return true;
}

template<>
bool
JSONPrimitiveMapper<long double>::serialize(const long double* const value, std::ostream& out, const std::optional<std::string>& field) {
  if(value == NULL) {
    return false;
  }
  return this->serialize(*value, out, field);
}

template<>
bool
JSONPrimitiveMapper<long double>::serialize(const std::optional<long double>& value, std::ostream& out, const std::optional<std::string>& field) {
  if(!value.has_value()) {
    return false;
  }
  return this->serialize(value.value(), out, field);
}

template<>
bool
JSONPrimitiveMapper<long double>::serialize(const std::vector<long double>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<long double>::serialize(const std::list<long double>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
bool
JSONPrimitiveMapper<long double>::serialize(const std::set<long double>& value, std::ostream& out, const std::optional<std::string>& field) {
  return serializeNumericArray(value, out, field);
}

template<>
void
JSONPrimitiveMapper<long double>::deserialize(long double& value, std::istream& input) {
  if(!anch::json::isNull(input)) {
    deserializeNonNull(value, input);
  }
}

template<>
void
JSONPrimitiveMapper<long double>::deserialize(std::optional<long double>& value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value.reset();
  } else {
    long double parsed;
    deserializeNonNull(parsed, input);
    value = std::move(parsed);
  }
}

template<>
void
JSONPrimitiveMapper<long double>::deserialize(long double* value, std::istream& input) {
  if(anch::json::isNull(input)) {
    value = NULL;
  } else {
    value = new long double();
    deserializeNonNull(*value, input);
  }
}

template<>
void
JSONPrimitiveMapper<long double>::deserialize(std::vector<long double>& value, std::istream& input) {
  anch::json::deserializeArray<long double>(input,
					    [&value](const long double& str) -> void { value.push_back(str); },
					    std::function<void(long double&,std::istream&)>(deserializeNonNull<long double>));
}

template<>
void
JSONPrimitiveMapper<long double>::deserialize(std::list<long double>& value, std::istream& input) {
  anch::json::deserializeArray<long double>(input,
					    [&value](const long double& str) -> void { value.push_back(str); },
					    std::function<void(long double&,std::istream&)>(deserializeNonNull<long double>));
}

template<>
void
JSONPrimitiveMapper<long double>::deserialize(std::set<long double>& value, std::istream& input) {
  anch::json::deserializeArray<long double>(input,
					    [&value](const long double& str) -> void { value.insert(str); },
					    std::function<void(long double&,std::istream&)>(deserializeNonNull<long double>));
}

template class JSONPrimitiveMapper<long double>;
// JSONPrimitiveMapper specialization for long double -
