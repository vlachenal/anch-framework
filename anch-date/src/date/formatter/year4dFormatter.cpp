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
#include "date/formatter/year4dFormatter.hpp"

#include <iomanip>
#include <sstream>

using std::string;
using std::ostream;
using std::setfill;
using std::setw;
using std::istringstream;

using anch::date::Date;
using anch::date::formatter::Year4DFormatter;
using anch::date::formatter::IDatePartFormatter;


const string Year4DFormatter::PATTERN = "%Y";


Year4DFormatter::Year4DFormatter() {
  // Nothing to do
}

Year4DFormatter::~Year4DFormatter() {
  // Nothing to do
}

void
Year4DFormatter::format(const Date& date, ostream& output) const noexcept {
  output << setfill('0') << setw(4) << getYear(date);
}

size_t
Year4DFormatter::getSize() const noexcept {
  return 4;
}

bool
Year4DFormatter::setValue(Date& date, const string& value) const noexcept {
  istringstream iss(value);
  int32_t val;
  iss >> std::dec >> val;
  if(iss.fail()) {
    return false;
  } else {
    setYear(date, val);
    return true;
  }
}

const string&
Year4DFormatter::getPattern() const noexcept {
  return Year4DFormatter::PATTERN;
}

IDatePartFormatter*
Year4DFormatter::getInstance() {
  return new Year4DFormatter();
}
