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

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include "date/dateTime.hpp"
#include "date/time.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::date::Date;
using anch::date::DateTime;
using anch::date::Zone;

namespace {

  template<typename F>
  void
  checkRefused(F func, const std::string& msg) {
    try {
      func();
    } catch(const std::invalid_argument& e) {
      return;
    }
    anch::ut::fail(msg);
  }

}

void
testConstruction() {
  Date date(2024, 2, 29);
  anch::ut::assertThat(date.year() == 2024 && date.month() == 2 && date.day() == 29, "Unexpected fields {}", date.format());
  anch::ut::assertThat(Date() == Date(1970, 1, 1), "Default date has to be epoch day");
  checkRefused([]() { Date(2025, 2, 29); }, "2025-02-29 has to be refused");
  checkRefused([]() { Date(2025, 13, 1); }, "Month 13 has to be refused");
  checkRefused([]() { Date(2025, 4, 31); }, "April 31 has to be refused");
  checkRefused([]() { Date(10000, 1, 1); }, "Year 10000 has to be refused");
  checkRefused([]() { Date(-1, 12, 31); }, "Year -1 has to be refused");
  checkRefused([]() { Date(std::chrono::year_month_day{}); }, "Invalid year_month_day has to be refused");
  // Conversions
  using namespace std::chrono;
  anch::ut::assertThat(static_cast<year_month_day>(date) == 2024y/February/29d && Date(2024y/February/29d) == date, "year_month_day conversions");
  anch::ut::assertThat(static_cast<sys_days>(Date(1970, 1, 2)).time_since_epoch().count() == 1 && Date(sys_days(days(-1))) == Date(1969, 12, 31),
		       "sys_days conversions");
  checkRefused([]() { Date(sys_days(days(3'000'000))); }, "Day after 9999-12-31 has to be refused");
  // Format and parse
  std::ostringstream out;
  out << Date(5, 6, 7);
  anch::ut::assertThat(out.str() == "0005-06-07" && Date::parse("2024-02-29") == date && date.format() == "2024-02-29", "Format: {}", out.str());
  checkRefused([]() { Date::parse("2025-02-29"); }, "Impossible date has to be refused");
}

void
testComparison() {
  Date first(2024, 2, 29);
  Date second(2024, 3, 1);
  anch::ut::assertTrue(first < second && second > first && first <= first && first != second && first == Date(2024, 2, 29), "Dates are ordered");
  std::set<Date> dates{second, first, Date(1999, 12, 31)};
  anch::ut::assertTrue(*dates.begin() == Date(1999, 12, 31) && *dates.rbegin() == second, "Dates are sorted");
  static_assert(Date() == Date() && Date() <= Date(), "Comparisons are constexpr");
}

void
testFields() {
  Date date(2024, 3, 1);
  anch::ut::assertThat(date.weekday() == 5 && Date(2024, 3, 3).weekday() == 7 && Date(2024, 3, 4).weekday() == 1, "ISO weekdays: {}", date.weekday());
  anch::ut::assertThat(date.dayOfYear() == 61 && Date(2023, 12, 31).dayOfYear() == 365 && Date(2024, 12, 31).dayOfYear() == 366,
		       "Days of year: {}", date.dayOfYear());
  anch::ut::assertTrue(date.isLeapYear() && !Date(2100, 1, 1).isLeapYear() && Date(2000, 1, 1).isLeapYear(), "Leap years");
}

void
testArithmetic() {
  Date date(2024, 1, 31);
  anch::ut::assertThat(date.plusDays(30) == Date(2024, 3, 1) && date.plusDays(-31) == Date(2023, 12, 31), "Days: {}", date.plusDays(30).format());
  anch::ut::assertThat(date.plusMonths(1) == Date(2024, 2, 29) && date.plusMonths(13) == Date(2025, 2, 28) && date.plusMonths(-2) == Date(2023, 11, 30),
		       "Months (last day of shorter months): {}", date.plusMonths(1).format());
  anch::ut::assertThat(Date(2024, 2, 29).plusYears(1) == Date(2025, 2, 28) && Date(2024, 2, 29).plusYears(4) == Date(2028, 2, 29),
		       "Years (February 29)");
  anch::ut::assertThat(Date(2024, 2, 29).daysUntil(Date(2025, 2, 28)) == 365 && Date(2025, 2, 28).daysUntil(Date(2024, 2, 29)) == -365,
		       "Days until");
  checkRefused([]() { Date(9999, 12, 31).plusDays(1); }, "Day after 9999-12-31 has to be refused");
  checkRefused([]() { Date(0, 1, 1).plusMonths(-1); }, "Month before 0000-01 has to be refused");
  checkRefused([]() { Date(2000, 1, 1).plusYears(8000); }, "Year 10000 has to be refused");
  checkRefused([]() { Date(2000, 1, 1).plusDays(INT64_MAX); }, "Huge number of days has to be refused");
}

void
testDateTime() {
  // Conversions with points in time give the time zone
  ::setenv("TZ", "Asia/Tokyo", 1); // UTC+9, no daylight saving time
  ::tzset();
  DateTime time(static_cast<std::time_t>(1735689600 - 3600)); // 2024-12-31T23:00:00Z = 2025-01-01T08:00:00+09:00
  anch::ut::assertThat(time.date(Zone::UTC) == Date(2024, 12, 31) && time.date(Zone::LOCAL) == Date(2025, 1, 1) && time.date() == Date(2025, 1, 1),
		       "Day of a point in time: {} / {}", time.date(Zone::UTC).format(), time.date(Zone::LOCAL).format());
  Date day(2025, 1, 1);
  anch::ut::assertThat(static_cast<std::time_t>(day.startOfDay(Zone::UTC)) == 1735689600, "UTC start of day");
  anch::ut::assertThat(static_cast<std::time_t>(day.startOfDay(Zone::LOCAL)) == 1735689600 - 9 * 3600, "Local start of day");
  Date today = Date::today(Zone::UTC);
  anch::ut::assertTrue(today == DateTime::now().date(Zone::UTC) || today.plusDays(1) == DateTime::now().date(Zone::UTC), "Today");
  ::unsetenv("TZ");
  ::tzset();
}

