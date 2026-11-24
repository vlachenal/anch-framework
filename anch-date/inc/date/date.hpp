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

namespace anch::date {

  class DateTime;
  class Time;

  /*!
   * \brief Time zone of conversions between days and points in time
   *
   * \since 0.3
   */
  enum class Zone {
    /*! UTC */
    UTC,
    /*! Local time (follows \c TZ environment variable) */
    LOCAL
  };

  /*!
   * \brief Calendar day (RFC 3339 \c full-date , JSON schema and OpenAPI \c date format, XSD \c xs:date )
   *
   * A \ref Date is a day of the proleptic Gregorian calendar, without time zone, from 0000-01-01 to 9999-12-31: it is
   * always valid (impossible dates are refused when it is built), so that formatting can not fail. It is not a point in
   * time ( \ref DateTime ): conversions between them give the time zone ( \ref DateTime::date , \ref startOfDay ).\n
   * Dates are ordered (comparison operators), days, months and years can be added, and dates are converted from and to
   * \c std::chrono::year_month_day and \c std::chrono::sys_days :
   * \snippet{trimleft} anch-date/examples/date.cpp date
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  class ANCH_DATE_EXPORT Date {

    // Attributes +
  private:
    /*! The day */
    std::chrono::year_month_day _date;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Date default constructor: 1970-01-01
     */
    constexpr Date() noexcept;

    /*!
     * \ref Date constructor
     *
     * \param year the year (0 to 9999)
     * \param month the month (1 to 12)
     * \param day the day of the month (1 to 31)
     *
     * \throw std::invalid_argument impossible date or year out of range
     */
    Date(int32_t year, unsigned month, unsigned day);

    /*!
     * \ref Date constructor
     *
     * \param date the day
     *
     * \throw std::invalid_argument impossible date ( \c ok() is \c false ) or year out of range
     */
    explicit Date(const std::chrono::year_month_day& date);

    /*!
     * \ref Date constructor
     *
     * \param days the days since epoch
     *
     * \throw std::invalid_argument year out of range
     */
    explicit Date(const std::chrono::sys_days& days);
    // Constructors -

    // Methods +
  public:
    /*!
     * Current day
     *
     * \param zone the time zone of the day (default local time)
     *
     * \return the current day
     */
    static Date today(anch::date::Zone zone = anch::date::Zone::LOCAL);

    /*!
     * Parse RFC 3339 \c full-date ( \c YYYY-MM-DD )
     *
     * \param text the text to parse
     *
     * \return the date
     *
     * \throw std::invalid_argument invalid format or impossible date
     */
    static Date parse(std::string_view text);

    /*!
     * Format RFC 3339 \c full-date ( \c YYYY-MM-DD )
     *
     * \return the formatted date
     */
    std::string format() const;

    /*!
     * Year getter
     *
     * \return the year (0 to 9999)
     */
    constexpr int32_t year() const noexcept;

    /*!
     * Month getter
     *
     * \return the month (1 to 12)
     */
    constexpr unsigned month() const noexcept;

    /*!
     * Day of the month getter
     *
     * \return the day of the month (1 to 31)
     */
    constexpr unsigned day() const noexcept;

    /*!
     * Day of the week (ISO 8601)
     *
     * \return the day of the week (1 is monday, 7 is sunday)
     */
    unsigned weekday() const noexcept;

    /*!
     * Day of the year
     *
     * \return the day of the year (1 to 366)
     */
    unsigned dayOfYear() const noexcept;

    /*!
     * Check leap year
     *
     * \return \c true when the year of the date is a leap year
     */
    constexpr bool isLeapYear() const noexcept;

    /*!
     * Add days
     *
     * \param days the number of days (negative to subtract)
     *
     * \return the new date
     *
     * \throw std::invalid_argument year out of range
     */
    Date plusDays(int64_t days) const;

    /*!
     * Add months: the day is the last day of the new month when the month is shorter (2024-01-31 plus 1 month is
     * 2024-02-29)
     *
     * \param months the number of months (negative to subtract)
     *
     * \return the new date
     *
     * \throw std::invalid_argument year out of range
     */
    Date plusMonths(int64_t months) const;

    /*!
     * Add years: February 29 is February 28 when the new year is not a leap year
     *
     * \param years the number of years (negative to subtract)
     *
     * \return the new date
     *
     * \throw std::invalid_argument year out of range
     */
    Date plusYears(int64_t years) const;

    /*!
     * Number of days to another date
     *
     * \param other the other date
     *
     * \return the number of days from this date to \c other (negative when \c other is before)
     */
    int64_t daysUntil(const Date& other) const noexcept;

    /*!
     * Point in time of the start of the day (midnight ; the first existing time when a daylight saving time change
     * skips midnight)
     *
     * \param zone the time zone (default local time)
     *
     * \return the point in time
     *
     * \throw std::invalid_argument point in time out of \ref DateTime range (years 1678 to 2261)
     */
    anch::date::DateTime startOfDay(anch::date::Zone zone = anch::date::Zone::LOCAL) const;

    /*!
     * Point in time of a time of this day
     *
     * \param time the time of day
     * \param zone the time zone (default local time ; a local time which does not exist because of a daylight saving
     *        time change is normalized as \c std::mktime does)
     *
     * \return the point in time
     *
     * \throw std::invalid_argument point in time out of \ref DateTime range (years 1678 to 2261)
     *
     * \since 0.3
     */
    anch::date::DateTime at(const anch::date::Time& time, anch::date::Zone zone = anch::date::Zone::LOCAL) const;
    // Methods -

    // Operators +
  public:
    /*!
     * Compare dates
     *
     * \param other the other date
     *
     * \return the chronological order
     */
    constexpr std::strong_ordering operator<=>(const Date& other) const noexcept;

    /*!
     * Check dates equality
     *
     * \param other the other date
     *
     * \return \c true when dates are the same day
     */
    constexpr bool operator==(const Date& other) const noexcept;

    /*!
     * Convert to \c std::chrono::year_month_day
     *
     * \return the day
     */
    constexpr explicit operator std::chrono::year_month_day() const noexcept;

    /*!
     * Convert to \c std::chrono::sys_days
     *
     * \return the days since epoch
     */
    explicit operator std::chrono::sys_days() const noexcept;
    // Operators -

  };

  /*!
   * Write RFC 3339 \c full-date
   *
   * \param out the output stream
   * \param date the date
   *
   * \return the output stream
   *
   * \since 0.3
   */
  ANCH_DATE_EXPORT std::ostream& operator<<(std::ostream& out, const anch::date::Date& date);

}  // anch::date

#include "date/impl/date.hpp"
