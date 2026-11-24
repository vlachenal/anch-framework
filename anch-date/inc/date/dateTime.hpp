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
#pragma once

#include "anch-date-export.hpp"

#include <chrono>
#include <compare>
#include <cstdint>
#include <ctime>

#include "date/date.hpp"


namespace anch::date {

  /*!
   * \brief Broken down date and time ( \ref DateTime::fields ): local time or UTC
   *
   * \since 0.3
   */
  struct ANCH_DATE_EXPORT DateFields {
    /*! The year */
    int32_t year = 1970;

    /*! The month (1-12) */
    uint16_t month = 1;

    /*! The day of the month (1-31) */
    uint16_t day = 1;

    /*! The hour (0-23) */
    uint16_t hour = 0;

    /*! The minute (0-59) */
    uint16_t minute = 0;

    /*! The second (0-60) */
    uint16_t second = 0;

    /*! The millisecond (0-999) */
    uint16_t millisecond = 0;

    /*! The microsecond (0-999) */
    uint16_t microsecond = 0;

    /*! The nanosecond (0-999) */
    uint16_t nanosecond = 0;

    /*! The day of the week (ISO 8601: 1 is monday, 7 is sunday ; ignored to build a \ref DateTime ) */
    uint16_t weekday = 4;

    /*! The day of the year (1-366 ; ignored to build a \ref DateTime ) */
    uint16_t yearday = 1;
  };

  /*!
   * \brief Point in time
   *
   * A \ref DateTime is a point in time with a nanosecond precision, stored as a number of nanoseconds since epoch (which
   * covers years 1678 to 2261). Its broken down representation ( \ref fields ) is computed in local time or UTC: local
   * time is computed with \c localtime_r (\c std::chrono time zones are not available with GCC 13): it is thread safe
   * and follows the \c TZ environment variable.\n
   * A default \ref DateTime is epoch: the current time is \ref now . \ref DateTime can be converted from and to
   * \c std::chrono::system_clock::time_point , \c std::time_t , \c std::tm and \c std::timespec . Its day is a
   * \ref Date ( \ref date ).
   *
   * \author Vincent Lachenal
   */
  class ANCH_DATE_EXPORT DateTime {

    // Attributes +
  private:
    /*! The point in time */
    std::chrono::sys_time<std::chrono::nanoseconds> _time;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref DateTime default constructor: epoch (the current time is \ref now )
     */
    constexpr DateTime() noexcept;

    /*!
     * \ref DateTime constructor
     *
     * \param time the time point to set
     *
     * \since 0.2
     */
    explicit DateTime(const std::chrono::system_clock::time_point& time);

    /*!
     * \ref DateTime constructor
     *
     * \param time the time to set
     *
     * \throw std::invalid_argument date out of range (years 1678 to 2261: timestamps are stored in nanoseconds)
     */
    explicit DateTime(const std::time_t& time);

    /*!
     * \ref DateTime constructor
     *
     * \param time the local time to set (\c tm_isdst has to be \c -1 when daylight saving time is unknown ; \c tm_wday
     *        and \c tm_yday are ignored ; out of range fields are normalized as \c std::mktime does)
     *
     * \throw std::invalid_argument date out of range (years 1678 to 2261)
     */
    explicit DateTime(const std::tm* const time);

    /*!
     * \ref DateTime constructor
     *
     * \param time the time to set (\c tv_nsec in [0, 999999999])
     *
     * \throw std::invalid_argument date out of range (years 1678 to 2261) or invalid \c tv_nsec
     */
    explicit DateTime(const std::timespec& time);

    /*!
     * \ref DateTime constructor
     *
     * \param fields the local time (out of range fields are normalized as \c std::mktime does ; \c weekday and
     *        \c yearday are ignored)
     *
     * \throw std::invalid_argument date out of range (years 1678 to 2261)
     *
     * \since 0.3
     */
    explicit DateTime(const anch::date::DateFields& fields);
    // Constructors -

    // Methods +
  public:
    /*!
     * Current point in time
     *
     * \return now
     *
     * \since 0.3
     */
    static DateTime now();

    /*!
     * Broken down date and time
     *
     * \param zone the time zone (default local time)
     *
     * \return the fields
     *
     * \since 0.3
     */
    anch::date::DateFields fields(anch::date::Zone zone = anch::date::Zone::LOCAL) const;

    /*!
     * Check if current \ref DateTime is after another one.
     *
     * \param date the other \ref DateTime
     *
     * \return \c true if current \ref DateTime is after the other one, \c false otherwise.
     */
    bool after(const DateTime& date) const noexcept;

    /*!
     * Check if current \ref DateTime is before another one.
     *
     * \param date the other \ref DateTime
     *
     * \return \c true if current \ref DateTime is before the other one, \c false otherwise.
     */
    bool before(const DateTime& date) const noexcept;

    /*!
     * Check if current \ref DateTime is equals to another one.
     *
     * \param date the other \ref DateTime
     *
     * \return \c true if current \ref DateTime is equals to the other one, \c false otherwise.
     */
    bool equals(const DateTime& date) const noexcept;

    /*!
     * Day of this point in time
     *
     * \param zone the time zone of the day (default local time)
     *
     * \return the day
     *
     * \since 0.3
     */
    anch::date::Date date(anch::date::Zone zone = anch::date::Zone::LOCAL) const;
    // Methods -

    // Operators +
  public:
    /*!
     * Compare dates (with nanosecond precision)
     *
     * \param date the date to compare to
     *
     * \return the comparison result
     */
    std::strong_ordering operator<=>(const DateTime& date) const noexcept;

    /*!
     * Check if dates are equals (with nanosecond precision)
     *
     * \param date the date to compare to
     *
     * \return \c true when dates are equals, \c false otherwise
     */
    bool operator==(const DateTime& date) const noexcept;

    /*!
     * Cast operator to \c std::chrono::system_clock::time_point
     *
     * \return the time point
     *
     * \since 0.2
     */
    explicit operator std::chrono::system_clock::time_point() const noexcept;

    /*!
     * Cast operator to \c std::time_t (sub-second part is truncated toward the past)
     *
     * \return the converted \c std::time_t
     */
    explicit operator std::time_t() const noexcept;

    /*!
     * Cast operator to \c std::tm (local time, with \c tm_wday , \c tm_yday and \c tm_isdst )
     *
     * \return the converted \c std::tm
     */
    explicit operator std::tm() const noexcept;

    /*!
     * Cast operator to \c std::timespec
     *
     * \return the converted \c std::timespec
     */
    explicit operator std::timespec() const noexcept;
    // Operators -

  };

} // anch::date

#include "date/impl/dateTime.hpp"
