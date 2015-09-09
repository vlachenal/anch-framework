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
#include "logger/formatter/threadIdFormatter.hpp"

#include <thread>

using std::string;
using std::ostream;

using anch::logger::formatter::ThreadIdFormatter;
using anch::logger::formatter::FormatterType;

/*!
 * \ref ThreadIdFormatter default constructor
 */
ThreadIdFormatter::ThreadIdFormatter() {
  // Nothing to do
}

/*!
 * \ref ThreadIdFormatter destructor
 */
ThreadIdFormatter::~ThreadIdFormatter() {
  // Nothing to do
}

/*!
 * Return the input string
 *
 * \param value The input string
 * \param out The output stream to write in
 */
void
ThreadIdFormatter::formatValue(const void* const, ostream& out) const noexcept {
  out << std::this_thread::get_id();
}

/*!
 * Get the formatter type
 *
 * \return The formatter type
 */
FormatterType
ThreadIdFormatter::getType() const noexcept {
  return FormatterType::THREAD_ID;
}
