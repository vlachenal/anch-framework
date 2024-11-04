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

#include "oas3/model.hpp"
#include "oas3/response.hpp"
#include "oas3/parameter.hpp"
#include "oas3/example.hpp"
#include "oas3/header.hpp"
#include "oas3/securityScheme.hpp"
#include "oas3/link.hpp"
#include "oas3/callback.hpp"
#include "oas3/pathItem.hpp"

namespace anch::oas3 {

  /*!
   * \brief OpenAPI specification components
   *
   * The components
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct Components {

    /*! Models definition */
    std::optional<std::map<std::string,anch::oas3::Model>> schemas;

    /*! Responses definition */
    std::optional<std::map<std::string,anch::oas3::Response>> responses;

    /*! Parameters definition */
    std::optional<std::map<std::string,anch::oas3::Parameter>> parameters;

    /*! Examples definition */
    std::optional<std::map<std::string,anch::oas3::Example>> examples;

    /*! Request bodies definition */
    std::optional<std::map<std::string,anch::oas3::RequestBody>> requestBodies;

    /*! Headers definition */
    std::optional<std::map<std::string,anch::oas3::Header>> headers;

    /*! Security schemes definition */
    std::optional<std::map<std::string,anch::oas3::SecurityScheme>> securitySchemes; // \todo fill it

    /*! Links definition */
    std::optional<std::map<std::string,anch::oas3::Link>> links;

    /*! Callbacks definition */
    std::optional<std::map<std::string,anch::oas3::Callback>> callbacks; // \todo fill it

    /*! Path items definition */
    std::optional<std::map<std::string,anch::oas3::PathItem>> pathItems;

  };

}
