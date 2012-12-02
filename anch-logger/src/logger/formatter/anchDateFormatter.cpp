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
#include "logger/formatter/anchDateFormatter.hpp"

#include "date/dateFormatter.hpp"

using std::string;
using std::ostream;

using anch::logger::formatter::AnchDateFormatter;
using anch::logger::formatter::FormatterType;

using anch::date::Date;


/**
 * {@link AnchDateFormatter} default constructor
 */
AnchDateFormatter::AnchDateFormatter(const string& dateFormat): _dateFormat(dateFormat) {
  // Nothing to do
}

/**
 * {@link AnchDateFormatter} destructor
 */
AnchDateFormatter::~AnchDateFormatter() {
  // Nothing to do
}

/**
 * Return the input string
 *
 * @param value The input string
 * @param out The output stream to write in
 */
void
AnchDateFormatter::formatValue(const void* const value, ostream& out) const noexcept {
  _dateFormat.format(Date(), out);
}

/**
 * Get the formatter type
 *
 * @return The formatter type
 */
FormatterType
AnchDateFormatter::getType() const noexcept {
  return FormatterType::ANCH_DATE;
}
