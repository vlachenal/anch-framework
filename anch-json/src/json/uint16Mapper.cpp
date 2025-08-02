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

// PrimitiveMapper specialization for uint16_t +
template<>
PrimitiveMapper<uint16_t>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<uint16_t>,uint16_t>() {
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
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<uint16_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const uint16_t* const value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<uint16_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::optional<uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<uint16_t>, options, field);
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::vector<uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint16_t>(value, out, &anch::json::serializeNumericValue<uint16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::list<uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint16_t>(value, out, &anch::json::serializeNumericValue<uint16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::set<uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeArray<uint16_t>(value, out, &anch::json::serializeNumericValue<uint16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint16_t>::serialize(const std::map<std::string,uint16_t>& value,
				     std::ostream& out,
				     const anch::json::MappingOptions& options,
				     const std::optional<std::string>& field) {
  anch::json::serializeMap<uint16_t>(value, out, &anch::json::serializeNumericValue<uint16_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<uint16_t>::deserializeValue(uint16_t& value, anch::json::ReaderContext& context) const {
  anch::json::juint val;
  if(anch::json::lexUInteger(val, context)) {
    if(value > std::numeric_limits<uint16_t>::max()) {
      throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
    }
    value = static_cast<uint16_t>(val);
    return true;
  }
  return false;
}

template class PrimitiveMapper<uint16_t>;
// PrimitiveMapper specialization for uint16_t -
