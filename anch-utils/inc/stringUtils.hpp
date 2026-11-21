/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2020 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace anch {

  /*!
   * Transform input string to lower case, byte per byte with \c std::tolower .\n
   * The conversion depends on the global C locale (\c std::setlocale ): only ASCII letters are converted with the
   * default \c "C" locale ; multibyte characters (UTF-8) are not converted.
   *
   * \param value the string to transform
   */
  void toLower(std::string& value);

  /*!
   * Transform input string to upper case, byte per byte with \c std::toupper .\n
   * The conversion depends on the global C locale (\c std::setlocale ): only ASCII letters are converted with the
   * default \c "C" locale ; multibyte characters (UTF-8) are not converted.
   *
   * \param value the string to transform
   */
  void toUpper(std::string& value);

}  // anch

#include "impl/stringUtils.hpp"
