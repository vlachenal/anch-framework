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
#include "oas3/pathItem.hpp"

#include "json/json.hpp"
#include "oas3/server.hpp"

using anch::oas3::PathItem;

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<PathItem>& mapper) {
  mapper
    .registerField("$ref", &PathItem::_ref)
    .registerField("summary", &PathItem::summary)
    .registerField("description", &PathItem::description)
    .registerField("get", &PathItem::getVerb)
    .registerField("put", &PathItem::putVerb)
    .registerField("post", &PathItem::postVerb)
    .registerField("delete", &PathItem::deleteVerb)
    .registerField("options", &PathItem::optionsVerb)
    .registerField("head", &PathItem::headVerb)
    .registerField("patch", &PathItem::patchVerb)
    .registerField("trace", &PathItem::traceVerb)
    .registerField("patch", &PathItem::servers)
    .registerField<anch::oas3::Server>("servers", &PathItem::servers)
    // \todo generic path parameters ...
    ;
}
