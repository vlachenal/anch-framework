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

#include "json/constants.hpp"
#include "json/mappingFunctions.hpp"
#include "json/mappingError.hpp"

#ifdef ANCH_UUID
#include "uuid.hpp"
#endif

#ifdef ANCH_DATE
#include "date/date.hpp"
#endif


using anch::json::PrimitiveMapper;

template<typename T>
inline
void
serializeValue(const T& value, std::ostream& out) {
  out << anch::json::STRING_DELIMITER << value << anch::json::STRING_DELIMITER;
}

void
deserializeValue(std::string& value, std::istream& input, [[maybe_unused]] const anch::json::MappingOptions& options) {
  // Look for '"'
  int current = input.get();
  if(current != anch::json::STRING_DELIMITER) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
  }
  std::ostringstream buffer;
  while(input) {
    // \todo max char in value ...
    current = input.get();
    if(current == '\\') {
      if(!input) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input);
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
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
  }
  value = buffer.str();
}

// PrimitiveMapper specialization for std::string +
template<>
PrimitiveMapper<std::string>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<std::string>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::string& value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<std::string>, field);
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::string* const value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<std::string>, field);
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::optional<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<std::string>, field);
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::vector<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string>(value, out, &serializeValue<std::string>, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::list<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string>(value, out, &serializeValue<std::string>, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::set<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string>(value, out, &serializeValue<std::string>, field);
  return true;
}

template<>
void
PrimitiveMapper<std::string>::deserialize(std::string& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<std::string>(value, input, options, &deserializeValue);
}

template<>
void
PrimitiveMapper<std::string>::deserialize(std::optional<std::string>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<std::string>(value, input, options, &deserializeValue);
}

template<>
void
PrimitiveMapper<std::string>::deserialize(std::string* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<std::string>(value, input, options, &deserializeValue);
}

template<>
void
PrimitiveMapper<std::string>::deserialize(std::vector<std::string>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<std::string>(input,
					    [&value](const std::string& str) -> void { value.push_back(str); },
					    options,
					    &deserializeValue);
}

template<>
void
PrimitiveMapper<std::string>::deserialize(std::list<std::string>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<std::string>(input,
					    [&value](const std::string& str) -> void { value.push_back(str); },
					    options,
					    &deserializeValue);
}

template<>
void
PrimitiveMapper<std::string>::deserialize(std::set<std::string>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<std::string>(input,
					    [&value](const std::string& str) -> void { value.insert(str); },
					    options,
					    &deserializeValue);
}

template class PrimitiveMapper<std::string>;
// PrimitiveMapper specialization for std::string -

// PrimitiveMapper specialization for std::string_view +
template<>
PrimitiveMapper<std::string_view>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<std::string_view>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::string_view& value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<std::string_view>, field);
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::string_view* const value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<std::string_view>, field);
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::optional<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<std::string_view>, field);
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::vector<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string_view>(value, out, &serializeValue<std::string_view>, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::list<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string_view>(value, out, &serializeValue<std::string_view>, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::set<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string_view>(value, out, &serializeValue<std::string_view>, field);
  return true;
}

template<>
void
PrimitiveMapper<std::string_view>::deserialize(std::string_view& value, std::istream& input, const anch::json::MappingOptions& options) = delete;

template<>
void
PrimitiveMapper<std::string_view>::deserialize(std::optional<std::string_view>& value, std::istream& input, const anch::json::MappingOptions& options) = delete;

template<>
void
PrimitiveMapper<std::string_view>::deserialize(std::string_view* value, std::istream& input, const anch::json::MappingOptions& options) = delete;

template<>
void
PrimitiveMapper<std::string_view>::deserialize(std::vector<std::string_view>& value, std::istream& input, const anch::json::MappingOptions& options) = delete;

template<>
void
PrimitiveMapper<std::string_view>::deserialize(std::list<std::string_view>& value, std::istream& input, const anch::json::MappingOptions& options) = delete;

template<>
void
PrimitiveMapper<std::string_view>::deserialize(std::set<std::string_view>& value, std::istream& input, const anch::json::MappingOptions& options) = delete;

template class PrimitiveMapper<std::string_view>;
// PrimitiveMapper specialization for std::string_view -

// PrimitiveMapper specialization for anch::UUID +
#ifdef ANCH_UUID
#endif
// PrimitiveMapper specialization for anch::UUID -

// PrimitiveMapper specialization for anch::date::Date +
#ifdef ANCH_DATE
#endif
// PrimitiveMapper specialization for anch::date::Date -
