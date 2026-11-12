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
#include "date/rfc3339.hpp"

#include <cstdint>
#include <ctime>
#include <format>
#include <limits>
#include <stdexcept>

using anch::date::Date;
using anch::date::rfc3339::Offset;
using std::chrono::days;
using std::chrono::nanoseconds;
using std::chrono::seconds;
using std::chrono::sys_days;
using std::chrono::sys_time;
using std::chrono::system_clock;
using std::chrono::year_month_day;

namespace {

  /*! Nanoseconds in one second */
  constexpr std::int64_t NANOS = 1'000'000'000;

  /*! Seconds in one day */
  constexpr std::int64_t DAY_SECONDS = 86'400;

  /*! Bound (excluded) of the seconds since epoch which can be stored in a \ref Date (nanoseconds on 64 bits) */
  constexpr std::int64_t MAX_SECONDS = std::numeric_limits<std::int64_t>::max() / NANOS;

  /*!
   * Raise parsing error
   *
   * \param kind the RFC 3339 production (\c date-time or \c full-date )
   * \param text the parsed text
   * \param reason the error reason (can be empty)
   *
   * \throw std::invalid_argument always
   */
  [[noreturn]] void
  invalid(std::string_view kind, std::string_view text, std::string_view reason = "") {
    throw std::invalid_argument(std::format("Invalid RFC 3339 {} '{}'{}", kind, text, reason));
  }

  /*!
   * Read a number of exactly \c count digits
   *
   * \param text the text
   * \param pos the number position
   * \param count the number of digits
   * \param value the read value
   *
   * \return \c true when there are \c count digits at \c pos , \c false otherwise
   */
  bool
  readNumber(std::string_view text, std::size_t pos, std::size_t count, int& value) noexcept {
    if(pos + count > text.size()) {
      return false;
    }
    value = 0;
    for(std::size_t i = pos ; i < pos + count ; ++i) {
      char digit = text[i];
      if(digit < '0' || digit > '9') {
	return false;
      }
      value = value * 10 + (digit - '0');
    }
    return true;
  }

  /*!
   * Check character
   *
   * \param text the text
   * \param pos the character position
   * \param expected the expected character
   *
   * \return \c true when \c expected is at \c pos , \c false otherwise
   */
  inline bool
  isChar(std::string_view text, std::size_t pos, char expected) noexcept {
    return pos < text.size() && text[pos] == expected;
  }

  /*!
   * Read \c full-date at the beginning of the text
   *
   * \param text the text
   * \param date the read date
   *
   * \return \c true when text starts with a valid date, \c false otherwise
   */
  bool
  readDate(std::string_view text, year_month_day& date) noexcept {
    int year = 0;
    int month = 0;
    int day = 0;
    if(!readNumber(text, 0, 4, year) || !isChar(text, 4, '-')
       || !readNumber(text, 5, 2, month) || !isChar(text, 7, '-')
       || !readNumber(text, 8, 2, day)) {
      return false;
    }
    date = year_month_day(std::chrono::year(year),
			  std::chrono::month(static_cast<unsigned>(month)),
			  std::chrono::day(static_cast<unsigned>(day)));
    return date.ok();
  }

}

