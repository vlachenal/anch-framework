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
#include <map>
#include <string>

namespace anch::oas3 {

  /*!
   * \brief Request body structure
   *
   * The request body
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct RequestBody {

    /*! A brief description of the request body. This could contain examples of use. CommonMark syntax MAY be used for rich text representation. */
    std::optional<std::string> description;

    /*! The content of the request body. The key is a media type or media type range and the value describes it. For requests that match multiple keys, only the most specific key is applicable. e.g. text/plain overrides text/[*] */
    std::map<std::string, anch::oas3::MediaType> content; // \todo declare it

    /*! Determines if the request body is required in the request. Defaults to false. */
    bool required = false;

  };

}
