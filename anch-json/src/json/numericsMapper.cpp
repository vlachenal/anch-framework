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
#include <cstdint>

#include "json/mappingFunctions.hpp"
#include "convert.hpp"


using anch::json::PrimitiveMapper;

template<typename T>
inline
void
serializeValue(const T& value, std::ostream& out, [[maybe_unused]] const anch::json::MappingOptions& options) {
  out << value;
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
    || car == 'a' // for hexadecimal format
    || car == 'A' // for hexadecimal format
    || car == 'b' // for hexadecimal format
    || car == 'B' // for hexadecimal format
    || car == 'c' // for hexadecimal format
    || car == 'C' // for hexadecimal format
    || car == 'd' // for hexadecimal format
    || car == 'D' // for hexadecimal format
    || car == 'e' // for hexadecimal format or exponential
    || car == 'E' // for hexadecimal format or exponential
    || car == 'f' // for hexadecimal format
    || car == 'F' // for hexadecimal format
    || car == 'x' // for hexadecimal format
    ;
}

template<typename T>
void
deserializeValue(T& value, std::istream& input, [[maybe_unused]] const anch::json::MappingOptions& options) {
  std::ostringstream buffer;
  int current;
  while(input) {
    // \todo max char in value ... 20
    current = input.peek();
    if(!isNumericChar(current)) {
      break;
    }
    input.get();
    buffer << static_cast<char>(current);
  }
  value = anch::convert<T>(buffer.str());
}

