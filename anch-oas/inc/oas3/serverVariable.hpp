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

#include <string>
#include <optional>
#include <set>


namespace anch::oas3 {

  /*!
   * \brief OpenAPI server variable specification
   *
   * Server variable
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct ServerVariable {

    /*! An enumeration of string values to be used if the substitution options are from a limited set. The array MUST NOT be empty. */
    std::optional<std::set<std::string>> enumServ;

    /*! REQUIRED. The default value to use for substitution, which SHALL be sent if an alternate value is not supplied. Note this behavior is different than the Schema Object's treatment of default values, because in those cases parameter values are optional. If the enum is defined, the value MUST exist in the enum's values. */
    std::string defaultServ;

    /*! An optional description for the server variable. CommonMark syntax MAY be used for rich text representation. */
    std::optional<std::string> description;

  };

}
