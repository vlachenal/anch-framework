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
#include <string>
#include <functional>


namespace anch::cli {

  class ArgHandler;

  /*!
   * \brief Argument
   *
   * Argument structure
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct Arg {

    /*! Argument function handler */
    std::function<void(const std::string&)> handler;/* = std::function<void(const std::string&)>();*/

    /*! Short option (starts with '-' and have only one character) */
    char sopt = '\0';

    /*! Long option (starts with '--' and use '=' for value) */
    std::optional<std::string> lopt = std::optional<std::string>();

    /*! Consume next CLI arguments */
    bool value = false;

    /*! Value name in help */
    std::optional<std::string> name = std::optional<std::string>();

    /*! Mandatory argument flag (default to \c false ) */
    bool mandatory = false;

    /*! Multiple occurence argument flag (default to \c false ) */
    bool multi = false;

    /*! Description */
    std::optional<std::string> description = std::optional<std::string>();

    /*! Example */
    std::optional<std::string> example = std::optional<std::string>();

  };

}  // anch::cli
