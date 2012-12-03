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

using std::string;
using std::ostream;
using std::setfill;
using std::setw;
using std::istringstream;

using anch::date::Date;
using anch::date::formatter::Year2DFormatter;


const string Year2DFormatter::PATTERN = "%y";


/**
 * {@link Year2DFormatter} default constructor
 */
Year2DFormatter::Year2DFormatter() {
  // Nothing to do
}

/**
 * {@link Year2DFormatter} destructor
 */
Year2DFormatter::~Year2DFormatter() {
  // Nothing to do
}

/**
 * Format date
 *
 * @param date The {@link Date} to format
 * @param output The output stream to write in
 */
void
Year2DFormatter::format(const Date& date, ostream& output) const noexcept {
  output << setfill('0') << setw(2) << getYear(date);
}

/**
 * Get size of formatter part
 *
 * @return The size of formatter part
 */
size_t
Year2DFormatter::getSize() const noexcept {
  return 2;
}

/**
 * Set value to {@link Date}
 *
 * @param date The {@link Date} to modify
 * @param value The value to set
 */
bool
Year2DFormatter::setValue(Date& date, const string& value) const noexcept {
  istringstream iss(value);
  int32_t val;
  iss >> std::dec >> val;
  if(iss.fail()) {
    return false;
  } else {
    setYear(date, 1900 + val);
    return true;
  }
}

/**
 * Get formatter part pattern
 *
 * @return The formatter part pattern
 */
const string&
Year2DFormatter::getPattern() const noexcept {
  return Year2DFormatter::PATTERN;
}
