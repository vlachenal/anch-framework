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
  anch::ut::assertTrue(today == DateTime().date(Zone::UTC) || today.plusDays(1) == DateTime().date(Zone::UTC), "Today");
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
    ;
}
