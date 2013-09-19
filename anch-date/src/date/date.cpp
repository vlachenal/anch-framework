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
#include "date/date.hpp"


using std::mutex;
using std::tm;
using std::time_t;
using std::mktime;
using std::localtime;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

using anch::date::Date;


// Static initialization +
mutex Date::_mutex;
// Static initialization -


//Constructors +
/*!
 * \ref Date default constructor.
 *
 * \param init Initialize the \ref Date to now (default true)
 */
Date::Date(bool init) {
  if(init) {
    time_point<high_resolution_clock> now = high_resolution_clock::now();
    auto epoch = now.time_since_epoch();
    _timestamp = duration_cast<std::chrono::nanoseconds>(epoch).count();
    initialize(epoch); // Retrieve ms, µs, ns
    // Initialize others fields according to UTC time
    time_t time = system_clock::to_time_t(now);
    _mutex.lock();
    initialize(localtime(&time));
    _mutex.unlock();
  }
}

/*!
 * \ref Date private copy constructor
 *
 * \param date The \ref Date to copy
 */
Date::Date(const Date& date): _timestamp(date._timestamp),
			      _years(date._years),
			      _months(date._months),
			      _ydays(date._ydays),
			      _mdays(date._mdays),
			      _wdays(date._wdays),
			      _hours(date._hours),
			      _minutes(date._minutes),
			      _seconds(date._seconds),
			      _milliseconds(date._milliseconds),
			      _microseconds(date._microseconds),
			      _nanoseconds(date._nanoseconds) {
  // Nothing to do more => every fields have been initialized
}

/*!
 * \ref Date constructor.
 *
 * \param time The time to set
 */
Date::Date(const time_t& time) {
  time_point<system_clock> timePoint = system_clock::from_time_t(time);
  _timestamp = duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch()).count();
  _mutex.lock();
  initialize(localtime(&time));
  _mutex.unlock();
}

/*!
 * \ref Date constructor.
 *
 * \param time The time to set
 */
Date::Date(const tm* const time) {
  _mutex.lock();
  // Copy time to not change time values
  tm tmpTm = tm(*time);
  time_point<system_clock> timePoint = system_clock::from_time_t(mktime(&tmpTm));
  _timestamp = duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch()).count();
  initialize(time);
  _mutex.unlock();
}
//Constructors -

// Destructors +
/*!
 * \ref Date destructor
 */
Date::~Date() {
  // Nothing to do
}
// Destructors -


// Methods +
/*!
 * Initialize date fields
 *
 * \param time The current time
 */
void
Date::initialize(const tm* const time) {
  _seconds = static_cast<uint16_t>(time->tm_sec);
  _minutes = static_cast<uint16_t>(time->tm_min);
  if(time->tm_isdst > 0) {
    _hours = static_cast<uint16_t>(time->tm_hour) + 1;
  } else {
    _hours = static_cast<uint16_t>(time->tm_hour);
  }
  _wdays = static_cast<uint16_t>(time->tm_wday);
  _mdays = static_cast<uint16_t>(time->tm_mday);
  _ydays = static_cast<uint16_t>(time->tm_yday);
  _months = static_cast<uint16_t>(time->tm_mon);
  _years = static_cast<int32_t>(time->tm_year) + 1900;
}

/*!
 * Compute timestamp from internal members
 */
void
Date::computeTimestamp() {
  _mutex.lock();
  tm tmpTm;
  computeTm(tmpTm);

  time_point<system_clock> timePoint = system_clock::from_time_t(mktime(&tmpTm));
  _timestamp = duration_cast<std::chrono::nanoseconds>(timePoint.time_since_epoch()).count();
  _mutex.unlock();
}
// Methods -
