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

// PrimitiveMapper specialization for int32_t +
template<>
PrimitiveMapper<int32_t>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<int32_t>,int32_t>() {
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
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<int32_t>, options, field);
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const int32_t* const value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<int32_t>, options, field);
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::optional<int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<int32_t>, options, field);
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::vector<int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int32_t>(value, out, &anch::json::serializeNumericValue<int32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::list<int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int32_t>(value, out, &anch::json::serializeNumericValue<int32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::set<int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeArray<int32_t>(value, out, &anch::json::serializeNumericValue<int32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int32_t>::serialize(const std::map<std::string,int32_t>& value,
				    std::ostream& out,
				    const anch::json::MappingOptions& options,
				    const std::optional<std::string>& field) {
  anch::json::serializeMap<int32_t>(value, out, &anch::json::serializeNumericValue<int32_t>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<int32_t>::deserialize(int32_t& value, anch::json::ReaderContext& context) const {
  anch::json::jint val;
  if(anch::json::lexInteger(val, context)) {
    if(value < std::numeric_limits<int32_t>::min() || value > std::numeric_limits<int32_t>::max()) {
      throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
    }
    value = static_cast<int32_t>(val);
    return true;
  }
  return false;
}

template class PrimitiveMapper<int32_t>;
// PrimitiveMapper specialization for int32_t -
