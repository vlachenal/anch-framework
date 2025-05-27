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
#include "json/mappingFunctions.hpp"

#include <istream>
#include <optional>

#include "json/constants.hpp"
#include "json/json.hpp"
#include "json/mapper.hpp"


void
anch::json::serializeFieldName(std::ostream& out, const std::string& field) {
  out.put(anch::json::STRING_DELIMITER)
    .write(field.data(), static_cast<std::streamsize>(field.size()))
    .put(anch::json::STRING_DELIMITER)
    .put(anch::json::FIELD_VALUE_SEPARATOR);
}

struct UnknownObject {
};

template<>
void
anch::json::registerObject([[maybe_unused]] anch::json::ObjectMapper<UnknownObject>& mapper) {
  // Nothing to do
}

bool
anch::json::serializeNull(std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field) {
  if(!options.serialize_null) {
    return false;
  }
  if(field.has_value()) {
    out.put(anch::json::STRING_DELIMITER)
      .write(field.value().data(), static_cast<std::streamsize>(field.value().size()))
      .put(anch::json::STRING_DELIMITER)
      .put(anch::json::FIELD_VALUE_SEPARATOR);
  }
  out.write("null", 4);
  return true;
}
