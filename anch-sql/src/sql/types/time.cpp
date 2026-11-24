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
#include "sql/types/time.hpp"

using anch::sql::Time;


namespace {

  /*!
   * Time of day of a point in time (local time), on 1970-01-01
   *
   * \param date the point in time
   *
   * \return the time of day
   */
  anch::date::DateTime
  timeOfDay(const anch::date::DateTime& date) {
    anch::date::DateFields fields = date.fields();
    fields.year = 1970;
    fields.month = 1;
    fields.day = 1;
    return anch::date::DateTime(fields);
  }

}

// Constructors +
Time::Time(bool init): anch::date::DateTime(timeOfDay(init ? anch::date::DateTime::now() : anch::date::DateTime())) {
  // Nothing to do
}

Time::Time(const anch::date::DateTime& date): anch::date::DateTime(timeOfDay(date)) {
  // Nothing to do
}

Time::Time(const std::time_t& time): anch::date::DateTime(timeOfDay(anch::date::DateTime(time))) {
  // Nothing to do
}

Time::Time(const std::tm* const time): anch::date::DateTime(timeOfDay(anch::date::DateTime(time))) {
  // Nothing to do
}
// Constructors -
