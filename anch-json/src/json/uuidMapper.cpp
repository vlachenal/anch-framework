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
#ifdef ANCH_UUID
#include "json/primitiveMapper.hpp"

#include <string>

#include "uuid.hpp"


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
  context.output.put(anch::json::STRING_DELIMITER)
    .write(value.toString().data(), 36)
    .put(anch::json::STRING_DELIMITER);
}

template<>
bool
PrimitiveMapper<anch::UUID>::deserializeValue(anch::UUID& value, anch::json::ReaderContext& context) const {
  std::string uuid;
  if(!anch::json::lexString(uuid, context)) {
    return false;
  }
  value.parse(uuid);
  return true;
}

template class PrimitiveMapper<anch::UUID>;
// PrimitiveMapper specialization for anch::UUID -

#endif
