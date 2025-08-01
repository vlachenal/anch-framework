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
#include "json/impl/numericsMapper.hpp"
#include "json/lexer.hpp"
#include "convert.hpp"

using anch::json::PrimitiveMapper;

// PrimitiveMapper specialization for uint64_t +
template<>
PrimitiveMapper<uint64_t>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<uint64_t>,uint64_t>() {
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
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<uint64_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const uint64_t* const value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<uint64_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::optional<uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<uint64_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::vector<uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint64_t>(value, out, &anch::json::serializeNumericValue<uint64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::list<uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint64_t>(value, out, &anch::json::serializeNumericValue<uint64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::set<uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint64_t>(value, out, &anch::json::serializeNumericValue<uint64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint64_t>::serialize(const std::map<std::string,uint64_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeMap<uint64_t>(value, out, &anch::json::serializeNumericValue<uint64_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint64_t>::deserializeValue(uint64_t& value, anch::json::ReaderContext& context) const {
  return anch::json::lexUInteger(value, context);
}

template class PrimitiveMapper<uint64_t>;
// PrimitiveMapper specialization for uint64_t -
