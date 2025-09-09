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
#include "date/fmt/monthFormatter.hpp"

#include <iomanip>
#include <sstream>

using anch::date::Date;
using anch::date::MonthFormatter;
using anch::date::IDatePartFormatter;


const std::string MonthFormatter::PATTERN = "%m";


MonthFormatter::MonthFormatter() {
  // Nothing to do
}

MonthFormatter::~MonthFormatter() {
  // Nothing to do
}

void
MonthFormatter::format(const Date& date, std::ostream& output) const noexcept {
  output << std::setfill('0') << std::setw(2) << getMonth(date) + 1;
}

std::size_t
MonthFormatter::getSize() const noexcept {
  return 2;
}

bool
MonthFormatter::setValue(Date& date, const std::string& value) const noexcept {
  std::istringstream iss(value);
  uint16_t val;
  iss >> std::dec >> val;
  if(iss.fail()) {
    return false;
  } else {
    if(val > 12) {
      return false;
    } else {
      setMonth(date, static_cast<uint16_t>(val - 1));
      return true;
    }
  }
}

const std::string&
MonthFormatter::getPattern() const noexcept {
  return MonthFormatter::PATTERN;
}

IDatePartFormatter*
MonthFormatter::getInstance() {
  return new MonthFormatter();
}
