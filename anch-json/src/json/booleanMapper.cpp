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
#include <functional>

#include "json/mappingFunctions.hpp"
#include "json/lexer.hpp"

using anch::json::PrimitiveMapper;
using anch::json::ErrorCode;

inline
void
serializeValue(const bool& value, std::ostream& out, [[maybe_unused]] const anch::json::MappingOptions& options) {
  if(value) {
    out.write("true", 4);
  } else {
    out.write("false", 5);
  }
}

// PrimitiveMapper specialization for bool +
template<>
PrimitiveMapper<bool>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<bool>,bool>() {
  // Nothing to do
}

template<>
PrimitiveMapper<bool>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
bool
PrimitiveMapper<bool>::serialize(const bool& value,
				 std::ostream& out,
				 const anch::json::MappingOptions& options,
				 const std::optional<std::string>& field) {
  return anch::json::serialize<bool>(value, out, &serializeValue, options, field);
}

template<>
bool
PrimitiveMapper<bool>::serialize(const bool* const value,
				 std::ostream& out,
				 const anch::json::MappingOptions& options,
				 const std::optional<std::string>& field) {
  return anch::json::serialize<bool>(value, out, &serializeValue, options, field);
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::optional<bool>& value,
				 std::ostream& out,
				 const anch::json::MappingOptions& options,
				 const std::optional<std::string>& field) {
  return anch::json::serialize<bool>(value, out, &serializeValue, options, field);
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::vector<bool>& value,
				 std::ostream& out,
				 const anch::json::MappingOptions& options,
				 const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::list<bool>& value,
				 std::ostream& out,
				 const anch::json::MappingOptions& options,
				 const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::set<bool>& value,
				 std::ostream& out,
				 const anch::json::MappingOptions& options,
				 const std::optional<std::string>& field) {
  anch::json::serializeArray<bool>(value, out, &serializeValue, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<bool>::serialize(const std::map<std::string,bool>& value,
				 std::ostream& out,
				 const anch::json::MappingOptions& options,
				 const std::optional<std::string>& field) {
  anch::json::serializeMap<bool>(value, out, &serializeValue, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<bool>::deserialize(bool& value, anch::json::ReaderContext& context) const {
  return anch::json::lexBoolean(value, context);
}

template class PrimitiveMapper<bool>;
