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
#include "date/formatter/constantFormatter.hpp"

#include <iomanip>
#include <sstream>

using std::string;
using std::ostream;
using std::setfill;
using std::setw;
using std::istringstream;

using anch::date::Date;
using anch::date::formatter::ConstantFormatter;


/**
 * {@link ConstantFormatter} default constructor
 *
 * @param message The constant message
 */
ConstantFormatter::ConstantFormatter(const string& message): _message(message) {
  _message.shrink_to_fit();
}

/**
 * {@link ConstantFormatter} destructor
 */
ConstantFormatter::~ConstantFormatter() {
  // Nothing to do
}

/**
 * Format date
 *
 * @param date The {@link Date} to format
 * @param output The output stream to write in
 */
void
ConstantFormatter::format(const Date& date, ostream& output) const noexcept {
  output << _message;
}

/**
 * Get size of formatter part
 *
 * @return The size of formatter part
 */
size_t
ConstantFormatter::getSize() const noexcept {
  return _message.size();
}

/**
 * Set value to {@link Date}
 *
 * @param date The {@link Date} to modify
 * @param value The value to set
 */
bool
ConstantFormatter::setValue(Date& date, const string& value) const noexcept {
  // Nothing to do
  return true;
}

/**
 * Get formatter part pattern
 *
 * @return The formatter part pattern
 */
const string&
ConstantFormatter::getPattern() const noexcept {
  return _message;
}
