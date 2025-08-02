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

#include <string_view>
#include <set>
#include <vector>
#include <list>
#include <map>

#include "json/mappingFunctions.hpp"
#include "json/mappingError.hpp"
#include "json/impl/stringsMapper.hpp"

using anch::json::PrimitiveMapper;

using StringViewMapper = anch::json::GenericMapper<PrimitiveMapper<std::string_view>,std::string_view>;

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
PrimitiveMapper<std::string_view>::serialize(const std::string_view& value,
					     std::ostream& out,
					     const anch::json::MappingOptions& options,
					     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeStringValue<std::string_view>, options, field);
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::string_view* const value,
					     std::ostream& out,
					     const anch::json::MappingOptions& options,
					     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeStringValue<std::string_view>, options, field);
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::optional<std::string_view>& value,
					     std::ostream& out,
					     const anch::json::MappingOptions& options,
					     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeStringValue<std::string_view>, options, field);
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::vector<std::string_view>& value,
					     std::ostream& out,
					     const anch::json::MappingOptions& options,
					     const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string_view>(value, out, &anch::json::serializeStringValue<std::string_view>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::list<std::string_view>& value,
					     std::ostream& out,
					     const anch::json::MappingOptions& options,
					     const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string_view>(value, out, &anch::json::serializeStringValue<std::string_view>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string_view>::serialize(const std::set<std::string_view>& value,
					     std::ostream& out,
					     const anch::json::MappingOptions& options,
					     const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string_view>(value, out, &anch::json::serializeStringValue<std::string_view>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string_view>::deserializeValue(std::string_view& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::string_view& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::optional<std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::string_view* value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::vector<std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::list<std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::set<std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::map<std::string,std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template class PrimitiveMapper<std::string_view>;
// PrimitiveMapper specialization for std::string_view -
