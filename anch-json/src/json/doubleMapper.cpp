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

// PrimitiveMapper specialization for double +
template<>
PrimitiveMapper<double>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<double>,double>() {
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
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<double>, options, field);
}

template<>
bool
PrimitiveMapper<double>::serialize(const double* const value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<double>, options, field);
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::optional<double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeNumericValue<double>, options, field);
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::vector<double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<double>(value, out, &anch::json::serializeNumericValue<double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::list<double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<double>(value, out, &anch::json::serializeNumericValue<double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::set<double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeArray<double>(value, out, &anch::json::serializeNumericValue<double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<double>::serialize(const std::map<std::string,double>& value,
				   std::ostream& out,
				   const anch::json::MappingOptions& options,
				   const std::optional<std::string>& field) {
  anch::json::serializeMap<double>(value, out, &anch::json::serializeNumericValue<double>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<double>::deserializeValue(double& value, anch::json::ReaderContext& context) const {
  return anch::json::lexNumber(value, context);
}

template class PrimitiveMapper<double>;
// PrimitiveMapper specialization for double -
