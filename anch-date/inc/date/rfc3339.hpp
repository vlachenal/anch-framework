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
#include <cstddef>
#include <string>
#include <string_view>

#include "date/date.hpp"
#include "date/dateTime.hpp"
#include "date/time.hpp"


/*!
 * \brief RFC 3339 dates formatting and parsing
 *
 * RFC 3339 (https://www.rfc-editor.org/rfc/rfc3339#section-5.6) defines:
 * - \c date-time : <tt>YYYY-MM-DDTHH:MM:SS[.fraction](Z|+HH:MM|-HH:MM)</tt> (a point in time), mapped to
 *   \ref anch::date::DateTime ;
 * - \c full-date : <tt>YYYY-MM-DD</tt> (a calendar day without time zone), mapped to
 *   \ref anch::date::Date ;
 * - \c partial-time : <tt>HH:MM:SS[.fraction]</tt> (a time of day without time zone), mapped to
 *   \ref anch::date::Time (leap seconds are refused: a time of day without day can not hold them).
 *
 * These are the \c date-time and \c date formats of JSON schema and OpenAPI.\n
 * Parsing follows the RFC grammar: \c T and \c Z can be lower case ; the space separator allowed by the RFC note is
 * refused ; fraction can have any number of digits (digits after the nanosecond are truncated) ; offset \c -00:00
 * (unknown local offset) is read as UTC ; leap second ( \c 60 ) is accepted at 23:59 UTC only, and is read as the
 * next second (time points have no leap seconds).\n
 * Functions are thread safe.
 *
 * \author Vincent Lachenal
 *
 * \since 0.2
 */
namespace anch::date::rfc3339 {

  /*!
   * \brief Offset used to format \c date-time
   *
   * \since 0.2
   */
  enum class Offset {
    /*! UTC time, with \c Z offset */
    UTC,

    /*! Local time (follows \c TZ environment variable), with its numeric offset (\c +02:00 for example) ; historic
     * offsets with seconds are truncated to the minute (written time is shifted, the point in time is preserved) */
    LOCAL
  };

  /*! Maximum \c date-time string size: <tt>YYYY-MM-DDTHH:MM:SS.nnnnnnnnn+HH:MM</tt> */
  constexpr std::size_t DATE_TIME_MAX_SIZE = 35;

  /*! \c full-date string size: <tt>YYYY-MM-DD</tt> */
  constexpr std::size_t DATE_SIZE = 10;

  /*! Maximum \c partial-time string size: <tt>HH:MM:SS.nnnnnnnnn</tt> */
  constexpr std::size_t TIME_MAX_SIZE = 18;

  /*!
   * Format \c date-time .\n
   * Fraction is written with the number of digits needed by the date: none, 3, 6 or 9.
   *
   * \param date the date to format
   * \param output the output buffer (at least \ref DATE_TIME_MAX_SIZE characters ; not \c '\0' terminated)
   * \param offset the offset to use (default UTC)
   *
   * \return the number of characters written
   */
  ANCH_DATE_EXPORT std::size_t formatDateTime(const anch::date::DateTime& date, char* output, Offset offset = Offset::UTC) noexcept;

  /*!
   * Format \c date-time
   *
   * \param date the date to format
   * \param offset the offset to use (default UTC)
   *
   * \return the formatted date
   */
  ANCH_DATE_EXPORT std::string formatDateTime(const anch::date::DateTime& date, Offset offset = Offset::UTC);

  /*!
   * Parse \c date-time
   *
   * \param text the text to parse
   *
   * \return the date
   *
   * \throw std::invalid_argument invalid format, impossible date or point in time out of \ref anch::date::DateTime
   *        range (years 1678 to 2261)
   */
  ANCH_DATE_EXPORT anch::date::DateTime parseDateTime(std::string_view text);

  /*!
   * Format \c full-date
   *
   * \param date the date to format
   * \param output the output buffer (at least \ref DATE_SIZE characters ; not \c '\0' terminated)
   *
   * \return the number of characters written (\ref DATE_SIZE )
   */
  ANCH_DATE_EXPORT std::size_t formatDate(const anch::date::Date& date, char* output) noexcept;

  /*!
   * Format \c full-date
   *
   * \param date the date to format
   *
   * \return the formatted date
   */
  ANCH_DATE_EXPORT std::string formatDate(const anch::date::Date& date);

  /*!
   * Parse \c full-date
   *
   * \param text the text to parse
   *
   * \return the date
   *
   * \throw std::invalid_argument invalid format or impossible date
   */
  ANCH_DATE_EXPORT anch::date::Date parseDate(std::string_view text);

  /*!
   * Format \c partial-time .\n
   * Fraction is written with the number of digits needed by the time: none, 3, 6 or 9.
   *
   * \param time the time to format
   * \param output the output buffer (at least \ref TIME_MAX_SIZE characters ; not \c '\0' terminated)
   *
   * \return the number of characters written
   *
   * \since 0.3
   */
  ANCH_DATE_EXPORT std::size_t formatTime(const anch::date::Time& time, char* output) noexcept;

  /*!
   * Format \c partial-time
   *
   * \param time the time to format
   *
   * \return the formatted time
   *
   * \since 0.3
   */
  ANCH_DATE_EXPORT std::string formatTime(const anch::date::Time& time);

  /*!
   * Parse \c partial-time (fraction can have any number of digits: digits after the nanosecond are truncated)
   *
   * \param text the text to parse
   *
   * \return the time
   *
   * \throw std::invalid_argument invalid format, impossible time or leap second
   *
   * \since 0.3
   */
  ANCH_DATE_EXPORT anch::date::Time parseTime(std::string_view text);

}  // anch::date::rfc3339
