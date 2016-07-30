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
#include "date/formatter/hour12Formatter.hpp"

#include <iomanip>
#include <sstream>

using anch::date::Date;
using anch::date::formatter::Hour12Formatter;
using anch::date::formatter::IDatePartFormatter;


const std::string Hour12Formatter::PATTERN = "%h";


Hour12Formatter::Hour12Formatter() {
  // Nothing to do
}

Hour12Formatter::~Hour12Formatter() {
  // Nothing to do
}

void
Hour12Formatter::format(const Date& date, std::ostream& output) const noexcept {
  uint16_t hours = getHour(date);
  if(hours >= 12) {
    hours = static_cast<uint16_t>(hours - 12);
  }
  output << std::setfill('0') << std::setw(2) << hours;
}

std::size_t
Hour12Formatter::getSize() const noexcept {
  return 2;
}

bool
Hour12Formatter::setValue(Date& date, const std::string& value) const noexcept {
  std::istringstream iss(value);
  uint16_t val;
  iss >> std::dec >> val;
  if(iss.fail()) {
    return false;
  } else {
    if(val > 11) {
      return false;
    } else {
      setHour(date, static_cast<uint16_t>(getHour(date) + val));
      return true;
    }
  }
}

const std::string&
Hour12Formatter::getPattern() const noexcept {
  return Hour12Formatter::PATTERN;
}

IDatePartFormatter*
Hour12Formatter::getInstance() {
  return new Hour12Formatter();
}
