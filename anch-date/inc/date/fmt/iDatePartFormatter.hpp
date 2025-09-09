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

#include <iostream>
#include <ostream>

#include "date/date.hpp"


namespace anch::date {

  /*!
   * Date part formatter interface
   *
   * \author Vincent Lachenal
   */
  class IDatePartFormatter {
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
     * \param date The \ref Date to format
     * \param output The output stream to write in
     */
    virtual void format(const anch::date::Date& date, std::ostream& output) const noexcept = 0;

    /*!
     * Get size of formatter part
     *
     * \return The size of formatter part
     */
    virtual std::size_t getSize() const noexcept = 0;

    /*!
     * Set value to \ref Date
     *
     * \param date The \ref Date to modify
     * \param value The value to set
     */
    virtual bool setValue(anch::date::Date& date, const std::string& value) const noexcept = 0;

    /*!
     * Get formatter part pattern
     *
     * \return The formatter part pattern
     */
    virtual const std::string& getPattern() const noexcept = 0;

    // Accessors +
  protected:
    /*!
     * \ref Date year getter
     *
     * \return The year
     */
    static int32_t getYear(const anch::date::Date& date);

    /*!
     * \ref Date year setter
     *
     * \param date The \ref Date to modify
     * \param year The year value to set
     */
    static void setYear(anch::date::Date& date, int32_t year);

    /*!
     * \ref Date month getter
     *
     * \return The month
     */
    static uint16_t getMonth(const anch::date::Date& date);

    /*!
     * \ref Date month setter
     *
     * \param date The \ref Date to modify
     * \param month The month value to set
     */
    static void setMonth(anch::date::Date& date, uint16_t month);

    /*!
     * \ref Date day getter
     *
     * \return The day
     */
    static uint16_t getDay(const anch::date::Date& date);

    /*!
     * \ref Date day setter
     *
     * \param date The \ref Date to modify
     * \param mday The day value to set
     */
    static void setDay(anch::date::Date& date, uint16_t mday);

    /*!
     * \ref Date hour getter
     *
     * \return The hour
     */
    static uint16_t getHour(const anch::date::Date& date);

    /*!
     * \ref Date hour setter
     *
     * \param date The \ref Date to modify
     * \param hour The hour value to set
     */
    static void setHour(anch::date::Date& date, uint16_t hour);

    /*!
     * \ref Date minute getter
     *
     * \return The minute
     */
    static uint16_t getMinute(const anch::date::Date& date);

    /*!
     * \ref Date minute setter
     *
     * \param date The \ref Date to modify
     * \param minute The minute value to set
     */
    static void setMinute(anch::date::Date& date, uint16_t minute);

    /*!
     * \ref Date second getter
     *
     * \return The second
     */
    static uint16_t getSecond(const anch::date::Date& date);

    /*!
     * \ref Date second setter
     *
     * \param date The \ref Date to modify
     * \param second The second value to set
     */
    static void setSecond(anch::date::Date& date, uint16_t second);

    /*!
     * \ref Date millisecond getter
     *
     * \return The millisecond
     */
    static uint16_t getMillisecond(const anch::date::Date& date);

    /*!
     * \ref Date millisecond setter
     *
     * \param date The \ref Date to modify
     * \param milli The millisecond value to set
     */
    static void setMillisecond(anch::date::Date& date, uint16_t milli);

    /*!
     * \ref Date microsecond getter
     *
     * \return The microsecond
     */
    static uint16_t getMicrosecond(const anch::date::Date& date);

    /*!
     * \ref Date microsecond setter
     *
     * \param date The \ref Date to modify
     * \param micro The microsecond value to set
     */
    static void setMicrosecond(anch::date::Date& date, uint16_t micro);

    /*!
     * \ref Date nanosecond getter
     *
     * \return The nanosecond
     */
    static uint16_t getNanosecond(const anch::date::Date& date);

    /*!
     * \ref Date nanosecond setter
     *
     * \param date The \ref Date to modify
     * \param nano The nanosecond value to set
     */
    static void setNanosecond(anch::date::Date& date, uint16_t nano);
    // Accessors -
  };

}

#include "date/impl/fmt/iDatePartFormatter.hpp"
