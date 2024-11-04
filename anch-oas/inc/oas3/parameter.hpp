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
   * \brief OpenAPI specification model
   *
   * The model
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct Parameter {

    /*! Parameter name */
    std::optional<std::string> name;

    /*! Parameter location/type */
    std::string in;

    /*! Description */
    std::optional<std::string> description;

    /*! Parameter requirement flag (default to \c false ) */
    bool required = false;

    /*! Parameter deprecation flag (default to \c false ) */
    bool deprecated = false;

    /*! Parameter allow empty value flag (default to \c false ) */
    bool allowEmptyValue = false;

  };

}
