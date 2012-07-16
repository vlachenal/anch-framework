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
#include "logger/formatter/dateFormatter.hpp"

#include <iomanip>
#include <ctime>
#include <chrono>

using std::string;
using std::ostream;

using anch::logger::formatter::DateFormatter;
using anch::logger::formatter::FormatterType;

/**
 * {@link DateFormatter} default constructor
 */
DateFormatter::DateFormatter(const string& dateFormat): _dateFormat(dateFormat) {
  // Nothing to do
}

/**
 * {@link DateFormatter} destructor
 */
DateFormatter::~DateFormatter() {
  // Nothing to do
}

/**
 * Return the input string
 *
 * @param value The input string
 * @param out The output stream to write in
 */
void
DateFormatter::formatValue(const void* const value, ostream& out) const throw() {
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  char buf[200];
  std::strftime(buf,200,_dateFormat.data(),std::localtime(&now_c));
  out << buf;
}

/**
 * Get the formatter type
 *
 * @return The formatter type
 */
FormatterType
DateFormatter::getType() const throw() {
  return FormatterType::DATE;
}
