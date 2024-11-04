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
#pragma once

#include <optional>

namespace anch::oas3 {

  /*!
   * \brief OpenAPI specification reference
   *
   * A simple object to allow referencing other components in the OpenAPI document, internally and externally.\n
   *
   * The $ref string value contains a URI [RFC3986], which identifies the location of the value being referenced.\n \todo RFC link
   *
   * See the rules for resolving Relative References.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct Reference {

    /*! The reference identifier. This MUST be in the form of a URI. */
    std::string _ref;

    /*! A short summary which by default SHOULD override that of the referenced component. If the referenced object-type does not allow a summary field, then this field has no effect. */
    std::string summary;

    /*! A description which by default SHOULD override that of the referenced component. CommonMark syntax MAY be used for rich text representation. If the referenced object-type does not allow a description field, then this field has no effect. */
    std::optional<std::string> description;

  };

}
