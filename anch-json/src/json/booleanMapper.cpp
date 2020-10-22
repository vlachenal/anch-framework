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

#include "json/mappingFunctions.hpp"

using anch::json::PrimitiveMapper;

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
  int nbChars = 0;
  while(input && nbChars < 5) { // maximum 5 characters for 'false'
    current = input.peek();
    if(!isNumericChar(current)) {
      break;
    }
    input.get();
    buffer << static_cast<char>(current);
    ++nbChars;
  }
  std::string strVal = buffer.str();
  if(strVal == "true") {
    value = true;
  } else if(strVal == "false") {
    value = false;
  } else {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, std::optional<std::string>(strVal));
  }
}

// PrimitiveMapper specialization for bool +
template<>
PrimitiveMapper<bool>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<bool>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<bool>::serialize(const bool& value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize<bool>(value, out, &serializeValue, field);
}

template<>
bool
PrimitiveMapper<bool>::serialize(const bool* const value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize<bool>(value, out, &serializeValue, field);
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::optional<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize<bool>(value, out, &serializeValue, field);
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::vector<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, field);
  return true;
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::list<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, field);
  return true;
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::set<bool>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, field);
  return true;
}

template<>
void
PrimitiveMapper<bool>::deserialize(bool& value, std::istream& input) {
  anch::json::deserialize<bool>(value, input, &deserializeValue);
}

template<>
void
PrimitiveMapper<bool>::deserialize(std::optional<bool>& value, std::istream& input) {
  anch::json::deserialize<bool>(value, input, &deserializeValue);
}

template<>
void
PrimitiveMapper<bool>::deserialize(bool* value, std::istream& input) {
  anch::json::deserialize<bool>(value, input, &deserializeValue);
}

template<>
void
PrimitiveMapper<bool>::deserialize(std::vector<bool>& value, std::istream& input) {
  anch::json::deserializeArray<bool>(input,
				     [&value](const bool& str) -> void { value.push_back(str); },
				     &deserializeValue);
}

template<>
void
PrimitiveMapper<bool>::deserialize(std::list<bool>& value, std::istream& input) {
  anch::json::deserializeArray<bool>(input,
				     [&value](const bool& str) -> void { value.push_back(str); },
				     &deserializeValue);
}

template<>
void
PrimitiveMapper<bool>::deserialize(std::set<bool>& value, std::istream& input) {
  anch::json::deserializeArray<bool>(input,
				     [&value](const bool& str) -> void { value.insert(str); },
				     &deserializeValue);
}

template class PrimitiveMapper<bool>;
