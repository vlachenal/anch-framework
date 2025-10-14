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
#include <filesystem>

#include "ini/section.hpp"
#include "ini/parserError.hpp"


namespace anch::ini {

  /*!
   * Parse file
   *
   * \param path the file's path (as string)
   *
   * \return the root section
   *
   * \throw \ref anch::ini::ParseError parsing exception
   */
  anch::ini::Section parse(const std::string& path);

  /*!
   * Parse file
   *
   * \param path the file's path
   *
   * \return the root section
   *
   * \throw \ref anch::ini::ParseError parsing exception
   */
  anch::ini::Section parse(const std::filesystem::path& path);

  /*!
   * Parse file
   *
   * \param path the file's path
   * \param section the section to fill
   *
   * \throw \ref anch::ini::ParseError parsing exception
   */
  void merge(const std::filesystem::path& path, anch::ini::Section& section);

}

#include "ini/impl/parser.hpp"
