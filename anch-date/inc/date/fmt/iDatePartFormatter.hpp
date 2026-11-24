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

#include <cstdint>
#include <ostream>
#include <string>

#include "date/dateTime.hpp"


namespace anch::date {

  /*!
   * \brief Date and time part formatter interface
   *
   * Implement this interface to add a pattern to \ref DateFormatter (\ref DateFormatter::registerFormatterPart ).
   * Protected accessors give access to \ref DateTime fields.\n
   * A part has a fixed size: \ref setValue receives exactly \ref getSize characters. When a \ref DateTime is parsed, its
   * fields are set to 1970-01-01 00:00:00.000 before parts are called, and the timestamp is computed afterwards.
   *
   * \author Vincent Lachenal
   */
  class ANCH_DATE_EXPORT IDatePartFormatter {
  public:
    /*!
     * \ref IDatePartFormatter destructor
     */
    virtual ~IDatePartFormatter() {
      // Nothing to do
    }

    /*!
     * Format date
     *
     * \param date The \ref DateTime to format
     * \param output The output stream to write in
     */
    virtual void format(const anch::date::DateTime& date, std::ostream& output) const noexcept = 0;

    /*!
     * Get size of formatter part
     *
     * \return The size of formatter part
     */
    virtual std::size_t getSize() const noexcept = 0;

    /*!
     * Set value to \ref DateTime
     *
     * \param date The \ref DateTime to modify
     * \param value The value to set
     *
     * \return \c true when value is valid, \c false otherwise
     */
    virtual bool setValue(anch::date::DateTime& date, const std::string& value) const noexcept = 0;

    /*!
     * Get formatter part pattern
     *
     * \return The formatter part pattern
     */
    virtual const std::string& getPattern() const noexcept = 0;

    // Accessors +
  protected:
    /*!
     * \ref DateTime year getter
     *
     * \return The year
     */
    static int32_t getYear(const anch::date::DateTime& date);

    /*!
     * \ref DateTime year setter
     *
     * \param date The \ref DateTime to modify
     * \param year The year value to set
     */
    static void setYear(anch::date::DateTime& date, int32_t year);

    /*!
     * \ref DateTime month getter
     *
     * \return The month
     */
    static uint16_t getMonth(const anch::date::DateTime& date);

    /*!
     * \ref DateTime month setter
     *
     * \param date The \ref DateTime to modify
     * \param month The month value to set
     */
    static void setMonth(anch::date::DateTime& date, uint16_t month);

    /*!
     * \ref DateTime day getter
     *
     * \return The day
     */
    static uint16_t getDay(const anch::date::DateTime& date);

    /*!
     * \ref DateTime day setter
     *
     * \param date The \ref DateTime to modify
     * \param mday The day value to set
     */
    static void setDay(anch::date::DateTime& date, uint16_t mday);

    /*!
     * \ref DateTime hour getter
     *
     * \return The hour
     */
    static uint16_t getHour(const anch::date::DateTime& date);

    /*!
     * \ref DateTime hour setter
     *
     * \param date The \ref DateTime to modify
     * \param hour The hour value to set
     */
    static void setHour(anch::date::DateTime& date, uint16_t hour);

    /*!
     * \ref DateTime minute getter
     *
     * \return The minute
     */
    static uint16_t getMinute(const anch::date::DateTime& date);

    /*!
     * \ref DateTime minute setter
     *
     * \param date The \ref DateTime to modify
     * \param minute The minute value to set
     */
    static void setMinute(anch::date::DateTime& date, uint16_t minute);

    /*!
     * \ref DateTime second getter
     *
     * \return The second
     */
    static uint16_t getSecond(const anch::date::DateTime& date);

    /*!
     * \ref DateTime second setter
     *
     * \param date The \ref DateTime to modify
     * \param second The second value to set
     */
    static void setSecond(anch::date::DateTime& date, uint16_t second);

    /*!
     * \ref DateTime millisecond getter
     *
     * \return The millisecond
     */
    static uint16_t getMillisecond(const anch::date::DateTime& date);

    /*!
     * \ref DateTime millisecond setter
     *
     * \param date The \ref DateTime to modify
     * \param milli The millisecond value to set
     */
    static void setMillisecond(anch::date::DateTime& date, uint16_t milli);

    /*!
     * \ref DateTime microsecond getter
     *
     * \return The microsecond
     */
    static uint16_t getMicrosecond(const anch::date::DateTime& date);

    /*!
     * \ref DateTime microsecond setter
     *
     * \param date The \ref DateTime to modify
     * \param micro The microsecond value to set
     */
    static void setMicrosecond(anch::date::DateTime& date, uint16_t micro);

    /*!
     * \ref DateTime nanosecond getter
     *
     * \return The nanosecond
     */
    static uint16_t getNanosecond(const anch::date::DateTime& date);

    /*!
     * \ref DateTime nanosecond setter
     *
     * \param date The \ref DateTime to modify
     * \param nano The nanosecond value to set
     */
    static void setNanosecond(anch::date::DateTime& date, uint16_t nano);
    // Accessors -
  };

}

#include "date/impl/fmt/iDatePartFormatter.hpp"
