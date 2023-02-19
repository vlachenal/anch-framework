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
#include <ostream>
#include <filesystem>


namespace anch::cli {

  /*!
   * \brief Application summary
   *
   * Describe current application (name, author, version, copyright, ...)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct App {

    /*! Application name (if not set, it will be set to executable name) */
    std::optional<std::string> name = std::optional<std::string>();

    /*! Version */
    std::optional<std::string> description = std::optional<std::string>();

    /*! Version */
    std::optional<std::string> version = std::optional<std::string>();

    /*! Author */
    std::optional<std::string> author = std::optional<std::string>();

    /*! Copyright */
    std::optional<std::string> copyright = std::optional<std::string>();

    /*! Licence */
    std::optional<std::string> licence = std::optional<std::string>();

    /*! Banner */
    std::optional<std::string> banner = std::optional<std::string>();

    /*! Banner path (it will be taken into account only if \c banner is not set) */
    std::optional<std::filesystem::path> bannerPath = std::optional<std::string>();

  };

}  // anch::cli
