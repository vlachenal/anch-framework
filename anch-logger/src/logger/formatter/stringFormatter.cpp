/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standart
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
#include "logger/formatter/stringFormatter.hpp"

using std::string;
using std::ostream;

using anch::logger::formatter::StringFormatter;
using anch::logger::formatter::FormatterType;

/**
 * {@link StringFormatter} default constructor
 */
StringFormatter::StringFormatter() {
  // Nothing to do
}

/**
 * {@link StringFormatter} destructor
 */
StringFormatter::~StringFormatter() {
  // Nothing to do
}

/**
 * Return the input string
 *
 * @param value The input string
 * @param out The output stream to write in
 */
void
StringFormatter::formatValue(const void* const value, ostream& out) const throw() {
  out << *((const string*)value);
}

/**
 * Get the formatter type
 *
 * @return The formatter type
 */
FormatterType
StringFormatter::getType() const throw() {
  return FormatterType::STRING;
}
