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
#include "logger/formatter/constFormatter.hpp"

using std::string;
using std::ostream;

using anch::logger::formatter::ConstFormatter;
using anch::logger::formatter::FormatterType;


/**
 * {@link ConstFormatter} constructor
 *
 * @param strToPrint The string to print
 */
ConstFormatter::ConstFormatter(const string strToPrint): _strToPrint(strToPrint) {
  // Nothing to do
}

/**
 * {@link ConstFormatter} destructor
 */
ConstFormatter::~ConstFormatter() {
  // Nothing to do
}

/**
 * Return the constant string
 *
 * @param value Nothing (NULL will be passed every time)
 * @param out The output stream to write in
 */
void
ConstFormatter::formatValue(const void* const value, ostream& out) const throw() {
  out << _strToPrint;
}

/**
 * Get the formatter type
 *
 * @return The formatter type
 */
FormatterType
ConstFormatter::getType() const throw() {
  return anch::logger::formatter::FormatterType::CONST;
}
