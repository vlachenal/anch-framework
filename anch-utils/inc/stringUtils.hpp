/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <string>
#include <algorithm>

namespace anch {

  /*!
   * Transform input string to lower caser
   *
   * \param value the string to transform
   */
  void toLower(std::string& value);

  /*!
   * Create new lower case string from  input string
   *
   * \param value the string to transform
   *
   * \return the lower case string
   */
  std::string toLower(const std::string& value);

  /*!
   * Transform input string to upper caser
   *
   * \param value the string to transform
   */
  void toUpper(std::string& value);

  /*!
   * Create new upper case string from  input string
   *
   * \param value the string to transform
   *
   * \return the upper case string
   */
  std::string toUpper(const std::string& value);

}  // anch

inline void
anch::toLower(std::string& value) {
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c){ return std::tolower(c); });
}

inline std::string
anch::toLower(const std::string& value) {
  std::string lower = value;
  anch::toLower(lower);
  return lower;
}

inline void
anch::toUpper(std::string& value) {
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c){ return std::toupper(c); });
}

inline std::string
anch::toUpper(const std::string& value) {
  std::string upper = value;
  anch::toUpper(upper);
  return upper;
}
