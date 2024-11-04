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
#include <map>

#include "oas3/serverVariable.hpp"

namespace anch::oas3 {

  /*!
   * \brief OpenAPI server specification
   *
   * Server
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct Server {

    /*! REQUIRED. A URL to the target host. This URL supports Server Variables and MAY be relative, to indicate that the host location is relative to the location where the OpenAPI document is being served. Variable substitutions will be made when a variable is named in {brackets}. */
    std::string url

    /*! An optional string describing the host designated by the URL. CommonMark syntax MAY be used for rich text representation. */;
    std::optional<std::string> description;

    /*! A map between a variable name and its value. The value is used for substitution in the server's URL template. */
    std::optional<std::map<std::string,anch::oas3::ServerVariable>> variables;

  };

}  // anch::oas3