void
testDateTimeFields() {
  // A point in time is only a number of nanoseconds since epoch: fields are computed on demand
  static_assert(sizeof(DateTime) == sizeof(int64_t), "DateTime holds a time point only");
  ::setenv("TZ", "Asia/Tokyo", 1);
  ::tzset();
  DateTime time(std::chrono::system_clock::time_point(std::chrono::nanoseconds(1735689600123456789))); // 2025-01-01T00:00:00.123456789Z
  anch::date::DateFields utc = time.fields(Zone::UTC);
  anch::date::DateFields local = time.fields();
  anch::ut::assertThat(utc.year == 2025 && utc.month == 1 && utc.day == 1 && utc.hour == 0 && utc.millisecond == 123 && utc.microsecond == 456
		       && utc.nanosecond == 789 && utc.weekday == 3 && utc.yearday == 1, "Unexpected UTC fields {}-{}-{} {}h", utc.year, utc.month, utc.day, utc.hour);
  anch::ut::assertThat(local.hour == 9 && local.day == 1 && local.nanosecond == 789, "Unexpected local fields {}h", local.hour);
  anch::ut::assertTrue(DateTime(local) == time, "Point in time has to be built from its local fields");
  anch::ut::assertThat(static_cast<std::time_t>(DateTime()) == 0 && DateTime::now() > DateTime(static_cast<std::time_t>(1735689600)), "Epoch and now");
  ::unsetenv("TZ");
  ::tzset();
}

void
testTime() {
  using namespace std::chrono_literals;
  anch::date::Time time(13, 45, 30, 120'000'000);
  anch::ut::assertThat(time.hour() == 13 && time.minute() == 45 && time.second() == 30 && time.nanosecond() == 120'000'000, "Unexpected fields");
  anch::ut::assertThat(anch::date::Time() == anch::date::Time(0, 0) && time.format() == "13:45:30.120" && anch::date::Time(1, 2, 3).format() == "01:02:03"
		       && anch::date::Time(1, 2, 3, 4).format() == "01:02:03.000000004", "Format: {}", time.format());
  anch::ut::assertTrue(anch::date::Time::parse("13:45:30.12") == time && anch::date::Time::parse("23:59:59.9999999999") == anch::date::Time(23, 59, 59, 999'999'999),
		       "Parse");
  for(const char* text : {"24:00:00", "12:60:00", "12:00:60", "12:00", "12:00:00Z", "12:00:00+01:00", "12:00:00.", " 12:00:00"}) {
    checkRefused([text]() { anch::date::Time::parse(text); }, std::string("Invalid time ") + text + " has to be refused");
  }
  checkRefused([]() { anch::date::Time(24, 0); }, "Hour 24 has to be refused");
  checkRefused([]() { anch::date::Time(std::chrono::nanoseconds(24h)); }, "24 hours has to be refused");
  anch::ut::assertTrue(anch::date::Time(23, 0).plus(2h) == anch::date::Time(1, 0) && anch::date::Time(1, 0).plus(-2h) == anch::date::Time(23, 0)
		       && anch::date::Time(1, 0).plus(-50h) == anch::date::Time(23, 0) && anch::date::Time(1, 0) < time, "Arithmetic and order");
  anch::ut::assertTrue(static_cast<std::chrono::nanoseconds>(anch::date::Time(0, 1)) == 60s, "Duration since midnight");
  std::ostringstream out;
  out << anch::date::Time(7, 8, 9);
  anch::ut::assertThat(out.str() == "07:08:09", "Stream: {}", out.str());
  // Conversions with points in time give the time zone
  ::setenv("TZ", "Asia/Tokyo", 1);
  ::tzset();
  DateTime point(std::chrono::system_clock::time_point(std::chrono::nanoseconds(1735689600123456789))); // 2025-01-01T00:00:00.123456789Z
  anch::ut::assertThat(point.time(Zone::UTC) == anch::date::Time(0, 0, 0, 123456789) && point.time() == anch::date::Time(9, 0, 0, 123456789),
		       "Time of day of a point in time: {} / {}", point.time(Zone::UTC).format(), point.time().format());
  Date day(2025, 1, 1);
  anch::ut::assertTrue(day.at(anch::date::Time(0, 0, 0, 123456789), Zone::UTC) == point && day.at(anch::date::Time(9, 0, 0, 123456789)) == point,
		       "Point in time of a time of day");
  checkRefused([]() { Date(9999, 12, 31).at(anch::date::Time(), Zone::UTC); }, "Point in time out of range has to be refused");
  checkRefused([]() { Date(1, 1, 1).startOfDay(Zone::UTC); }, "Point in time out of range has to be refused");
  ::unsetenv("TZ");
  ::tzset();
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH date unit tests")
    .description("Test AnCH calendar days (anch::date::Date)")
    .add("date-construction", testConstruction)
    .add("date-comparison", testComparison)
    .add("date-fields", testFields)
    .add("date-arithmetic", testArithmetic)
    .add("date-datetime", testDateTime)
    .add("datetime-fields", testDateTimeFields)
    .add("time", testTime)
    ;
}
