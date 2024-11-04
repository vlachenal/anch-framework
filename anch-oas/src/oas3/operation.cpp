/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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
#include "oas3/operation.hpp"

#include "json/json.hpp"

using anch::oas3::Operation;

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Operation>& mapper) {
  mapper
    .registerField<std::string>("tags", &Operation::tags)
    .registerField("summary", &Operation::summary)
    .registerField<anch::oas3::ExternalDocs>("externalDocs", &Operation::externalDocs)
    .registerField("operationId", &Operation::operationId)
    .registerField<anch::oas3::Parameter>("parameters", &Operation::parameters)
    .registerField("requestBody", &Operation::requestBody)
    .registerField<anch::oas3::Response>("responses", &Operation::responses)
    .registerField("deprecated", &Operation::deprecated)
    .registerField<anch::oas3::Server>("servers", &Operation::servers)
    ;
}