// date-time +
std::size_t
anch::date::rfc3339::formatDateTime(const Date& date, char* output, Offset offset) noexcept {
  auto time = std::chrono::time_point_cast<nanoseconds>(static_cast<system_clock::time_point>(date));
  std::int64_t shift = 0; // seconds
  if(offset == Offset::LOCAL) {
    std::time_t secs = system_clock::to_time_t(std::chrono::floor<seconds>(time));
    std::tm local{};
    ::localtime_r(&secs, &local);
    shift = static_cast<std::int64_t>(local.tm_gmtoff) / 60 * 60; // RFC 3339 offsets have no seconds
  }
  auto shifted = time + seconds(shift);
  auto day = std::chrono::floor<days>(shifted);
  year_month_day ymd(day);
  std::chrono::hh_mm_ss<nanoseconds> tod(shifted - day);
  char* out = std::format_to(output, "{:04}-{:02}-{:02}T{:02}:{:02}:{:02}",
			     static_cast<int>(ymd.year()), static_cast<unsigned>(ymd.month()),
			     static_cast<unsigned>(ymd.day()), tod.hours().count(), tod.minutes().count(),
			     tod.seconds().count());
  auto subsec = tod.subseconds().count();
  if(subsec != 0) {
    if(subsec % 1'000'000 == 0) {
      out = std::format_to(out, ".{:03}", subsec / 1'000'000);
    } else if(subsec % 1'000 == 0) {
      out = std::format_to(out, ".{:06}", subsec / 1'000);
    } else {
      out = std::format_to(out, ".{:09}", subsec);
    }
  }
  if(offset == Offset::UTC) {
    *out++ = 'Z';
  } else {
    std::int64_t mins = (shift < 0 ? -shift : shift) / 60;
    out = std::format_to(out, "{}{:02}:{:02}", (shift < 0 ? '-' : '+'), mins / 60, mins % 60);
  }
  return static_cast<std::size_t>(out - output);
}

std::string
anch::date::rfc3339::formatDateTime(const Date& date, Offset offset) {
  char buffer[DATE_TIME_MAX_SIZE];
  return std::string(buffer, formatDateTime(date, buffer, offset));
}

Date
anch::date::rfc3339::parseDateTime(std::string_view text) {
  constexpr std::string_view KIND = "date-time";
  year_month_day ymd;
  int hours = 0;
  int minutes = 0;
  int secs = 0;
  if(!readDate(text, ymd) || !(isChar(text, 10, 'T') || isChar(text, 10, 't'))
     || !readNumber(text, 11, 2, hours) || !isChar(text, 13, ':')
     || !readNumber(text, 14, 2, minutes) || !isChar(text, 16, ':')
     || !readNumber(text, 17, 2, secs)
     || hours > 23 || minutes > 59 || secs > 60) {
    invalid(KIND, text);
  }
  std::size_t pos = 19;
  // Fraction (digits after nanosecond are truncated) +
  std::int64_t subsec = 0;
  if(isChar(text, pos, '.')) {
    std::size_t start = ++pos;
    std::int64_t scale = NANOS;
    while(pos < text.size() && text[pos] >= '0' && text[pos] <= '9') {
      if(scale > 1) {
	scale /= 10;
	subsec += (text[pos] - '0') * scale;
      }
      ++pos;
    }
    if(pos == start) {
      invalid(KIND, text);
    }
  }
  // Fraction -
  // Offset +
  std::int64_t shift = 0; // seconds
  if(isChar(text, pos, 'Z') || isChar(text, pos, 'z')) {
    ++pos;
  } else if(isChar(text, pos, '+') || isChar(text, pos, '-')) {
    int offHours = 0;
    int offMinutes = 0;
    if(!readNumber(text, pos + 1, 2, offHours) || !isChar(text, pos + 3, ':')
       || !readNumber(text, pos + 4, 2, offMinutes) || offHours > 23 || offMinutes > 59) {
      invalid(KIND, text);
    }
    shift = (offHours * 3600 + offMinutes * 60) * (text[pos] == '-' ? -1 : 1);
    pos += 6;
  } else {
    invalid(KIND, text);
  }
  if(pos != text.size()) {
    invalid(KIND, text);
  }
  // Offset -
  std::int64_t utc = static_cast<std::int64_t>(sys_days(ymd).time_since_epoch().count()) * DAY_SECONDS
    + hours * 3600 + minutes * 60 + secs - shift;
  if(secs == 60 && ((utc - 1) % DAY_SECONDS + DAY_SECONDS) % DAY_SECONDS != DAY_SECONDS - 1) {
    invalid(KIND, text, ": leap second is only allowed at 23:59 UTC");
  }
  if(utc < -MAX_SECONDS || utc >= MAX_SECONDS) {
    invalid(KIND, text, ": out of range (years 1678 to 2261)");
  }
  sys_time<nanoseconds> time{nanoseconds(utc * NANOS + subsec)};
  return Date(std::chrono::time_point_cast<system_clock::duration>(time));
}
// date-time -

// full-date +
std::size_t
anch::date::rfc3339::formatDate(const year_month_day& date, char* output) {
  int year = static_cast<int>(date.year());
  unsigned month = static_cast<unsigned>(date.month());
  unsigned day = static_cast<unsigned>(date.day());
  if(!date.ok() || year < 0 || year > 9999) {
    throw std::invalid_argument(std::format("Invalid RFC 3339 full-date {:04}-{:02}-{:02}", year, month, day));
  }
  std::format_to(output, "{:04}-{:02}-{:02}", year, month, day);
  return DATE_SIZE;
}

std::string
anch::date::rfc3339::formatDate(const year_month_day& date) {
  char buffer[DATE_SIZE];
  return std::string(buffer, formatDate(date, buffer));
}

year_month_day
anch::date::rfc3339::parseDate(std::string_view text) {
  year_month_day date;
  if(text.size() != DATE_SIZE || !readDate(text, date)) {
    invalid("full-date", text);
  }
  return date;
}
// full-date -
