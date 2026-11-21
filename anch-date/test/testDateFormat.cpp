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
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <stdexcept>
#include <type_traits>

#include "date/dateFormatter.hpp"
#include "date/fmt/iDatePartFormatter.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::date::Date;
using anch::date::DateFormatter;
using anch::date::IDatePartFormatter;


// Custom formatter +
class MicroFormatter : public IDatePartFormatter {
public:
  static const std::string PATTERN;
  MicroFormatter() {}
  virtual ~MicroFormatter() {}
  void format(const Date& date, std::ostream& output) const noexcept {
    output << std::setfill('0') << std::setw(3) << getMicrosecond(date);
  }
  size_t getSize() const noexcept {
    return 3;
  }
  bool setValue(Date& date, const std::string& value) const noexcept {
    std::istringstream iss(value);
    uint16_t val;
    iss >> std::dec >> val;
    if(iss.fail()) {
      return false;
    } else {
      setMicrosecond(date, val);
      return true;
    }
  }
  const std::string& getPattern() const noexcept {
    return MicroFormatter::PATTERN;
  }
  static IDatePartFormatter* getInstance() {
    return new MicroFormatter();
  }
};
const std::string MicroFormatter::PATTERN = "%v";

class NanoFormatter : public IDatePartFormatter {
public:
  static const std::string PATTERN;
  NanoFormatter() {}
  virtual ~NanoFormatter() {}
  void format(const Date& date, std::ostream& output) const noexcept {
    output << std::setfill('0') << std::setw(3) << getNanosecond(date);
  }
  size_t getSize() const noexcept {
    return 3;
  }
  bool setValue(Date& date, const std::string& value) const noexcept {
    std::istringstream iss(value);
    uint16_t val;
    iss >> std::dec >> val;
    if(iss.fail()) {
      return false;
    } else {
      setNanosecond(date, val);
      return true;
    }
  }
  const std::string& getPattern() const noexcept {
    return NanoFormatter::PATTERN;
  }
  static IDatePartFormatter* getInstance() {
    return new NanoFormatter();
  }
};
// Custom formatter -

const std::string NanoFormatter::PATTERN = "%N";
DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");

void
testFormatDate() {
  std::cout << "Enter in test date format" << std::endl;

  std::string nowStr;
  Date now = Date();
  formatter.format(now, nowStr);
  std::cout << "Date from scratch: " << nowStr << std::endl;

  std::cout << "Exit test date format" << std::endl;
}

void
testFormatDateTimeT() {
  std::cout << "Enter in test date format from std::time_t" << std::endl;

  std::string timetStr;
  formatter.format(Date(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), timetStr);
  std::cout << "Date from time_t: " << timetStr << std::endl;

  std::cout << "Exit test date format from std::time_t" << std::endl;
}

Date
parseDate(const std::string& strDate) {
  Date parsedDate(false);
  formatter.parse(strDate, parsedDate);
  return parsedDate;
}

void
testParseDate() {
  std::cout << "Enter in test date parse" << std::endl;

  std::string strDate = "2012-10-23 10:02:59.998";
  Date parsedDate = parseDate(strDate);
  std::string parsedDateStr;
  formatter.format(parsedDate, parsedDateStr);
  anch::ut::assertThat(strDate == parsedDateStr, "Parse {} instrad of {}", parsedDateStr, strDate);
  std::cout << "Parsed date: " << parsedDateStr << std::endl;

  std::cout << "Exit test date parse" << std::endl;
}

void
testComparison() {
  std::cout << "Enter in test comparison" << std::endl;

  Date now = Date();
  std::string nowStr;
  formatter.format(now, nowStr);
  std::string strDate = "2012-10-23 10:02:59.998";
  Date parsedDate = parseDate(strDate);
  anch::ut::assertThat(now > parsedDate, "{} should be after {}", nowStr, strDate);
  std::cout << nowStr << " is after " << strDate << std::endl;

  std::cout << "Exit test comparison" << std::endl;
}

void
testFormatDateCustom() {
  std::cout << "Enter in custom formatter test" << std::endl;

  std::string nowStr;
  Date now = Date();
  DateFormatter formatter2("%Y-%m-%d %H:%M:%S.%s.%v.%N");
  formatter2.format(now, nowStr);
  std::cout << "Date from custom parser: " << nowStr << std::endl;

  std::cout << "Exit custom formatter test" << std::endl;
}

// Tests below are run with TZ=Europe/Paris (CET in winter, CEST in summer)
const std::time_t SUMMER_NOON = 1751364000; // 2025-07-01 10:00:00 UTC = 12:00 CEST
const std::time_t WINTER = 1735725600;      // 2025-01-01 10:00:00 UTC = 11:00 CET
const std::time_t SUMMER_MIDNIGHT = 1751320800; // 2025-06-30 22:00:00 UTC = 2025-07-01 00:00 CEST

