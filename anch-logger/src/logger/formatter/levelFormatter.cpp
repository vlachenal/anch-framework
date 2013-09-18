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
#include "logger/formatter/levelFormatter.hpp"

using std::string;
using std::ostream;

using anch::logger::formatter::LevelFormatter;
using anch::logger::formatter::FormatterType;
using anch::logger::Level;


/*!
 * \ref LevelFormatter default constructor
 */
LevelFormatter::LevelFormatter() {
  // Nothing to do
}

/*!
 * \ref LevelFormatter destructor
 */
LevelFormatter::~LevelFormatter() {
  // Nothing to do
}

/*!
 * Return the level label
 *
 * \param value The logging level
 * \param out The output stream to write in
 */
void
LevelFormatter::formatValue(const void* const value, ostream& out) const noexcept {
  const Level* const level = static_cast<const anch::logger::Level* const>(value);
  out << LEVEL_LABEL.find(*level)->second;
}

/*!
 * Get the formatter type
 *
 * \return The formatter type
 */
FormatterType
LevelFormatter::getType() const noexcept {
  return FormatterType::LEVEL;
}
