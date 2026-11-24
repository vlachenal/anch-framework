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
#include "date/time.hpp"

#include <ctime>
#include <format>
#include <stdexcept>

#include "date/rfc3339.hpp"

using anch::date::Time;
using anch::date::Zone;
using std::chrono::nanoseconds;

namespace {

  /*! Nanoseconds of a day */
  constexpr int64_t DAY = 86'400'000'000'000;

}

// Constructors +
Time::Time(unsigned hour, unsigned minute, unsigned second, uint32_t nanosecond): _time(0) {
  if(hour > 23 || minute > 59 || second > 59 || nanosecond > 999'999'999) {
    throw std::invalid_argument(std::format("Invalid time {:02}:{:02}:{:02}.{:09} (times are from 00:00:00 to 23:59:59.999999999)",
					    hour, minute, second, nanosecond));
  }
  _time = std::chrono::hours(hour) + std::chrono::minutes(minute) + std::chrono::seconds(second) + nanoseconds(nanosecond);
}

Time::Time(nanoseconds sinceMidnight): _time(sinceMidnight) {
  if(sinceMidnight.count() < 0 || sinceMidnight.count() >= DAY) {
    throw std::invalid_argument("Invalid time: duration since midnight has to be in [0, 24h)");
  }
}
// Constructors -

// Methods +
Time
Time::now(Zone zone) {
  const auto now = std::chrono::time_point_cast<nanoseconds>(std::chrono::system_clock::now());
  if(zone == Zone::UTC) {
    return Time(now - std::chrono::floor<std::chrono::days>(now));
  }
  std::time_t secs = std::chrono::system_clock::to_time_t(std::chrono::floor<std::chrono::seconds>(now));
  std::tm local{};
  ::localtime_r(&secs, &local);
  return Time(static_cast<unsigned>(local.tm_hour), static_cast<unsigned>(local.tm_min), static_cast<unsigned>(local.tm_sec % 60),
	      static_cast<uint32_t>((now - std::chrono::floor<std::chrono::seconds>(now)).count()));
}

Time
Time::parse(std::string_view text) {
  return anch::date::rfc3339::parseTime(text);
}

std::string
Time::format() const {
  return anch::date::rfc3339::formatTime(*this);
}

Time
Time::plus(nanoseconds duration) const noexcept {
  int64_t value = (_time.count() + duration.count() % DAY) % DAY;
  if(value < 0) {
    value += DAY;
  }
  return Time(nanoseconds(value));
}
// Methods -

std::ostream&
anch::date::operator<<(std::ostream& out, const Time& time) {
  char buffer[anch::date::rfc3339::TIME_MAX_SIZE];
  return out.write(buffer, static_cast<std::streamsize>(anch::date::rfc3339::formatTime(time, buffer)));
}