std::string
format(const DateFormatter& fmt, const Date& date) {
  std::string str;
  fmt.format(date, str);
  return str;
}

void
testDaylightSavingTime() {
  // D3: daylight saving time was added twice
  std::string str = format(formatter, Date(SUMMER_NOON));
  anch::ut::assertThat(str == "2025-07-01 12:00:00.000", "Summer date is {}", str);
  str = format(formatter, Date(WINTER));
  anch::ut::assertThat(str == "2025-01-01 11:00:00.000", "Winter date is {}", str);
  str = format(formatter, Date(std::chrono::system_clock::from_time_t(SUMMER_MIDNIGHT)));
  anch::ut::assertThat(str == "2025-07-01 00:00:00.000", "Summer midnight is {}", str);
}

void
testTimespec() {
  // D4: milliseconds were computed with a modulo
  DateFormatter fmt("%Y-%m-%d %H:%M:%S.%s.%v.%N");
  std::timespec spec{SUMMER_NOON, 123456789};
  Date date(spec);
  std::string str = format(fmt, date);
  anch::ut::assertThat(str == "2025-07-01 12:00:00.123.456.789", "timespec date is {}", str);
  std::timespec back = static_cast<std::timespec>(date);
  anch::ut::assertThat(back.tv_sec == spec.tv_sec && back.tv_nsec == spec.tv_nsec, "timespec is {}.{}", back.tv_sec, back.tv_nsec);
  anch::ut::assertThat(static_cast<std::time_t>(date) == SUMMER_NOON, "time_t is {}", static_cast<std::time_t>(date));
  auto tp = static_cast<std::chrono::system_clock::time_point>(date);
  anch::ut::assertTrue(Date(tp) == date, "time_point round trip");
}

void
testConversions() {
  Date date(SUMMER_NOON);
  std::tm local = static_cast<std::tm>(date);
  anch::ut::assertThat(local.tm_hour == 12 && local.tm_isdst > 0 && local.tm_wday == 2 && local.tm_yday == 181,
		       "tm: {}h isdst={} wday={} yday={}", local.tm_hour, local.tm_isdst, local.tm_wday, local.tm_yday);
  // D2: tm_isdst was not initialized ; -1 lets mktime determine it
  std::tm input{};
  input.tm_year = 125;
  input.tm_mon = 6;
  input.tm_mday = 1;
  input.tm_hour = 12;
  input.tm_isdst = -1;
  anch::ut::assertThat(static_cast<std::time_t>(Date(&input)) == SUMMER_NOON, "tm date is {}", format(formatter, Date(&input)));
  Date epoch(false);
  anch::ut::assertThat(static_cast<std::time_t>(epoch) == 0, "Date(false) should be epoch: {}", format(formatter, epoch));
}

void
testYear2Digits() {
  // D5: %y was written with 4 digits
  DateFormatter fmt("%y-%m-%d");
  std::string str = format(fmt, Date(SUMMER_NOON));
  anch::ut::assertThat(str == "25-07-01", "%y date is {}", str);
  Date date = fmt.parse("25-07-01");
  anch::ut::assertThat(format(formatter, date) == "2025-07-01 00:00:00.000", "25 parsed as {}", format(formatter, date));
  date = fmt.parse("70-01-02");
  anch::ut::assertThat(format(formatter, date) == "1970-01-02 00:00:00.000", "70 parsed as {}", format(formatter, date));
}

void
testHour12() {
  // D6: noon and midnight were written 00
  DateFormatter fmt("%h:%M %p");
  std::string str = format(fmt, Date(SUMMER_NOON));
  anch::ut::assertThat(str == "12:00 p.m.", "Noon is {}", str);
  str = format(fmt, Date(SUMMER_MIDNIGHT));
  anch::ut::assertThat(str == "12:00 a.m.", "Midnight is {}", str);
  str = format(fmt, Date(SUMMER_NOON + 3600 + 60));
  anch::ut::assertThat(str == "01:01 p.m.", "13:01 is {}", str);
  DateFormatter full("%Y-%m-%d %h:%M:%S %p");
  for(const char* value : {"2025-07-01 12:00:00 a.m.", "2025-07-01 12:00:00 p.m.", "2025-07-01 01:30:00 p.m.", "2025-07-01 11:59:59 p.m."}) {
    Date date = full.parse(value);
    anch::ut::assertThat(format(full, date) == value, "{} parsed as {}", value, format(full, date));
  }
  anch::ut::assertThat(format(formatter, full.parse("2025-07-01 12:00:00 a.m.")) == "2025-07-01 00:00:00.000", "12 a.m. should be midnight");
}

void
testParseReset() {
  // Fields which are not in pattern were kept from the parsed date (current time for Date())
  DateFormatter fmt("%Y-%m-%d");
  Date date; // now
  fmt.parse("2012-10-23", date);
  std::string str = format(formatter, date);
  anch::ut::assertThat(str == "2012-10-23 00:00:00.000", "Parsed date is {}", str);
}

