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
#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "date/date.hpp"
#include "date/rfc3339.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::date::Date;
using anch::date::rfc3339::Offset;
using std::chrono::year_month_day;

namespace rfc = anch::date::rfc3339;


/*!
 * Build date from nanoseconds since epoch
 */
Date
fromNanos(std::int64_t nanos) {
  return Date(std::chrono::system_clock::time_point(std::chrono::nanoseconds(nanos)));
}

/*!
 * Nanoseconds since epoch of date
 */
std::int64_t
toNanos(const Date& date) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
    static_cast<std::chrono::system_clock::time_point>(date).time_since_epoch()).count();
}

/*!
 * Check that parsing raises std::invalid_argument
 */
template<typename F>
void
checkInvalid(F parse, const std::string& text) {
  try {
    parse(text);
  } catch(const std::invalid_argument& e) {
    std::cout << e.what() << std::endl;
    return;
  }
  anch::ut::fail("'" + text + "' should have been refused");
}

void
testFormatUtc() {
  // 2025-07-01T10:00:00Z
  constexpr std::int64_t base = 1751364000LL * 1'000'000'000;
  const std::pair<std::int64_t,std::string> expected[] = {
    {base, "2025-07-01T10:00:00Z"},
    {base + 120'000'000, "2025-07-01T10:00:00.120Z"},
    {base + 123'456'000, "2025-07-01T10:00:00.123456Z"},
    {base + 1, "2025-07-01T10:00:00.000000001Z"},
    {-500'000'000, "1969-12-31T23:59:59.500Z"},
    {0, "1970-01-01T00:00:00Z"},
  };
  for(const auto& [nanos, text] : expected) {
    std::string res = rfc::formatDateTime(fromNanos(nanos));
    anch::ut::assertThat(res == text, "Expected {} ; found {}", text, res);
  }
}

void
testFormatLocal() {
  // TZ=Europe/Paris
  const std::pair<std::int64_t,std::string> expected[] = {
    {1751364000, "2025-07-01T12:00:00+02:00"},  // summer
    {1735725600, "2025-01-01T11:00:00+01:00"},  // winter
    {-2208988800, "1900-01-01T00:09:00+00:09"}, // Paris mean time (+00:09:21): offset truncated to the minute
  };
  for(const auto& [secs, text] : expected) {
    Date date(static_cast<std::time_t>(secs));
    std::string res = rfc::formatDateTime(date, Offset::LOCAL);
    anch::ut::assertThat(res == text, "Expected {} ; found {}", text, res);
    anch::ut::assertThat(rfc::parseDateTime(res) == date, "{} should be parsed to the same point in time", res);
  }
}

void
testParse() {
  constexpr std::int64_t base = 1751364000LL * 1'000'000'000; // 2025-07-01T10:00:00Z
  const std::pair<std::string,std::int64_t> expected[] = {
    {"2025-07-01T10:00:00Z", base},
    {"2025-07-01t10:00:00z", base},
    {"2025-07-01T12:00:00+02:00", base},
    {"2025-07-01T04:30:00-05:30", base},
    {"2025-07-01T10:00:00-00:00", base},
    {"2025-07-01T10:00:00.1Z", base + 100'000'000},
    {"2025-07-01T10:00:00.123456789Z", base + 123'456'789},
    {"2025-07-01T10:00:00.1234567891234Z", base + 123'456'789}, // truncated
    {"1969-12-31T23:59:59.5Z", -500'000'000},
    {"2025-07-02T09:59:00+23:59", base},
  };
  for(const auto& [text, nanos] : expected) {
    std::int64_t res = toNanos(rfc::parseDateTime(text));
    anch::ut::assertThat(res == nanos, "{}: expected {} ; found {}", text, nanos, res);
  }
  // Round trip +
  for(std::int64_t nanos : std::initializer_list<std::int64_t>{base, base + 7, -1, 4'102'444'799'999'999'999}) {
    Date date = fromNanos(nanos);
    std::string text = rfc::formatDateTime(date);
    anch::ut::assertThat(rfc::parseDateTime(text) == date, "Round trip failed for {}", text);
  }
  // Round trip -
}

