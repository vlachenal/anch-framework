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
#include "sql/types/time.hpp"

using anch::sql::Time;


// Constructors +
Time::Time(bool init): anch::date::Date(init) {
  _years = 0;
  _months = 0;
  _ydays = 0;
  _mdays = 0;
  _wdays = 0;
}

Time::Time(const anch::date::Date& date): anch::date::Date(date) {
  _years = 0;
  _months = 0;
  _ydays = 0;
  _mdays = 0;
  _wdays = 0;
}

Time::Time(const std::time_t& time): anch::date::Date(time) {
  _years = 0;
  _months = 0;
  _ydays = 0;
  _mdays = 0;
  _wdays = 0;
}

Time::Time(const std::tm* const time): anch::date::Date(time) {
  _years = 0;
  _months = 0;
  _ydays = 0;
  _mdays = 0;
  _wdays = 0;
}
// Constructors -

// Destructor +
Time::~Time() {
  // Nothing to do
}
// Destructor -
