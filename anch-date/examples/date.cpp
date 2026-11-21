/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include "date/date.hpp"

//! [formatter]
#include "date/dateFormatter.hpp"

void
formatterExample() {
  anch::date::DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");
  std::string str;
  formatter.format(anch::date::Date(), str);                     // now
  anch::date::Date date = formatter.parse("2012-10-23 10:02:59.998");
  std::time_t time = static_cast<std::time_t>(date);
  std::cout << str << " " << time << std::endl;
}
//! [formatter]

//! [rfc3339]
#include "date/rfc3339.hpp"

bool
rfc3339Example() {
  anch::date::Date date = anch::date::rfc3339::parseDateTime("2025-07-01T12:00:00.5+02:00");
  std::string utc = anch::date::rfc3339::formatDateTime(date);                                  // 2025-07-01T10:00:00.500Z
  std::string local = anch::date::rfc3339::formatDateTime(date, anch::date::rfc3339::Offset::LOCAL);
  std::chrono::year_month_day day = anch::date::rfc3339::parseDate("2024-02-29");
  std::cout << local << " " << static_cast<int>(day.year()) << std::endl;
  return utc == "2025-07-01T10:00:00.500Z";
}
//! [rfc3339]

int
main() {
  formatterExample();
  return rfc3339Example() ? 0 : 1;
}
