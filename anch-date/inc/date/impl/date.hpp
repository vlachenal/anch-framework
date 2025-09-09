/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

namespace anch::date {

  template<typename R, typename P>
  void
  anch::date::Date::initialize(const std::chrono::duration<R,P>& duration) noexcept {
    _nanoseconds = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000);
    _microseconds = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000);
    _milliseconds = static_cast<uint16_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000);
    }

  inline
  void
  anch::date::Date::computeTm(std::tm& time) const {
    time.tm_sec = _seconds;
    time.tm_min = _minutes;
    time.tm_hour = _hours;
    time.tm_mday = _mdays;
    time.tm_mon = _months;
    time.tm_year = _years - 1900;
  }

  inline
  bool
  anch::date::Date::after(const anch::date::Date& date) const noexcept {
    return _timestamp > date._timestamp;
  }

  inline
  bool
  anch::date::Date::before(const anch::date::Date& date) const noexcept {
    return _timestamp < date._timestamp;
  }

  inline
  bool
  anch::date::Date::equals(const anch::date::Date& date) const noexcept {
    return _timestamp == date._timestamp;
  }

  inline
  bool
  anch::date::Date::operator > (const anch::date::Date& date) const noexcept {
    return after(date);
  }

  inline
  bool
  anch::date::Date::operator >= (const anch::date::Date& date) const noexcept {
    return (after(date) || equals(date));
  }

  inline
  bool
  anch::date::Date::operator < (const anch::date::Date& date) const noexcept {
    return before(date);
  }

  inline
  bool
  anch::date::Date::operator <= (const anch::date::Date& date) const noexcept {
    return (before(date) || equals(date));
  }

  inline
  bool
  anch::date::Date::operator == (const anch::date::Date& date) const noexcept {
    return equals(date);
  }

  inline
  bool
  anch::date::Date::operator != (const anch::date::Date& date) const noexcept {
    return !equals(date);
  }

  inline
  anch::date::Date::operator std::time_t() const noexcept {
    std::tm time;
    computeTm(time);
    return std::mktime(&time);
  }

  inline
  anch::date::Date::operator std::tm() const noexcept {
    std::tm time;
    computeTm(time);
    return time;
  }


  inline
  anch::date::Date::operator std::timespec() const noexcept {
    std::timespec time;
    std::tm cal;
    computeTm(cal);
    time.tv_sec = std::mktime(&cal);
    time.tv_nsec = _nanoseconds + 1000 * _microseconds + 1000000 * _milliseconds;
    return time;
  }

}
