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

  inline
  int32_t
  anch::date::IDatePartFormatter::getYear(const anch::date::Date& date) {
    return date._years;
  }

  inline
  void
  anch::date::IDatePartFormatter::setYear(anch::date::Date& date, int32_t year) {
    date._years = year;
  }

  inline
  uint16_t
  anch::date::IDatePartFormatter::getMonth(const anch::date::Date& date) {
    return date._months;
  }

  inline
  void
  anch::date::IDatePartFormatter::setMonth(anch::date::Date& date, uint16_t month) {
    date._months = month;
  }

  inline
  uint16_t
  anch::date::IDatePartFormatter::getDay(const anch::date::Date& date) {
    return date._mdays;
  }

  inline
  void
  anch::date::IDatePartFormatter::setDay(anch::date::Date& date, uint16_t mday) {
    date._mdays = mday;
  }

  inline
  uint16_t
  anch::date::IDatePartFormatter::getHour(const anch::date::Date& date) {
    return date._hours;
  }

  inline
  void
  anch::date::IDatePartFormatter::setHour(anch::date::Date& date, uint16_t hour) {
    date._hours = hour;
  }

  inline
  uint16_t
  anch::date::IDatePartFormatter::getMinute(const anch::date::Date& date) {
    return date._minutes;
  }

  inline
  void
  anch::date::IDatePartFormatter::setMinute(anch::date::Date& date, uint16_t minute) {
    date._minutes = minute;
  }

  inline
  uint16_t
  anch::date::IDatePartFormatter::getSecond(const anch::date::Date& date) {
    return date._seconds;
  }

  inline
  void
  anch::date::IDatePartFormatter::setSecond(anch::date::Date& date, uint16_t second) {
    date._seconds = second;
  }

  inline
  uint16_t
  anch::date::IDatePartFormatter::getMillisecond(const anch::date::Date& date) {
    return date._milliseconds;
  }

  inline
  void
  anch::date::IDatePartFormatter::setMillisecond(anch::date::Date& date, uint16_t milli) {
    date._milliseconds = milli;
  }

  inline
  uint16_t
  anch::date::IDatePartFormatter::getMicrosecond(const anch::date::Date& date) {
    return date._microseconds;
  }

  inline
  void
  anch::date::IDatePartFormatter::setMicrosecond(anch::date::Date& date, uint16_t micro) {
    date._microseconds = micro;
  }

  inline
  uint16_t
  anch::date::IDatePartFormatter::getNanosecond(const anch::date::Date& date) {
    return date._nanoseconds;
  }

  inline
  void
  anch::date::IDatePartFormatter::setNanosecond(anch::date::Date& date, uint16_t nano) {
    date._nanoseconds = nano;
  }

}
