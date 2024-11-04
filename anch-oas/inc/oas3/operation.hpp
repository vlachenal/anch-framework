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
#include <vector>
#include <optional>

#include "oas3/externalDocs.hpp"
#include "oas3/parameter.hpp"
#include "oas3/requestBody.hpp"
#include "oas3/response.hpp"
#include "oas3/server.hpp"

namespace anch::oas3 {

  /*!
   * \brief OpenAPI specification operation
   *
   * Endpoint operation declaration
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct Operation {

    /*! Tags (declare only one if you want to code will be generated properly) */
    std::optional<sdt::vector<std::string>> tags;

    /*! Summary */
    std::optional<std::string> summary;

    /*! Description */
    std::optional<std::string> description;

    /*! External documentation */
    std::optional<anch::oas3::ExternalDocs> externalDocs;

    /*! Operation identifier (method name) */
    std::string operationId;

    /*! Request's parameters */
    std::optional<std::vector<anch::oas3::Parameter>> parameters; // \todo define ... for union

    /*! Request's body */
    std::optional<anch::oas3::RequestBody> requestBody; // \todo define

    /*! Possible responses (OK and KO) */
    std::optional<std::vector<anch::oas3::Response>> responses; // \todo define

    // \todo callback

    /*! Deprecated flag */
    bool deprecated = false;

    // \todo security

    /*! Servers declaration */
    std::optional<std::vector<anch::oas3::Server>> servers;

  };

}
