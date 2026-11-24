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

  class DateFormatter;
  class IDatePartFormatter;

  /*!
   * \brief DateTime and time utility class
   *
   * A \ref DateTime is a point in time with a nanosecond precision (stored as a number of nanoseconds since epoch, which
   * covers years 1678 to 2261), and its broken down representation in local time (year, month, day, hour, minute,
   * second, millisecond, microsecond and nanosecond).\n
   * Local time is computed with \c localtime_r (\c std::chrono time zones are not available with GCC 13): it is thread
   * safe and follows the \c TZ environment variable.\n
   * \ref DateTime can be converted from and to \c std::chrono::system_clock::time_point , \c std::time_t , \c std::tm and
   * \c std::timespec .
   *
   * \author Vincent Lachenal
   */
  class ANCH_DATE_EXPORT DateTime {

    friend class IDatePartFormatter;
    friend class DateFormatter;

    // Attributes +
  protected:
    /*! The timestamp (nanoseconds since epoch) */
    std::int64_t _timestamp = 0;

    /*! The years */
    int32_t _years = 0;

    /*! The months (0-11) */
    uint16_t _months = 0;

    /*! The days of the year (0-365) */
    uint16_t _ydays = 0;

    /*! The days of the month (1-31) */
    uint16_t _mdays = 0;

    /*! The days of the week (0-6, 0 is sunday) */
    uint16_t _wdays = 0;

    /*! The hours (0-23) */
    uint16_t _hours = 0;

    /*! The minutes (0-59) */
    uint16_t _minutes = 0;

    /*! The seconds (0-60) */
    uint16_t _seconds = 0;

    /*! The milliseconds (0-999) */
    uint16_t _milliseconds = 0;

    /*! The microseconds (0-999) */
    uint16_t _microseconds = 0;

    /*! The nanoseconds (0-999) */
    uint16_t _nanoseconds = 0;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref DateTime default constructor
     *
     * \param init initialize the \ref DateTime to now (default \c true ) ; epoch otherwise
     */
    explicit DateTime(bool init = true);

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
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref DateTime destructor
     */
    virtual ~DateTime();
    // Destructor -

    // Methods +
  private:
    /*!
     * Initialize timestamp and local time fields
     *
     * \param time the time point
     */
    void initialize(const std::chrono::sys_time<std::chrono::nanoseconds>& time);

    /*!
     * Compute timestamp from local time fields (and normalize fields)
     *
     * \throw std::invalid_argument date out of range (years 1678 to 2261)
     */
    void computeTimestamp();

  public:
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
     * \param zone the time zone of the day (default local time, as the fields of the date)
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
