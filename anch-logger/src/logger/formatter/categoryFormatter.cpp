/*
  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "logger/formatter/categoryFormatter.hpp"

using std::string;
using std::ostream;

using anch::logger::formatter::CategoryFormatter;
using anch::logger::formatter::FormatterType;

/**
 * {@link CategoryFormatter} default constructor
 */
CategoryFormatter::CategoryFormatter() {
  // Nothing to do
}

/**
 * {@link CategoryFormatter} destructor
 */
CategoryFormatter::~CategoryFormatter() {
  // Nothing to do
}

/**
 * Return the input string
 *
 * @param value The input string
 * @param out The output stream to write in
 */
void
CategoryFormatter::formatValue(const void* const value, ostream& out) const throw() {
  out << *((string*)value);
}

/**
 * Get the formatter type
 *
 * @return The formatter type
 */
FormatterType
CategoryFormatter::getType() const throw() {
  return FormatterType::CATEGORY;
}
