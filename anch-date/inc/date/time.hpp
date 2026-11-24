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

#include "anch-date-export.hpp"

#include <chrono>
#include <compare>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

#include "date/date.hpp"

namespace anch::date {

  /*!
   * \brief Time of day (RFC 3339 \c partial-time , SQL \c TIME , XSD \c xs:time without time zone)
   *
   * A \ref Time is a time of day with a nanosecond precision, from 00:00:00 to 23:59:59.999999999, without time zone:
   * it is always valid (impossible times are refused when it is built ; leap seconds can not be represented without
   * a day). It is not a point in time ( \ref DateTime ): conversions between them give the time zone
   * ( \ref DateTime::time , \ref Date::at ).
   * \snippet{trimleft} anch-date/examples/date.cpp time
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  class ANCH_DATE_EXPORT Time {

    // Attributes +
  private:
    /*! Nanoseconds since midnight */
    std::chrono::nanoseconds _time;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Time default constructor: midnight (00:00:00)
     */
    constexpr Time() noexcept;

    /*!
     * \ref Time constructor
     *
     * \param hour the hour (0 to 23)
     * \param minute the minute (0 to 59)
     * \param second the second (0 to 59)
     * \param nanosecond the nanoseconds of the second (0 to 999999999)
     *
     * \throw std::invalid_argument impossible time
     */
    Time(unsigned hour, unsigned minute, unsigned second = 0, uint32_t nanosecond = 0);

    /*!
     * \ref Time constructor
     *
     * \param sinceMidnight the duration since midnight (less than 24 hours)
     *
     * \throw std::invalid_argument negative duration or duration of 24 hours or more
     */
    explicit Time(std::chrono::nanoseconds sinceMidnight);
    // Constructors -

    // Methods +
  public:
    /*!
     * Current time of day
     *
     * \param zone the time zone (default local time)
     *
     * \return the current time of day
     */
    static Time now(anch::date::Zone zone = anch::date::Zone::LOCAL);

    /*!
     * Parse RFC 3339 \c partial-time ( \c HH:MM:SS[.fraction] , without offset)
     *
     * \param text the text to parse
     *
     * \return the time
     *
     * \throw std::invalid_argument invalid format, impossible time or leap second
     */
    static Time parse(std::string_view text);

    /*!
     * Format RFC 3339 \c partial-time (fraction written with the number of digits needed: none, 3, 6 or 9)
     *
     * \return the formatted time
     */
    std::string format() const;

    /*!
     * Hour getter
     *
     * \return the hour (0 to 23)
     */
    constexpr unsigned hour() const noexcept;

    /*!
     * Minute getter
     *
     * \return the minute (0 to 59)
     */
    constexpr unsigned minute() const noexcept;

    /*!
     * Second getter
     *
     * \return the second (0 to 59)
     */
    constexpr unsigned second() const noexcept;

    /*!
     * Nanoseconds of the second getter
     *
     * \return the nanoseconds (0 to 999999999)
     */
    constexpr uint32_t nanosecond() const noexcept;

    /*!
     * Add duration: the result wraps around midnight (23:00 plus 2 hours is 01:00)
     *
     * \param duration the duration (negative to subtract)
     *
     * \return the new time
     */
    Time plus(std::chrono::nanoseconds duration) const noexcept;
    // Methods -

    // Operators +
  public:
    /*!
     * Compare times
     *
     * \param other the other time
     *
     * \return the chronological order in a day
     */
    constexpr std::strong_ordering operator<=>(const Time& other) const noexcept;

    /*!
     * Check times equality
     *
     * \param other the other time
     *
     * \return \c true when times are equal
     */
    constexpr bool operator==(const Time& other) const noexcept;

    /*!
     * Convert to the duration since midnight
     *
     * \return the duration since midnight
     */
    constexpr explicit operator std::chrono::nanoseconds() const noexcept;
    // Operators -

  };

  /*!
   * Write RFC 3339 \c partial-time
   *
   * \param out the output stream
   * \param time the time
   *
   * \return the output stream
   *
   * \since 0.3
   */
  ANCH_DATE_EXPORT std::ostream& operator<<(std::ostream& out, const anch::date::Time& time);

}  // anch::date

namespace anch::date {

  constexpr
  Time::Time() noexcept: _time(0) {
    // Nothing to do
  }

  constexpr unsigned
  Time::hour() const noexcept {
    return static_cast<unsigned>(std::chrono::duration_cast<std::chrono::hours>(_time).count());
  }

  constexpr unsigned
  Time::minute() const noexcept {
    return static_cast<unsigned>(std::chrono::duration_cast<std::chrono::minutes>(_time).count() % 60);
  }

  constexpr unsigned
  Time::second() const noexcept {
    return static_cast<unsigned>(std::chrono::duration_cast<std::chrono::seconds>(_time).count() % 60);
  }

  constexpr uint32_t
  Time::nanosecond() const noexcept {
    return static_cast<uint32_t>(_time.count() % 1'000'000'000);
  }

  constexpr std::strong_ordering
  Time::operator<=>(const Time& other) const noexcept {
    return _time <=> other._time;
  }

  constexpr bool
  Time::operator==(const Time& other) const noexcept {
    return _time == other._time;
  }

  constexpr
  Time::operator std::chrono::nanoseconds() const noexcept {
    return _time;
  }

}  // anch::date
