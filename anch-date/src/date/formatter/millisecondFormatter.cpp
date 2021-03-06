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
#include "date/formatter/millisecondFormatter.hpp"

#include <iomanip>
#include <sstream>

using std::string;
using std::ostream;
using std::setfill;
using std::setw;
using std::istringstream;

using anch::date::Date;
using anch::date::formatter::MillisecondFormatter;
using anch::date::formatter::IDatePartFormatter;


const string MillisecondFormatter::PATTERN = "%s";


MillisecondFormatter::MillisecondFormatter() {
  // Nothing to do
}

MillisecondFormatter::~MillisecondFormatter() {
  // Nothing to do
}

void
MillisecondFormatter::format(const Date& date, ostream& output) const noexcept {
  output << setfill('0') << setw(3) << getMillisecond(date);
}

std::size_t
MillisecondFormatter::getSize() const noexcept {
  return 3;
}

bool
MillisecondFormatter::setValue(Date& date, const string& value) const noexcept {
  istringstream iss(value);
  uint16_t val;
  iss >> std::dec >> val;
  if(iss.fail()) {
    return false;
  } else {
    setMillisecond(date, val);
    return true;
  }
}

const string&
MillisecondFormatter::getPattern() const noexcept {
  return MillisecondFormatter::PATTERN;
}

IDatePartFormatter*
MillisecondFormatter::getInstance() {
  return new MillisecondFormatter();
}
