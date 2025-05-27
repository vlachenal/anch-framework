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
#include <map>

#include "json/mappingFunctions.hpp"
#include "json/mappingError.hpp"
#include "json/impl/stringsMapper.hpp"
#include "json/lexer.hpp"


using anch::json::PrimitiveMapper;


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
PrimitiveMapper<std::string>::serialize(const std::string& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeStringValue<std::string>, options, field);
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::string* const value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeStringValue<std::string>, options, field);
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::optional<std::string>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  return anch::json::serialize(value, out, &anch::json::serializeStringValue<std::string>, options, field);
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::vector<std::string>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string>(value, out, &anch::json::serializeStringValue<std::string>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::list<std::string>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string>(value, out, &anch::json::serializeStringValue<std::string>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::set<std::string>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  anch::json::serializeArray<std::string>(value, out, &anch::json::serializeStringValue<std::string>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string>::serialize(const std::map<std::string,std::string>& value,
					std::ostream& out,
					const anch::json::MappingOptions& options,
					const std::optional<std::string>& field) {
  anch::json::serializeMap<std::string>(value, out, &anch::json::serializeStringValue<std::string>, options, field);
  return true;
}

template<>
bool
PrimitiveMapper<std::string>::deserialize(std::string& value, anch::json::ReaderContext& context) {
  return anch::json::lexString(value, context);
}

template class PrimitiveMapper<std::string>;
// PrimitiveMapper specialization for std::string -

// PrimitiveMapper specialization for anch::UUID +
#ifdef ANCH_UUID
#include "uuid/uuid.hpp"
using anch::uuid::UUID;
#endif
// PrimitiveMapper specialization for anch::UUID -

// PrimitiveMapper specialization for anch::date::Date +
#ifdef ANCH_DATE
#include "date/date.hpp"
using anch::date::Date;
#endif
// PrimitiveMapper specialization for anch::date::Date -
