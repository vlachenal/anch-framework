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

using std::string;
using std::ostream;
using std::setfill;
using std::setw;
using std::istringstream;

using anch::date::Date;
using anch::date::formatter::Hour12Formatter;
using anch::date::formatter::IDatePartFormatter;


const string Hour12Formatter::PATTERN = "%h";


Hour12Formatter::Hour12Formatter() {
  // Nothing to do
}

Hour12Formatter::~Hour12Formatter() {
  // Nothing to do
}

void
Hour12Formatter::format(const Date& date, ostream& output) const noexcept {
  uint16_t hours = getHour(date);
  if(hours >= 12) {
    hours = hours - 12;
  }
  output << setfill('0') << setw(2) << hours;
}

size_t
Hour12Formatter::getSize() const noexcept {
  return 2;
}

bool
Hour12Formatter::setValue(Date& date, const string& value) const noexcept {
  istringstream iss(value);
  uint16_t val;
  iss >> std::dec >> val;
  if(iss.fail()) {
    return false;
  } else {
    if(val > 11) {
      return false;
    } else {
      setHour(date, getHour(date) + val);
      return true;
    }
  }
}

const string&
Hour12Formatter::getPattern() const noexcept {
  return Hour12Formatter::PATTERN;
}

IDatePartFormatter*
Hour12Formatter::getInstance() {
  return new Hour12Formatter();
}
