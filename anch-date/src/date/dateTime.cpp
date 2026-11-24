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
#include "date/dateTime.hpp"

#include <limits>
#include <stdexcept>

using anch::date::DateTime;
using std::chrono::nanoseconds;
using std::chrono::seconds;
using std::chrono::sys_time;
using std::chrono::system_clock;

namespace {

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
   *
   * \throw std::invalid_argument date can not be stored in nanoseconds since epoch on 64 bits (about 1678 to 2261)
   */
  /*! Maximum number of seconds from epoch which can be stored in nanoseconds (years 1678 to 2261) */
  constexpr std::time_t MAX_SECONDS = std::numeric_limits<std::int64_t>::max() / 1'000'000'000 - 1;

  /*!
   * Convert seconds from epoch to time point in nanoseconds
   *
   * \param secs the seconds from epoch
   *
   * \return the time point
   *
   * \throw std::invalid_argument date out of range
   */
  sys_time<nanoseconds>
  fromSeconds(std::time_t secs) {
    if(secs < -MAX_SECONDS || secs > MAX_SECONDS) {
      throw std::invalid_argument("DateTime out of range (years 1678 to 2261)");
    }
    return sys_time<nanoseconds>(std::chrono::duration_cast<nanoseconds>(std::chrono::seconds(secs)));
  }

  sys_time<nanoseconds>
  fromLocalTime(std::tm& local) {
    // Seconds whose nanoseconds (and sub-second fields) fit in 64 bits
    return fromSeconds(std::mktime(&local));
  }

}

// Constructors +
DateTime::DateTime(const system_clock::time_point& time): _time(std::chrono::time_point_cast<nanoseconds>(time)) {
  // Nothing to do
}

DateTime::DateTime(const std::time_t& time): _time(fromSeconds(time)) {
  // Nothing to do
}

DateTime::DateTime(const std::tm* const time): _time() {
  std::tm local = *time; // mktime normalizes its argument
  _time = fromLocalTime(local);
}

DateTime::DateTime(const std::timespec& time): _time() {
  if(time.tv_nsec < 0 || time.tv_nsec > 999'999'999) {
    throw std::invalid_argument("Invalid timespec: tv_nsec has to be in [0, 999999999]");
  }
  _time = fromSeconds(time.tv_sec) + nanoseconds(time.tv_nsec);
}

DateTime::DateTime(const anch::date::DateFields& fields): _time() {
  std::tm local{};
  local.tm_year = fields.year - 1900;
  local.tm_mon = fields.month - 1;
  local.tm_mday = fields.day;
  local.tm_hour = fields.hour;
  local.tm_min = fields.minute;
  local.tm_sec = fields.second;
  local.tm_isdst = -1; // let mktime determine daylight saving time
  _time = fromLocalTime(local) + std::chrono::milliseconds(fields.millisecond) + std::chrono::microseconds(fields.microsecond)
    + nanoseconds(fields.nanosecond);
}
// Constructors -

// Methods +
DateTime
DateTime::now() {
  return DateTime(system_clock::now());
}

anch::date::DateFields
DateTime::fields(anch::date::Zone zone) const {
  std::time_t secs = system_clock::to_time_t(std::chrono::floor<seconds>(_time));
  std::tm broken{};
  if(zone == anch::date::Zone::UTC) {
    ::gmtime_r(&secs, &broken);
  } else {
    ::localtime_r(&secs, &broken);
  }
  auto subsec = (_time - std::chrono::floor<seconds>(_time)).count(); // [0, 999999999]
  anch::date::DateFields fields;
  fields.year = broken.tm_year + 1900;
  fields.month = static_cast<uint16_t>(broken.tm_mon + 1);
  fields.day = static_cast<uint16_t>(broken.tm_mday);
  fields.hour = static_cast<uint16_t>(broken.tm_hour);
  fields.minute = static_cast<uint16_t>(broken.tm_min);
  fields.second = static_cast<uint16_t>(broken.tm_sec);
  fields.millisecond = static_cast<uint16_t>(subsec / 1000000);
  fields.microsecond = static_cast<uint16_t>(subsec / 1000 % 1000);
  fields.nanosecond = static_cast<uint16_t>(subsec % 1000);
  fields.weekday = static_cast<uint16_t>(broken.tm_wday == 0 ? 7 : broken.tm_wday);
  fields.yearday = static_cast<uint16_t>(broken.tm_yday + 1);
  return fields;
}

anch::date::Date
DateTime::date(anch::date::Zone zone) const {
  if(zone == anch::date::Zone::UTC) {
    return anch::date::Date(std::chrono::floor<std::chrono::days>(_time));
  }
  const anch::date::DateFields local = fields(zone);
  return anch::date::Date(local.year, local.month, local.day);
}
anch::date::Time
DateTime::time(anch::date::Zone zone) const {
  if(zone == anch::date::Zone::UTC) {
    return anch::date::Time(_time - std::chrono::floor<std::chrono::days>(_time));
  }
  const anch::date::DateFields local = fields(zone);
  return anch::date::Time(local.hour, local.minute, local.second % 60u,
			  static_cast<uint32_t>(local.millisecond) * 1'000'000 + static_cast<uint32_t>(local.microsecond) * 1'000 + local.nanosecond);
}
// Methods -

// Operators +
DateTime::operator system_clock::time_point() const noexcept {
  return std::chrono::time_point_cast<system_clock::duration>(_time);
}

DateTime::operator std::time_t() const noexcept {
  return system_clock::to_time_t(std::chrono::floor<seconds>(_time));
}

DateTime::operator std::tm() const noexcept {
  return toLocalTime(_time);
}

DateTime::operator std::timespec() const noexcept {
  auto secs = std::chrono::floor<seconds>(_time);
  std::timespec spec{};
  spec.tv_sec = system_clock::to_time_t(secs);
  spec.tv_nsec = static_cast<long>((_time - secs).count());
  return spec;
}
// Operators -
