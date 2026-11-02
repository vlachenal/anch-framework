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
#include "log/fmt/dateFormatter.hpp"

#include <iomanip>
#include <ctime>
#include <chrono>

using std::string;
using std::ostream;

using anch::log::fmt::DateFormatter;
using anch::log::fmt::FormatterType;

DateFormatter::DateFormatter(const string& dateFormat): _dateFormat(dateFormat) {
  // Nothing to do
}

DateFormatter::~DateFormatter() {
  // Nothing to do
}

void
DateFormatter::formatValue(const void* const, ostream& out) const noexcept {
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm local;
  if(localtime_r(&now_c, &local) == nullptr) { // std::localtime uses a static buffer shared between threads
    return;
  }
  char buf[200];
  if(std::strftime(buf, sizeof(buf), _dateFormat.data(), &local) == 0) { // too long or empty: buf is indeterminate
    return;
  }
  out << buf;
}

FormatterType
DateFormatter::getType() const noexcept {
  return FormatterType::DATE;
}