void
testParseMilliseconds() {
  // Milliseconds were ignored in timestamp
  Date first = formatter.parse("2012-10-23 10:02:59.001");
  Date second = formatter.parse("2012-10-23 10:02:59.999");
  anch::ut::assertTrue(first < second && first != second, "Milliseconds should be compared");
  anch::ut::assertThat(static_cast<std::time_t>(first) == 1350979379, "Parsed time_t is {}", static_cast<std::time_t>(first));
}

void
testParseInvalid() {
  for(const char* value : {"2025-02-30 10:00:00.000", "2025-13-01 10:00:00.000", "2025-00-10 10:00:00.000",
			   "2025-1a-01 10:00:00.000", "2025-01-01 24:00:00.000", "2025-01-01 10:60:00.000",
			   "2025-01-01 10:00:00.-01", "2025-01-01 10:00:00+000", "2025/01/01 10:00:00.000",
			   "2025-01-01 10:00:00.0000", " 2025-01-01 10:00:00.00",
			   // Out of nanoseconds on 64 bits range (signed overflow found by fuzzing)
			   "1013-11-16 20:41:20.123", "9999-12-31 23:59:59.999", "0000-01-01 00:00:00.000"}) {
    bool raised = false;
    try {
      formatter.parse(value);
    } catch(const std::invalid_argument&) {
      raised = true;
    }
    anch::ut::assertThat(raised, "{} should be refused", value);
  }
  formatter.parse("2024-02-29 10:00:00.000"); // leap year
  formatter.parse("1700-01-01 00:00:00.000"); // range limits
  formatter.parse("2261-12-31 23:59:59.999");
}

void
testFormatterOwnership() {
  // Parts were deleted by each copy (double free)
  static_assert(!std::is_copy_constructible_v<DateFormatter>, "DateFormatter has to be non copyable");
  DateFormatter fmt("%Y");
  DateFormatter moved(std::move(fmt));
  anch::ut::assertThat(format(moved, Date(SUMMER_NOON)) == "2025", "Moved formatter result is {}", format(moved, Date(SUMMER_NOON)));
}

void
testUnknownPattern() {
  DateFormatter fmt("100%% %Q %Y%");
  std::string str = format(fmt, Date(SUMMER_NOON));
  anch::ut::assertThat(str == "100%% %Q 2025%", "Unknown pattern is written as is: {}", str);
}

void
testOutOfRange() {
  // Timestamps are stored in nanoseconds: dates out of years 1678 to 2261 are refused instead of aborting (-ftrapv)
  for(std::time_t secs : {static_cast<std::time_t>(20'000'000'000LL), static_cast<std::time_t>(-20'000'000'000LL)}) {
    try {
      Date date(secs);
      anch::ut::fail("time_t out of range should be refused");
    } catch(const std::invalid_argument& e) {
      std::cout << e.what() << std::endl;
    }
    try {
      Date date(std::timespec{secs, 0});
      anch::ut::fail("timespec out of range should be refused");
    } catch(const std::invalid_argument& e) {
      std::cout << e.what() << std::endl;
    }
  }
  for(long nsec : {-1L, 1'000'000'000L}) {
    try {
      Date date(std::timespec{0, nsec});
      anch::ut::fail("Invalid tv_nsec should be refused");
    } catch(const std::invalid_argument& e) {
      std::cout << e.what() << std::endl;
    }
  }
  // Bounds are accepted
  Date max(static_cast<std::time_t>(9'223'372'035LL));
  Date min(std::timespec{static_cast<std::time_t>(-9'223'372'035LL), 999'999'999L});
  anch::ut::assertThat(static_cast<std::time_t>(max) == 9'223'372'035LL, "Maximum date should be kept");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  DateFormatter::registerFormatterPart(NanoFormatter::PATTERN, &NanoFormatter::getInstance);
  DateFormatter::registerFormatterPart(MicroFormatter::PATTERN, &MicroFormatter::getInstance);
  tests
    .name("AnCH date unit tests")
    .description("Test AnCH date library")
    .add("date-format", testFormatDate)
    .add("date-format-time_t", testFormatDateTimeT)
    .add("date-parse", testParseDate)
    .add("date-comparison", testComparison)
    .add("date-format-custom", testFormatDateCustom)
    .add("date-dst", testDaylightSavingTime)
    .add("date-timespec", testTimespec)
    .add("date-conversions", testConversions)
    .add("date-year-2-digits", testYear2Digits)
    .add("date-hour-12", testHour12)
    .add("date-parse-reset", testParseReset)
    .add("date-parse-milliseconds", testParseMilliseconds)
    .add("date-parse-invalid", testParseInvalid)
    .add("date-formatter-ownership", testFormatterOwnership)
    .add("date-unknown-pattern", testUnknownPattern)
    .add("date-out-of-range", testOutOfRange)
    ;
}
