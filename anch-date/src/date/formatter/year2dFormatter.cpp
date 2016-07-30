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
#include "date/formatter/year2dFormatter.hpp"

#include <iomanip>
#include <sstream>

using anch::date::Date;
using anch::date::formatter::Year2DFormatter;
using anch::date::formatter::IDatePartFormatter;


const std::string Year2DFormatter::PATTERN = "%y";


Year2DFormatter::Year2DFormatter() {
  // Nothing to do
}

Year2DFormatter::~Year2DFormatter() {
  // Nothing to do
}

void
Year2DFormatter::format(const Date& date, std::ostream& output) const noexcept {
  output << std::setfill('0') << std::setw(2) << getYear(date);
}

std::size_t
Year2DFormatter::getSize() const noexcept {
  return 2;
}

bool
Year2DFormatter::setValue(Date& date, const std::string& value) const noexcept {
  std::istringstream iss(value);
  int32_t val;
  iss >> std::dec >> val;
  if(iss.fail()) {
    return false;
  } else {
    setYear(date, 1900 + val);
    return true;
  }
}

const std::string&
Year2DFormatter::getPattern() const noexcept {
  return Year2DFormatter::PATTERN;
}

IDatePartFormatter*
Year2DFormatter::getInstance() {
  return new Year2DFormatter();
}
