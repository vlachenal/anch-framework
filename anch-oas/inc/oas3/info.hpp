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

#include "oas3/contact.hpp"
#include "oas3/license.hpp"

namespace anch::oas3 {

  /*!
   * \brief OpenAPI specification information
   *
   * Mandatory API informations
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct Info {

    /*! The title of the API */
    std::string title;

    /*! A short summary of the API */
    std::optional<std::string> summary;

    /*! A description of the API */
    std::optional<std::string> description;

    /*! A URL to the Terms of Service for the API */
    std::optional<std::string> termsOfService;

    /*! The contact information for the exposed API */
    std::optional<anch::oas3::Contact> contact;

    /*! The license information for the exposed API */
    std::optional<anch::oas3::License> license;

    /*! The version of the OpenAPI document */
    std::string version;

  };

}  // anch::oas3
