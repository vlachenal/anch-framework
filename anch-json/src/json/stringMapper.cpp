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

#include "json/mappingError.hpp"
#include "json/lexer.hpp"


using anch::json::PrimitiveMapper;


// PrimitiveMapper specialization for std::string +
template<>
PrimitiveMapper<std::string>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<std::string>,std::string>() {
  // Nothing to do
}

template<>
PrimitiveMapper<std::string>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
void
PrimitiveMapper<std::string>::serializeValue(const std::string& value, anch::json::WriterContext& context) const {
    context.output.put(anch::json::STRING_DELIMITER)
      .write(value.data(), static_cast<std::streamsize>(value.size()))
      .put(anch::json::STRING_DELIMITER);
}

template<>
bool
PrimitiveMapper<std::string>::deserializeValue(std::string& value, anch::json::ReaderContext& context) const {
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
