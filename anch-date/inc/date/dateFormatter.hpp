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

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "date/date.hpp"


namespace anch::date {

  /*! \ref anch::date::IDatePartFormatter instance getter function prototype (the instance is deleted by
   * \ref DateFormatter ) */
  typedef anch::date::IDatePartFormatter* (*getInstance)();

  /*!
   * \brief \ref Date string formatter and parser
   *
   * Patterns are:
   * - \%Y: 4 digits year
   * - \%y: 2 digits year (parsed as 1969-2068 like \c strptime )
   * - \%m: month (01-12)
   * - \%d: day in month (01-31)
   * - \%H: hours (00-23)
   * - \%h: hours (01-12, like \c strptime \c \%I ; 12 for noon and midnight)
   * - \%p: a.m./p.m. marker
   * - \%M: minutes (00-59)
   * - \%S: seconds (00-60)
   * - \%s: milliseconds (000-999)
   *
   * Other characters, and unknown patterns, are written as is. Other patterns can be registered with
   * \ref registerFormatterPart .\n
   * Parsing requires every field to have its exact size (no sign, no space) ; fields which are not in the pattern are
   * set to 1970-01-01 00:00:00.000 local time ; impossible dates (2025-02-30 for example) are refused.\n
   * \ref format and \ref parse can be called concurrently. Formatter parts have to be registered before formatters
   * which use them are built.
   *
   * \author Vincent Lachenal
   */
  class DateFormatter {

    // Attributes +
  private:
    /*! Formatter list */
    std::vector<std::unique_ptr<anch::date::IDatePartFormatter>> _formatters;

    /*! String size */
    std::size_t _size;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref DateFormatter constructor
     *
     * \param dateFormat the date format
     */
    explicit DateFormatter(const std::string& dateFormat);

    /*!
     * Forbids \ref DateFormatter copy constructor
     */
    DateFormatter(const DateFormatter&) = delete;

    /*!
     * \ref DateFormatter move constructor
     */
    DateFormatter(DateFormatter&&) noexcept = default;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref DateFormatter destructor
     */
    virtual ~DateFormatter() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Register a new formatter part (when pattern has not already been registered)
     *
     * \param pattern the formatter part pattern (\c '\%' followed by one character)
     * \param instGetter the formatter part new instance getter
     */
    static void registerFormatterPart(const std::string& pattern, getInstance instGetter) noexcept;

    /*!
     * Format date
     *
     * \param date the date to format
     * \param output the output string
     */
    void format(const anch::date::Date& date, std::string& output) const noexcept;

    /*!
     * Format date
     *
     * \param date the date to format
     * \param output the output stream
     */
    void format(const anch::date::Date& date, std::ostream& output) const noexcept;

    /*!
     * Build date from string
     *
     * \param strDate string formatted date
     * \param date the output date (all its fields are set)
     *
     * \throw std::invalid_argument invalid format, impossible date or date out of range (years 1678 to 2261)
     */
    void parse(const std::string& strDate, anch::date::Date& date) const;

    /*!
     * Build date from string
     *
     * \param strDate string formatted date
     *
     * \return the date
     *
     * \throw std::invalid_argument invalid format, impossible date or date out of range (years 1678 to 2261)
     */
    anch::date::Date parse(const std::string& strDate) const;

  private:
    /*!
     * Add formatter to formatter list
     *
     * \param formatter the formatter part
     */
    void addFormatter(anch::date::IDatePartFormatter* formatter);
    // Methods -

  };

} // anch::date
