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
#ifndef _ANCH_UTILS_CONVERT_H_
#define _ANCH_UTILS_CONVERT_H_

#include <sstream>
#include <iomanip>
#include <string>
#include <typeinfo>


namespace anch {

  /*!
   * Performs strict conversion from \c std::string to number
   *
   * \param str the string to convert
   *
   * \return the number
   *
   * \throw std::bad_cast conversion error
   */
  template <typename N>
  N convert(const std::string& str) throw(std::bad_cast) {
    N num;
    std::stringstream conv;
    conv << std::noskipws << str;
    conv >> num;
    if(conv.fail() || !conv.eof()) {
      throw std::bad_cast();
    }
    return num;
  }

}

#endif // _ANCH_UTILS_CONVERT_H_
