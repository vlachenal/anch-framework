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
#include "date/fmt/markerFormatter.hpp"

#include <iomanip>
#include <sstream>

using anch::date::Date;
using anch::date::MarkerFormatter;
using anch::date::IDatePartFormatter;


const std::string MarkerFormatter::PATTERN = "%p";


MarkerFormatter::MarkerFormatter() {
  // Nothing to do
}

MarkerFormatter::~MarkerFormatter() {
  // Nothing to do
}

void
MarkerFormatter::format(const Date& date, std::ostream& output) const noexcept {
  std::string marker;
  if(getHour(date) < 12) {
    marker = "a.m.";
  } else {
    marker = "p.m.";
  }
  output << marker;
}

std::size_t
MarkerFormatter::getSize() const noexcept {
  return 4;
}

bool
MarkerFormatter::setValue(Date& date, const std::string& value) const noexcept {
  if(value == "p.m.") {
    setHour(date, static_cast<uint16_t>(getHour(date) + 12));
  } else if(value != "a.m.") {
    return false;
  }
  return true;
}

const std::string&
MarkerFormatter::getPattern() const noexcept {
  return MarkerFormatter::PATTERN;
}

IDatePartFormatter*
MarkerFormatter::getInstance() {
  return new MarkerFormatter();
}
