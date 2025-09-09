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
#include "date/fmt/constantFormatter.hpp"

#include <iomanip>
#include <sstream>

using std::string;
using std::ostream;
using std::setfill;
using std::setw;
using std::istringstream;

using anch::date::Date;
using anch::date::ConstantFormatter;


ConstantFormatter::ConstantFormatter(const string& message): _message(message) {
  _message.shrink_to_fit();
}

ConstantFormatter::~ConstantFormatter() {
  // Nothing to do
}

void
ConstantFormatter::format(const Date&, ostream& output) const noexcept {
  output << _message;
}

std::size_t
ConstantFormatter::getSize() const noexcept {
  return _message.size();
}

bool
ConstantFormatter::setValue(Date&, const string&) const noexcept {
  // Nothing to do
  return true;
}

const string&
ConstantFormatter::getPattern() const noexcept {
  return _message;
}
