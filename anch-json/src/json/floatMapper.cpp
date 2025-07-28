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

// PrimitiveMapper specialization for float +
template<>
PrimitiveMapper<float>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<float>,float>() {
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
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<float>, options, field);
}

template<>
bool
PrimitiveMapper<float>::serialize(const float* const value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<float>, options, field);
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::optional<float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<float>, options, field);
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::vector<float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  anch::json::serializeArray<float>(value, out, &anch::json::serializeNumericValue<float>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::list<float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  anch::json::serializeArray<float>(value, out, &anch::json::serializeNumericValue<float>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::set<float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  anch::json::serializeArray<float>(value, out, &anch::json::serializeNumericValue<float>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<float>::serialize(const std::map<std::string,float>& value,
				  std::ostream& out,
				  const anch::json::MappingOptions& options,
				  const std::optional<std::string>& field) {
  anch::json::serializeMap<float>(value, out, &anch::json::serializeNumericValue<float>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<float>::deserialize(float& value, anch::json::ReaderContext& context) const {
  double val;
  if(anch::json::lexNumber(val, context)) {
    value = static_cast<float>(val);
    return true;
  }
  return false;
}

template class PrimitiveMapper<float>;
// PrimitiveMapper specialization for float -
