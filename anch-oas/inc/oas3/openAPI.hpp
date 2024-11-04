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
#include <list>
#include <map>

#include "oas3/info.hpp"
#include "oas3/server.hpp"
//#include "oas3/security.hpp"
#include "oas3/tag.hpp"
#include "oas3/pathItem.hpp"
#include "oas3/components.hpp"
#include "oas3/externalDocs.hpp"

namespace anch::oas3 {

  /*!
   * \brief OpenAPI specification schema
   *
   * The schema
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct OpenAPI {

    /*! OpenAPI version */
    std::string openapi;

    /*! API informations */
    anch::oas3::Info info;

    /*!  JSON dialect ?! */
    std::optional<std::string> jsonSchemaDialect;

    /*! Servers declaration ; default to '/' */
    std::optional<std::list<anch::oas3::Server>> servers;

    /*! Endpoints declaration */
    std::map<std::string, anch::oas3::PathItem> paths; // \todo check this

    // \todo webhooks

    /*! Components definition */
    std::optional<anch::oas3::Components> components;

    /*! Security declarations  */
    std::map<std::string, std::list<std::string>> security; // \todo check this

    /*! Tags declaration */
    std::optional<std::list<anch::oas3::Tag>> tags;

    /*! External documentation declaration */
    std::optional<anch::oas3::ExternalDocs> externalDocs;

  };

}  // anch::oas3