// PrimitiveMapper specialization for uint64_t +
template<>
PrimitiveMapper<uint64_t>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<uint64_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const uint64_t& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint64_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const uint64_t* const value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint64_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::optional<uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint64_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::vector<uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint64_t>(value, out, &serializeValue<uint64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::list<uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint64_t>(value, out, &serializeValue<uint64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::set<uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint64_t>(value, out, &serializeValue<uint64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::map<std::string,uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeMap<uint64_t>(value, out, &serializeValue<uint64_t>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<uint64_t>::deserialize(uint64_t& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint64_t>(value, input, options, &deserializeValue<uint64_t>);
}

template<>
void
PrimitiveMapper<uint64_t>::deserialize(std::optional<uint64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint64_t>(value, input, options, &deserializeValue<uint64_t>);
}

template<>
void
PrimitiveMapper<uint64_t>::deserialize(uint64_t* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint64_t>(value, input, options, &deserializeValue<uint64_t>);
}

template<>
void
PrimitiveMapper<uint64_t>::deserialize(std::vector<uint64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint64_t>(input,
					 [&value](const uint64_t& str) -> void { value.push_back(str); },
					 options,
					 &deserializeValue<uint64_t>);
}

template<>
void
PrimitiveMapper<uint64_t>::deserialize(std::list<uint64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint64_t>(input,
					 [&value](const uint64_t& str) -> void { value.push_back(str); },
					 options,
					 &deserializeValue<uint64_t>);
}

template<>
void
PrimitiveMapper<uint64_t>::deserialize(std::set<uint64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint64_t>(input,
					 [&value](const uint64_t& str) -> void { value.insert(str); },
					 options,
					 &deserializeValue<uint64_t>);
}

template<>
void
PrimitiveMapper<uint64_t>::deserialize(std::map<std::string,uint64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<uint64_t>(input,
				       [&value](const std::pair<std::string,uint64_t>& str) -> void { value.insert(str); },
				       options,
				       &deserializeValue<uint64_t>);
}

template class PrimitiveMapper<uint64_t>;
// PrimitiveMapper specialization for uint64_t -

// PrimitiveMapper specialization for int64_t +
template<>
PrimitiveMapper<int64_t>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<int64_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<int64_t>::serialize(const int64_t& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int64_t>, options, field);
}

template<>
bool
PrimitiveMapper<int64_t>::serialize(const int64_t* const value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int64_t>, options, field);
}

template<>
bool
PrimitiveMapper<int64_t>::serialize(const std::optional<int64_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int64_t>, options, field);
}

template<>
bool
PrimitiveMapper<int64_t>::serialize(const std::vector<int64_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int64_t>(value, out, &serializeValue<int64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int64_t>::serialize(const std::list<int64_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int64_t>(value, out, &serializeValue<int64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int64_t>::serialize(const std::set<int64_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int64_t>(value, out, &serializeValue<int64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int64_t>::serialize(const std::map<std::string,int64_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeMap<int64_t>(value, out, &serializeValue<int64_t>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<int64_t>::deserialize(int64_t& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int64_t>(value, input, options, &deserializeValue<int64_t>);
}

template<>
void
PrimitiveMapper<int64_t>::deserialize(std::optional<int64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int64_t>(value, input, options, &deserializeValue<int64_t>);
}

template<>
void
PrimitiveMapper<int64_t>::deserialize(int64_t* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int64_t>(value, input, options, &deserializeValue<int64_t>);
}

template<>
void
PrimitiveMapper<int64_t>::deserialize(std::vector<int64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int64_t>(input,
					[&value](const int64_t& str) -> void { value.push_back(str); },
					options,
					&deserializeValue<int64_t>);
}

template<>
void
PrimitiveMapper<int64_t>::deserialize(std::list<int64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int64_t>(input,
					[&value](const int64_t& str) -> void { value.push_back(str); },
					options,
					&deserializeValue<int64_t>);
}

template<>
void
PrimitiveMapper<int64_t>::deserialize(std::set<int64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int64_t>(input,
					[&value](const int64_t& str) -> void { value.insert(str); },
					options,
					&deserializeValue<int64_t>);
}

template<>
void
PrimitiveMapper<int64_t>::deserialize(std::map<std::string,int64_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<int64_t>(input,
				      [&value](const std::pair<std::string,int64_t>& str) -> void { value.insert(str); },
				      options,
				      &deserializeValue<int64_t>);
}

template class PrimitiveMapper<int64_t>;
// PrimitiveMapper specialization for int64_t -

// PrimitiveMapper specialization for uint32_t +
template<>
PrimitiveMapper<uint32_t>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<uint32_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<uint32_t>::serialize(const uint32_t& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint32_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint32_t>::serialize(const uint32_t* const value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint32_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint32_t>::serialize(const std::optional<uint32_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint32_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint32_t>::serialize(const std::vector<uint32_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint32_t>(value, out, &serializeValue<uint32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint32_t>::serialize(const std::list<uint32_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint32_t>(value, out, &serializeValue<uint32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint32_t>::serialize(const std::set<uint32_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint32_t>(value, out, &serializeValue<uint32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint32_t>::serialize(const std::map<std::string,uint32_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeMap<uint32_t>(value, out, &serializeValue<uint32_t>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<uint32_t>::deserialize(uint32_t& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint32_t>(value, input, options, &deserializeValue<uint32_t>);
}

template<>
void
PrimitiveMapper<uint32_t>::deserialize(std::optional<uint32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint32_t>(value, input, options, &deserializeValue<uint32_t>);
}

template<>
void
PrimitiveMapper<uint32_t>::deserialize(uint32_t* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint32_t>(value, input, options, &deserializeValue<uint32_t>);
}

template<>
void
PrimitiveMapper<uint32_t>::deserialize(std::vector<uint32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint32_t>(input,
					 [&value](const uint32_t& str) -> void { value.push_back(str); },
					 options,
					 &deserializeValue<uint32_t>);
}

template<>
void
PrimitiveMapper<uint32_t>::deserialize(std::list<uint32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint32_t>(input,
					 [&value](const uint32_t& str) -> void { value.push_back(str); },
					 options,
					 &deserializeValue<uint32_t>);
}

template<>
void
PrimitiveMapper<uint32_t>::deserialize(std::set<uint32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint32_t>(input,
					 [&value](const uint32_t& str) -> void { value.insert(str); },
					 options,
					 &deserializeValue<uint32_t>);
}

template<>
void
PrimitiveMapper<uint32_t>::deserialize(std::map<std::string,uint32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<uint32_t>(input,
				       [&value](const std::pair<std::string,uint32_t>& str) -> void { value.insert(str); },
				       options,
				       &deserializeValue<uint32_t>);
}

template class PrimitiveMapper<uint32_t>;
// PrimitiveMapper specialization for uint32_t -

// PrimitiveMapper specialization for int32_t +
template<>
PrimitiveMapper<int32_t>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<int32_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const int32_t& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int32_t>, options, field);
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const int32_t* const value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int32_t>, options, field);
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::optional<int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int32_t>, options, field);
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::vector<int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int32_t>(value, out, &serializeValue<int32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::list<int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int32_t>(value, out, &serializeValue<int32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::set<int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int32_t>(value, out, &serializeValue<int32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::map<std::string,int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeMap<int32_t>(value, out, &serializeValue<int32_t>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<int32_t>::deserialize(int32_t& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int32_t>(value, input, options, &deserializeValue<int32_t>);
}

template<>
void
PrimitiveMapper<int32_t>::deserialize(std::optional<int32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int32_t>(value, input, options, &deserializeValue<int32_t>);
}

template<>
void
PrimitiveMapper<int32_t>::deserialize(int32_t* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int32_t>(value, input, options, &deserializeValue<int32_t>);
}

template<>
void
PrimitiveMapper<int32_t>::deserialize(std::vector<int32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int32_t>(input,
					[&value](const int32_t& str) -> void { value.push_back(str); },
					options,
					&deserializeValue<int32_t>);
}

template<>
void
PrimitiveMapper<int32_t>::deserialize(std::list<int32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int32_t>(input,
					[&value](const int32_t& str) -> void { value.push_back(str); },
					options,
					&deserializeValue<int32_t>);
}

template<>
void
PrimitiveMapper<int32_t>::deserialize(std::set<int32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int32_t>(input,
					[&value](const int32_t& str) -> void { value.insert(str); },
					options,
					&deserializeValue<int32_t>);
}

template<>
void
PrimitiveMapper<int32_t>::deserialize(std::map<std::string,int32_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<int32_t>(input,
				      [&value](const std::pair<std::string,int32_t>& str) -> void { value.insert(str); },
				      options,
				      &deserializeValue<int32_t>);
}

template class PrimitiveMapper<int32_t>;
// PrimitiveMapper specialization for int32_t -

// PrimitiveMapper specialization for uint16_t +
template<>
PrimitiveMapper<uint16_t>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<uint16_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const uint16_t& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint16_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const uint16_t* const value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint16_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::optional<uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint16_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::vector<uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint16_t>(value, out, &serializeValue<uint16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::list<uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint16_t>(value, out, &serializeValue<uint16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::set<uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint16_t>(value, out, &serializeValue<uint16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::map<std::string,uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeMap<uint16_t>(value, out, &serializeValue<uint16_t>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<uint16_t>::deserialize(uint16_t& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint16_t>(value, input, options, &deserializeValue<uint16_t>);
}

template<>
void
PrimitiveMapper<uint16_t>::deserialize(std::optional<uint16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint16_t>(value, input, options, &deserializeValue<uint16_t>);
}

template<>
void
PrimitiveMapper<uint16_t>::deserialize(uint16_t* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint16_t>(value, input, options, &deserializeValue<uint16_t>);
}

template<>
void
PrimitiveMapper<uint16_t>::deserialize(std::vector<uint16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint16_t>(input,
					 [&value](const uint16_t& str) -> void { value.push_back(str); },
					 options,
					 &deserializeValue<uint16_t>);
}

template<>
void
PrimitiveMapper<uint16_t>::deserialize(std::list<uint16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint16_t>(input,
					 [&value](const uint16_t& str) -> void { value.push_back(str); },
					 options,
					 &deserializeValue<uint16_t>);
}

template<>
void
PrimitiveMapper<uint16_t>::deserialize(std::set<uint16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint16_t>(input,
					 [&value](const uint16_t& str) -> void { value.insert(str); },
					 options,
					 &deserializeValue<uint16_t>);
}

template<>
void
PrimitiveMapper<uint16_t>::deserialize(std::map<std::string,uint16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<uint16_t>(input,
				       [&value](const std::pair<std::string,uint16_t>& str) -> void { value.insert(str); },
				       options,
				       &deserializeValue<uint16_t>);
}

template class PrimitiveMapper<uint16_t>;
// PrimitiveMapper specialization for uint16_t -

// PrimitiveMapper specialization for int16_t +
template<>
PrimitiveMapper<int16_t>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<int16_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<int16_t>::serialize(const int16_t& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int16_t>, options, field);
}

template<>
bool
PrimitiveMapper<int16_t>::serialize(const int16_t* const value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int16_t>, options, field);
}

template<>
bool
PrimitiveMapper<int16_t>::serialize(const std::optional<int16_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int16_t>, options, field);
}

template<>
bool
PrimitiveMapper<int16_t>::serialize(const std::vector<int16_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int16_t>(value, out, &serializeValue<int16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int16_t>::serialize(const std::list<int16_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int16_t>(value, out, &serializeValue<int16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int16_t>::serialize(const std::set<int16_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int16_t>(value, out, &serializeValue<int16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int16_t>::serialize(const std::map<std::string,int16_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeMap<int16_t>(value, out, &serializeValue<int16_t>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<int16_t>::deserialize(int16_t& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int16_t>(value, input, options, &deserializeValue<int16_t>);
}

template<>
void
PrimitiveMapper<int16_t>::deserialize(std::optional<int16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int16_t>(value, input, options, &deserializeValue<int16_t>);
}

template<>
void
PrimitiveMapper<int16_t>::deserialize(int16_t* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int16_t>(value, input, options, &deserializeValue<int16_t>);
}

template<>
void
PrimitiveMapper<int16_t>::deserialize(std::vector<int16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int16_t>(input,
					[&value](const int16_t& str) -> void { value.push_back(str); },
					options,
					&deserializeValue<int16_t>);
}

template<>
void
PrimitiveMapper<int16_t>::deserialize(std::list<int16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int16_t>(input,
					[&value](const int16_t& str) -> void { value.push_back(str); },
					options,
					&deserializeValue<int16_t>);
}

template<>
void
PrimitiveMapper<int16_t>::deserialize(std::set<int16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int16_t>(input,
					[&value](const int16_t& str) -> void { value.insert(str); },
					options,
					&deserializeValue<int16_t>);
}

template<>
void
PrimitiveMapper<int16_t>::deserialize(std::map<std::string,int16_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<int16_t>(input,
				      [&value](const std::pair<std::string,int16_t>& str) -> void { value.insert(str); },
				      options,
				      &deserializeValue<int16_t>);
}

template class PrimitiveMapper<int16_t>;
// PrimitiveMapper specialization for int16_t -

// PrimitiveMapper specialization for uint8_t +
template<>
PrimitiveMapper<uint8_t>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<uint8_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<uint8_t>::serialize(const uint8_t& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint8_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint8_t>::serialize(const uint8_t* const value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint8_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint8_t>::serialize(const std::optional<uint8_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<uint8_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint8_t>::serialize(const std::vector<uint8_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<uint8_t>(value, out, &serializeValue<uint8_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint8_t>::serialize(const std::list<uint8_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<uint8_t>(value, out, &serializeValue<uint8_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint8_t>::serialize(const std::set<uint8_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<uint8_t>(value, out, &serializeValue<uint8_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint8_t>::serialize(const std::map<std::string,uint8_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeMap<uint8_t>(value, out, &serializeValue<uint8_t>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<uint8_t>::deserialize(uint8_t& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint8_t>(value, input, options, &deserializeValue<uint8_t>);
}

template<>
void
PrimitiveMapper<uint8_t>::deserialize(std::optional<uint8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint8_t>(value, input, options, &deserializeValue<uint8_t>);
}

template<>
void
PrimitiveMapper<uint8_t>::deserialize(uint8_t* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<uint8_t>(value, input, options, &deserializeValue<uint8_t>);
}

template<>
void
PrimitiveMapper<uint8_t>::deserialize(std::vector<uint8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint8_t>(input,
					[&value](const uint8_t& str) -> void { value.push_back(str); },
					options,
					&deserializeValue<uint8_t>);
}

template<>
void
PrimitiveMapper<uint8_t>::deserialize(std::list<uint8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint8_t>(input,
					[&value](const uint8_t& str) -> void { value.push_back(str); },
					options,
					&deserializeValue<uint8_t>);
}

template<>
void
PrimitiveMapper<uint8_t>::deserialize(std::set<uint8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<uint8_t>(input,
					[&value](const uint8_t& str) -> void { value.insert(str); },
					options,
					&deserializeValue<uint8_t>);
}

template<>
void
PrimitiveMapper<uint8_t>::deserialize(std::map<std::string,uint8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<uint8_t>(input,
				      [&value](const std::pair<std::string,uint8_t>& str) -> void { value.insert(str); },
				      options,
				      &deserializeValue<uint8_t>);
}

template class PrimitiveMapper<uint8_t>;
// PrimitiveMapper specialization for uint8_t -

// PrimitiveMapper specialization for int8_t +
template<>
PrimitiveMapper<int8_t>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<int8_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<int8_t>::serialize(const int8_t& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int8_t>, options, field);
}

template<>
bool
PrimitiveMapper<int8_t>::serialize(const int8_t* const value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int8_t>, options, field);
}

template<>
bool
PrimitiveMapper<int8_t>::serialize(const std::optional<int8_t>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<int8_t>, options, field);
}

template<>
bool
PrimitiveMapper<int8_t>::serialize(const std::vector<int8_t>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<int8_t>(value, out, &serializeValue<int8_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int8_t>::serialize(const std::list<int8_t>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<int8_t>(value, out, &serializeValue<int8_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int8_t>::serialize(const std::set<int8_t>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<int8_t>(value, out, &serializeValue<int8_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int8_t>::serialize(const std::map<std::string,int8_t>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeMap<int8_t>(value, out, &serializeValue<int8_t>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<int8_t>::deserialize(int8_t& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int8_t>(value, input, options, &deserializeValue<int8_t>);
}

template<>
void
PrimitiveMapper<int8_t>::deserialize(std::optional<int8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int8_t>(value, input, options, &deserializeValue<int8_t>);
}

template<>
void
PrimitiveMapper<int8_t>::deserialize(int8_t* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<int8_t>(value, input, options, &deserializeValue<int8_t>);
}

template<>
void
PrimitiveMapper<int8_t>::deserialize(std::vector<int8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int8_t>(input,
				       [&value](const int8_t& str) -> void { value.push_back(str); },
				       options,
				       &deserializeValue<int8_t>);
}

template<>
void
PrimitiveMapper<int8_t>::deserialize(std::list<int8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int8_t>(input,
				       [&value](const int8_t& str) -> void { value.push_back(str); },
				       options,
				       &deserializeValue<int8_t>);
}

template<>
void
PrimitiveMapper<int8_t>::deserialize(std::set<int8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<int8_t>(input,
				       [&value](const int8_t& str) -> void { value.insert(str); },
				       options,
				       &deserializeValue<int8_t>);
}

template<>
void
PrimitiveMapper<int8_t>::deserialize(std::map<std::string,int8_t>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<int8_t>(input,
				     [&value](const std::pair<std::string,int8_t>& str) -> void { value.insert(str); },
				     options,
				     &deserializeValue<int8_t>);
}

template class PrimitiveMapper<int8_t>;
// PrimitiveMapper specialization for int8_t -

// PrimitiveMapper specialization for float +
template<>
PrimitiveMapper<float>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<float>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<float>::serialize(const float& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<float>, options, field);
}

template<>
bool
PrimitiveMapper<float>::serialize(const float* const value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<float>, options, field);
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::optional<float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<float>, options, field);
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::vector<float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  anch::json::serializeArray<float>(value, out, &serializeValue<float>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::list<float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  anch::json::serializeArray<float>(value, out, &serializeValue<float>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::set<float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  anch::json::serializeArray<float>(value, out, &serializeValue<float>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::map<std::string,float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  anch::json::serializeMap<float>(value, out, &serializeValue<float>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<float>::deserialize(float& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<float>(value, input, options, &deserializeValue<float>);
}

template<>
void
PrimitiveMapper<float>::deserialize(std::optional<float>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<float>(value, input, options, &deserializeValue<float>);
}

template<>
void
PrimitiveMapper<float>::deserialize(float* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<float>(value, input, options, &deserializeValue<float>);
}

template<>
void
PrimitiveMapper<float>::deserialize(std::vector<float>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<float>(input,
				      [&value](const float& str) -> void { value.push_back(str); },
				      options,
				      &deserializeValue<float>);
}

template<>
void
PrimitiveMapper<float>::deserialize(std::list<float>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<float>(input,
				      [&value](const float& str) -> void { value.push_back(str); },
				      options,
				      &deserializeValue<float>);
}

template<>
void
PrimitiveMapper<float>::deserialize(std::set<float>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<float>(input,
				      [&value](const float& str) -> void { value.insert(str); },
				      options,
				      &deserializeValue<float>);
}

template<>
void
PrimitiveMapper<float>::deserialize(std::map<std::string,float>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<float>(input,
				    [&value](const std::pair<std::string,float>& str) -> void { value.insert(str); },
				    options,
				    &deserializeValue<float>);
}

template class PrimitiveMapper<float>;
// PrimitiveMapper specialization for float -

// PrimitiveMapper specialization for double +
template<>
PrimitiveMapper<double>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<double>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<double>::serialize(const double& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<double>, options, field);
}

template<>
bool
PrimitiveMapper<double>::serialize(const double* const value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<double>, options, field);
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::optional<double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<double>, options, field);
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::vector<double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<double>(value, out, &serializeValue<double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::list<double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<double>(value, out, &serializeValue<double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::set<double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<double>(value, out, &serializeValue<double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::map<std::string,double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeMap<double>(value, out, &serializeValue<double>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<double>::deserialize(double& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<double>(value, input, options, &deserializeValue<double>);
}

template<>
void
PrimitiveMapper<double>::deserialize(std::optional<double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<double>(value, input, options, &deserializeValue<double>);
}

template<>
void
PrimitiveMapper<double>::deserialize(double* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<double>(value, input, options, &deserializeValue<double>);
}

template<>
void
PrimitiveMapper<double>::deserialize(std::vector<double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<double>(input,
				       [&value](const double& str) -> void { value.push_back(str); },
				       options,
				       &deserializeValue<double>);
}

template<>
void
PrimitiveMapper<double>::deserialize(std::list<double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<double>(input,
				       [&value](const double& str) -> void { value.push_back(str); },
				       options,
				       &deserializeValue<double>);
}

template<>
void
PrimitiveMapper<double>::deserialize(std::set<double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<double>(input,
				       [&value](const double& str) -> void { value.insert(str); },
				       options,
				       &deserializeValue<double>);
}

template<>
void
PrimitiveMapper<double>::deserialize(std::map<std::string,double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<double>(input,
				     [&value](const std::pair<std::string,double>& str) -> void { value.insert(str); },
				     options,
				     &deserializeValue<double>);
}

template class PrimitiveMapper<double>;
// PrimitiveMapper specialization for double -

// PrimitiveMapper specialization for long double +
template<>
PrimitiveMapper<long double>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<long double>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<long double>::serialize(const long double& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<long double>, options, field);
}

template<>
bool
PrimitiveMapper<long double>::serialize(const long double* const value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<long double>, options, field);
}

template<>
bool
PrimitiveMapper<long double>::serialize(const std::optional<long double>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &serializeValue<long double>, options, field);
}

template<>
bool
PrimitiveMapper<long double>::serialize(const std::vector<long double>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  anch::json::serializeArray<long double>(value, out, &serializeValue<long double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<long double>::serialize(const std::list<long double>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  anch::json::serializeArray<long double>(value, out, &serializeValue<long double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<long double>::serialize(const std::set<long double>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  anch::json::serializeArray<long double>(value, out, &serializeValue<long double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<long double>::serialize(const std::map<std::string,long double>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  anch::json::serializeMap<long double>(value, out, &serializeValue<long double>, options, field);
  return true;
}

template<>
void
PrimitiveMapper<long double>::deserialize(long double& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<long double>(value, input, options, &deserializeValue<long double>);
}

template<>
void
PrimitiveMapper<long double>::deserialize(std::optional<long double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<long double>(value, input, options, &deserializeValue<long double>);
}

template<>
void
PrimitiveMapper<long double>::deserialize(long double* value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserialize<long double>(value, input, options, &deserializeValue<long double>);
}

template<>
void
PrimitiveMapper<long double>::deserialize(std::vector<long double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<long double>(input,
					    [&value](const long double& str) -> void { value.push_back(str); },
					    options,
					    &deserializeValue<long double>);
}

template<>
void
PrimitiveMapper<long double>::deserialize(std::list<long double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<long double>(input,
					    [&value](const long double& str) -> void { value.push_back(str); },
					    options,
					    &deserializeValue<long double>);
}

template<>
void
PrimitiveMapper<long double>::deserialize(std::set<long double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeArray<long double>(input,
					    [&value](const long double& str) -> void { value.insert(str); },
					    options,
					    &deserializeValue<long double>);
}

template<>
void
PrimitiveMapper<long double>::deserialize(std::map<std::string,long double>& value, std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::deserializeMap<long double>(input,
					  [&value](const std::pair<std::string,long double>& str) -> void { value.insert(str); },
					  options,
					  &deserializeValue<long double>);
}

template class PrimitiveMapper<long double>;
// PrimitiveMapper specialization for long double -
