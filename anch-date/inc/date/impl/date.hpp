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
#pragma once

namespace anch::date {

  constexpr
  Date::Date() noexcept: _date(std::chrono::year(1970), std::chrono::January, std::chrono::day(1)) {
    // Nothing to do
  }

  constexpr int32_t
  Date::year() const noexcept {
    return static_cast<int32_t>(static_cast<int>(_date.year()));
  }

  constexpr unsigned
  Date::month() const noexcept {
    return static_cast<unsigned>(_date.month());
  }

  constexpr unsigned
  Date::day() const noexcept {
    return static_cast<unsigned>(_date.day());
  }

  constexpr bool
  Date::isLeapYear() const noexcept {
    return _date.year().is_leap();
  }

  constexpr std::strong_ordering
  Date::operator<=>(const Date& other) const noexcept {
    return _date <=> other._date;
  }

  constexpr bool
  Date::operator==(const Date& other) const noexcept {
    return _date == other._date;
  }

  constexpr
  Date::operator std::chrono::year_month_day() const noexcept {
    return _date;
  }

}  // anch::date
