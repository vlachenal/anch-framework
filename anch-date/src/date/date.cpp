/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include <algorithm>
#include <ctime>
#include <format>
#include <stdexcept>

#include "date/dateTime.hpp"
#include "date/rfc3339.hpp"
#include "date/time.hpp"

using anch::date::Date;
using anch::date::DateTime;
using anch::date::Zone;
using std::chrono::sys_days;
using std::chrono::year_month_day;

namespace {

  /*! First day ( 0000-01-01 ) */
  const sys_days MIN_DAY = sys_days(year_month_day(std::chrono::year(0), std::chrono::January, std::chrono::day(1)));

  /*! Last day ( 9999-12-31 ) */
  const sys_days MAX_DAY = sys_days(year_month_day(std::chrono::year(9999), std::chrono::December, std::chrono::day(31)));

  /*! Check date: possible date from 0000-01-01 to 9999-12-31 */
  year_month_day
  checked(const year_month_day& date) {
    const int year = static_cast<int>(date.year());
    if(!date.ok() || year < 0 || year > 9999) {
      throw std::invalid_argument(std::format("Invalid date {:04}-{:02}-{:02} (dates are possible days from 0000-01-01 to 9999-12-31)", year,
					      static_cast<unsigned>(date.month()), static_cast<unsigned>(date.day())));
    }
    return date;
  }

  /*! Out of range error */
  [[noreturn]] void
  outOfRange() {
    throw std::invalid_argument("Date out of range (0000-01-01 to 9999-12-31)");
  }

}

// Constructors +
Date::Date(int32_t year, unsigned month, unsigned day):
  _date(checked(year_month_day(std::chrono::year(year), std::chrono::month(month), std::chrono::day(day)))) {
  // Nothing to do
}

Date::Date(const year_month_day& date): _date(checked(date)) {
  // Nothing to do
}

Date::Date(const sys_days& days): _date() {
  if(days < MIN_DAY || days > MAX_DAY) {
    outOfRange();
  }
  _date = year_month_day(days);
}
// Constructors -

// Methods +
Date
Date::today(Zone zone) {
  const auto now = std::chrono::system_clock::now();
  if(zone == Zone::UTC) {
    return Date(std::chrono::floor<std::chrono::days>(now));
  }
  std::time_t secs = std::chrono::system_clock::to_time_t(now);
  std::tm local{};
  ::localtime_r(&secs, &local);
  return Date(local.tm_year + 1900, static_cast<unsigned>(local.tm_mon + 1), static_cast<unsigned>(local.tm_mday));
}

Date
Date::parse(std::string_view text) {
  return anch::date::rfc3339::parseDate(text);
}

std::string
Date::format() const {
  return anch::date::rfc3339::formatDate(*this);
}

unsigned
Date::weekday() const noexcept {
  return std::chrono::weekday(sys_days(_date)).iso_encoding();
}

unsigned
Date::dayOfYear() const noexcept {
  const sys_days first(year_month_day(_date.year(), std::chrono::January, std::chrono::day(1)));
  return static_cast<unsigned>((sys_days(_date) - first).count() + 1);
}

Date
Date::plusDays(int64_t days) const {
  const int64_t current = sys_days(_date).time_since_epoch().count();
  if(days < MIN_DAY.time_since_epoch().count() - current || days > MAX_DAY.time_since_epoch().count() - current) {
    outOfRange();
  }
  return Date(sys_days(std::chrono::days(static_cast<int>(current + days))));
}

Date
Date::plusMonths(int64_t months) const {
  const int64_t current = static_cast<int64_t>(year()) * 12 + month() - 1;
  if(months < -current || months > 9999 * 12 + 11 - current) {
    outOfRange();
  }
  const int64_t total = current + months;
  const std::chrono::year_month newMonth(std::chrono::year(static_cast<int>(total / 12)), std::chrono::month(static_cast<unsigned>(total % 12 + 1)));
  const std::chrono::day last = std::chrono::year_month_day_last(newMonth.year(), std::chrono::month_day_last(newMonth.month())).day();
  return Date(year_month_day(newMonth.year(), newMonth.month(), std::min(_date.day(), last)));
}

Date
Date::plusYears(int64_t years) const {
  if(years < -static_cast<int64_t>(year()) || years > 9999 - static_cast<int64_t>(year())) {
    outOfRange();
  }
  const std::chrono::year newYear(static_cast<int>(year() + years));
  if(_date.month() == std::chrono::February && _date.day() == std::chrono::day(29) && !newYear.is_leap()) {
    return Date(year_month_day(newYear, std::chrono::February, std::chrono::day(28)));
  }
  return Date(year_month_day(newYear, _date.month(), _date.day()));
}

int64_t
Date::daysUntil(const Date& other) const noexcept {
  return (sys_days(other._date) - sys_days(_date)).count();
}

DateTime
Date::startOfDay(Zone zone) const {
  if(zone == Zone::UTC) { // std::time_t constructor checks the range of points in time
    return DateTime(static_cast<std::time_t>(sys_days(_date).time_since_epoch().count()) * 86'400);
  }
  std::tm local{};
  local.tm_year = year() - 1900;
  local.tm_mon = static_cast<int>(month()) - 1;
  local.tm_mday = static_cast<int>(day());
  local.tm_isdst = -1; // let mktime determine daylight saving time
  return DateTime(&local);
}
DateTime
Date::at(const anch::date::Time& time, Zone zone) const {
  const std::chrono::nanoseconds sinceMidnight(time);
  if(zone == Zone::UTC) { // std::timespec constructor checks the range of points in time
    const int64_t secs = std::chrono::duration_cast<std::chrono::seconds>(sinceMidnight).count();
    std::timespec spec{};
    spec.tv_sec = static_cast<std::time_t>(sys_days(_date).time_since_epoch().count()) * 86'400 + static_cast<std::time_t>(secs);
    spec.tv_nsec = static_cast<long>(time.nanosecond());
    return DateTime(spec);
  }
  anch::date::DateFields fields;
  fields.year = year();
  fields.month = static_cast<uint16_t>(month());
  fields.day = static_cast<uint16_t>(day());
  fields.hour = static_cast<uint16_t>(time.hour());
  fields.minute = static_cast<uint16_t>(time.minute());
  fields.second = static_cast<uint16_t>(time.second());
  fields.millisecond = static_cast<uint16_t>(time.nanosecond() / 1'000'000);
  fields.microsecond = static_cast<uint16_t>(time.nanosecond() / 1'000 % 1'000);
  fields.nanosecond = static_cast<uint16_t>(time.nanosecond() % 1'000);
  return DateTime(fields);
}
// Methods -

// Operators +
Date::operator sys_days() const noexcept {
  return sys_days(_date);
}

std::ostream&
anch::date::operator<<(std::ostream& out, const Date& date) {
  char buffer[anch::date::rfc3339::DATE_SIZE];
  return out.write(buffer, static_cast<std::streamsize>(anch::date::rfc3339::formatDate(date, buffer)));
}
// Operators -