void
testLeapSecond() {
  std::int64_t res = toNanos(rfc::parseDateTime("2016-12-31T23:59:60Z"));
  std::int64_t next = toNanos(rfc::parseDateTime("2017-01-01T00:00:00Z"));
  anch::ut::assertThat(res == next, "Leap second should be read as next second");
  res = toNanos(rfc::parseDateTime("2016-12-31T18:59:60-05:00"));
  anch::ut::assertThat(res == next, "Leap second with offset should be read as next second");
  checkInvalid(rfc::parseDateTime, "2016-12-31T22:59:60Z");
  checkInvalid(rfc::parseDateTime, "2016-12-31T23:59:60+01:00");
}

void
testParseInvalid() {
  for(const char* text : {
      "", "2025-07-01", "2025-07-01T10:00:00", "2025-07-01 10:00:00Z", "2025-07-01T10:00Z", "2025-7-01T10:00:00Z",
      "2025-07-01T10:00:00.Z", "2025-07-01T10:00:00+0200", "2025-07-01T10:00:00+02", "2025-07-01T10:00:00+24:00",
      "2025-07-01T10:00:00+02:60", "2025-07-01T10:00:00ZZ", "2025-07-01T10:00:00Z ", " 2025-07-01T10:00:00Z",
      "2025-13-01T10:00:00Z", "2025-00-01T10:00:00Z", "2025-02-29T10:00:00Z", "2025-07-00T10:00:00Z",
      "2025-07-01T24:00:00Z", "2025-07-01T10:60:00Z", "2025-07-01T10:00:61Z", "+025-07-01T10:00:00Z",
      "2025-07-01T1a:00:00Z", "2025-07-01T10:00:00.12a3Z"}) {
    checkInvalid(rfc::parseDateTime, text);
  }
  // Out of Date range (nanoseconds on 64 bits) +
  checkInvalid(rfc::parseDateTime, "1677-01-01T00:00:00Z");
  checkInvalid(rfc::parseDateTime, "2262-12-31T00:00:00Z");
  checkInvalid(rfc::parseDateTime, "9999-12-31T23:59:59Z");
  // Out of Date range (nanoseconds on 64 bits) -
  rfc::parseDateTime("2024-02-29T10:00:00Z");
}

void
testDate() {
  using namespace std::chrono;
  year_month_day date = rfc::parseDate("2024-02-29");
  anch::ut::assertThat(date == 2024y/February/29d, "Invalid date");
  anch::ut::assertThat(rfc::formatDate(date) == "2024-02-29", "Invalid format: {}", rfc::formatDate(date));
  anch::ut::assertThat(rfc::formatDate(0y/January/1d) == "0000-01-01", "Year 0 should be formatted");
  anch::ut::assertThat(rfc::parseDate("9999-12-31") == 9999y/December/31d, "Year 9999 should be parsed");
  for(const char* text : {"", "2025-02-29", "2025-13-01", "2025-1-01", "2025-01-01T00:00:00Z", "2025/01/01", "20250101", " 2025-01-01"}) {
    checkInvalid(rfc::parseDate, text);
  }
  for(year_month_day invalid : {year_month_day{}, 2025y/February/29d, year_month_day(year(10000), January, day(1)), year_month_day(year(-1), January, day(1))}) {
    try {
      rfc::formatDate(invalid);
      anch::ut::fail("Invalid date should not be formatted");
    } catch(const std::invalid_argument& e) {
      std::cout << e.what() << std::endl;
    }
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH date RFC 3339 unit tests")
    .description("Test RFC 3339 date-time and full-date formatting and parsing")
    .add("rfc3339-format-utc", testFormatUtc)
    .add("rfc3339-format-local", testFormatLocal)
    .add("rfc3339-parse", testParse)
    .add("rfc3339-leap-second", testLeapSecond)
    .add("rfc3339-parse-invalid", testParseInvalid)
    .add("rfc3339-date", testDate)
    ;
}
