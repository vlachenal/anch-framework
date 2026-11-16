/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifdef ANCH_UUID
#include "json/primitiveMapper.hpp"

#include <string>
#include <stdexcept>

#include "uuid.hpp"

#include "json/mappingError.hpp"


using anch::json::PrimitiveMapper;


// PrimitiveMapper specialization for anch::UUID +
template<>
PrimitiveMapper<anch::UUID>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<anch::UUID>,anch::UUID>() {
  // Nothing to do
}

template<>
PrimitiveMapper<anch::UUID>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
void
PrimitiveMapper<anch::UUID>::serializeValue(const anch::UUID& value, anch::json::WriterContext& context) const {
  char uuid[anch::UUID::STRING_SIZE + 2]; // '"' + UUID + '"' (no allocation)
  uuid[0] = anch::json::STRING_DELIMITER;
  value.toChars(uuid + 1);
  uuid[anch::UUID::STRING_SIZE + 1] = anch::json::STRING_DELIMITER;
  context.write(std::string_view(uuid, sizeof(uuid)));
}

template<>
bool
PrimitiveMapper<anch::UUID>::deserializeValue(anch::UUID& value, anch::json::ReaderContext& context) const {
  std::string uuid;
  if(!anch::json::lexString(uuid, context)) {
    return false;
  }
  try {
    value.parse(uuid);
  } catch(const std::invalid_argument& e) { // invalid UUID format
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context, e.what());
  }
  return true;
}

template class anch::json::PrimitiveMapper<anch::UUID>;
// PrimitiveMapper specialization for anch::UUID -

#endif
