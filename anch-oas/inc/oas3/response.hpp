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

#include <map>
#include <optional>

#include "oas3/header.hpp"
#include "oas3/mediaType.hpp"
#include "oas3/link.hpp"

namespace anch::oas3 {

  /*!
   * \brief OpenAPI specification response
   *
   * The response
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct Response {

    /*! Response description */
    std::string description;

    /*! Reponse headers */
    std::optional<std::map<std::string,anch::oas3::Header>> headers;

    /*! Reponse by content types */
    std::optional<std::map<std::string,anch::oas3::MediaType>> content; // \todo define

    /*! Reponse links ... */
    std::optional<std::map<std::string,anch::oas3::Link>> links;

  };

}
