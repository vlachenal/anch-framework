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
#include "resource/section.hpp"


using std::string;
using anch::resource::Section;


// Static initialization +
string Section::DEFAULT_VALUE = "";
// Static initialization -


// Constructors +
Section::Section() {
  // Nothing to do
}

Section::Section(const Section& section): _parameters(section._parameters) {
  // Nothing to do
}
// Constructors -

// Destructors +
Section::~Section() {
  // Nothing to do
}
// Destructors -

// Methods +
const std::string&
Section::getParameter(const std::string& parameterName) const {
  auto iter = _parameters.find(parameterName);
  if(iter == _parameters.end()) {
    return Section::DEFAULT_VALUE;
  } else {
    return iter->second;
  }
}

#ifdef ANCH_STD_OTP
std::optional<std::string>
Section::parameter(const std::string& param) const {
  std::optional<std::string> value;
  auto iter = _parameters.find(param);
  if(iter != _parameters.end()) {
    value = iter->second;
  }
  return value;
}
#endif
// Methods -
