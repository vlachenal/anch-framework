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
#include "date/date.hpp"

using anch::date::Date;
using std::chrono::nanoseconds;
using std::chrono::seconds;
using std::chrono::sys_time;
using std::chrono::system_clock;

namespace {

  /*!
   * Convert timestamp to time point
   *
   * \param timestamp the number of nanoseconds since epoch
   *
   * \return the time point
   */
  inline sys_time<nanoseconds>
  toTimePoint(std::int64_t timestamp) {
    return sys_time<nanoseconds>(nanoseconds(timestamp));
  }

  /*!
   * Convert time point to local time
   *
   * \param time the time point (truncated toward the past to the second)
   *
   * \return the local time
   */
  std::tm
  toLocalTime(const sys_time<nanoseconds>& time) {
    std::time_t secs = system_clock::to_time_t(std::chrono::floor<seconds>(time));
    std::tm local{};
    ::localtime_r(&secs, &local);
    return local;
  }

  /*!
   * Convert local time to time point
   *
   * \param local the local time (normalized by \c std::mktime )
   *
   * \return the time point
   */
  sys_time<nanoseconds>
  fromLocalTime(std::tm& local) {
    return std::chrono::time_point_cast<nanoseconds>(system_clock::from_time_t(std::mktime(&local)));
  }

}

// Constructors +
Date::Date(bool init) {
  initialize(init ? std::chrono::time_point_cast<nanoseconds>(system_clock::now()) : sys_time<nanoseconds>());
}

Date::Date(const system_clock::time_point& time) {
  initialize(std::chrono::time_point_cast<nanoseconds>(time));
}

Date::Date(const std::time_t& time) {
  initialize(std::chrono::time_point_cast<nanoseconds>(system_clock::from_time_t(time)));
}

Date::Date(const std::tm* const time) {
  std::tm local = *time; // mktime normalizes its argument
  initialize(fromLocalTime(local));
}

Date::Date(const std::timespec& time) {
  initialize(std::chrono::time_point_cast<nanoseconds>(system_clock::from_time_t(time.tv_sec)) + nanoseconds(time.tv_nsec));
}
// Constructors -

// Destructor +
Date::~Date() {
  // Nothing to do
}
// Destructor -

// Methods +
void
Date::initialize(const sys_time<nanoseconds>& time) {
  _timestamp = time.time_since_epoch().count();
  std::tm local = toLocalTime(time);
  _years = local.tm_year + 1900;
  _months = static_cast<uint16_t>(local.tm_mon);
  _ydays = static_cast<uint16_t>(local.tm_yday);
  _mdays = static_cast<uint16_t>(local.tm_mday);
  _wdays = static_cast<uint16_t>(local.tm_wday);
  _hours = static_cast<uint16_t>(local.tm_hour);
  _minutes = static_cast<uint16_t>(local.tm_min);
  _seconds = static_cast<uint16_t>(local.tm_sec);
  auto subsec = (time - std::chrono::floor<seconds>(time)).count(); // [0, 999999999]
  _milliseconds = static_cast<uint16_t>(subsec / 1000000);
  _microseconds = static_cast<uint16_t>(subsec / 1000 % 1000);
  _nanoseconds = static_cast<uint16_t>(subsec % 1000);
}

void
Date::computeTimestamp() {
  std::tm local{};
  local.tm_year = _years - 1900;
  local.tm_mon = _months;
  local.tm_mday = _mdays;
  local.tm_hour = _hours;
  local.tm_min = _minutes;
  local.tm_sec = _seconds;
  local.tm_isdst = -1; // let mktime determine daylight saving time
  initialize(fromLocalTime(local)
	     + std::chrono::milliseconds(_milliseconds)
	     + std::chrono::microseconds(_microseconds)
	     + nanoseconds(_nanoseconds));
}
// Methods -

// Operators +
Date::operator system_clock::time_point() const noexcept {
  return std::chrono::time_point_cast<system_clock::duration>(toTimePoint(_timestamp));
}

Date::operator std::time_t() const noexcept {
  return system_clock::to_time_t(std::chrono::floor<seconds>(toTimePoint(_timestamp)));
}

Date::operator std::tm() const noexcept {
  return toLocalTime(toTimePoint(_timestamp));
}

Date::operator std::timespec() const noexcept {
  auto time = toTimePoint(_timestamp);
  auto secs = std::chrono::floor<seconds>(time);
  std::timespec spec{};
  spec.tv_sec = system_clock::to_time_t(secs);
  spec.tv_nsec = static_cast<long>((time - secs).count());
  return spec;
}
// Operators -
