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
#ifndef _ANCH_DATE_DATE_FORMAT_H_
#define _ANCH_DATE_DATE_FORMAT_H_

#include <iostream>
#include <vector>
#include <boost/regex.hpp>

#include "date/date.hpp"


namespace anch {
  namespace date {

    /**
     * {@link Date} string formatter.<br>
     * <br>
     * Paterns are:
     * <li>
     *   <ul>%Y: 4 digits year</ul>
     *   <ul>%y: 2 digits year</ul>
     *   <ul>%M: Month (1-12)</ul>
     *   <ul>%d: Day in month (1-31)</ul>
     *   <ul>%H: 00-23 hours</ul>
     *   <ul>%h: 00-11 hours</ul>
     *   <ul>%a: AM/PM marker</ul>
     *   <ul>%m: minutes (00-59)</ul>
     *   <ul>%s: seconds (00-59)</ul>
     *   <ul>%S: milliseconds (000-999)</ul>
     * </li>
     * <br>
     * Do not use characters '%' in your string pattern.<br>
     * <br>
     * Warning: This class is not thread safe.
     *
     * @author Vincent Lachenal
     */
    class DateFormatter {

      // Attributes +
      /** Date formatter pattern */
      const static boost::regex DATE_PATTERN;

      /** Formatter list */
      std::vector<anch::date::formatter::IDatePartFormatter*> _formatters;

      /** String size */
      size_t _size;
      // Attributes -

      // Constructors +
    public:
      /**
       * {@link DateFormatter} constructor
       *
       * @param dateFormat The date format
       */
      DateFormatter(const std::string& dateFormat);
      // Constructors -

      // Destructor +
    public:
      /**
       * {@link DateFormatter} destructor
       */
      virtual ~DateFormatter();
      // Destructor -

      // Methods +
    public:
      /**
       * Format date
       *
       * @param date The date to format
       * @param output The output string
       */
      void format(const anch::date::Date& date, std::string& output) const;

      /**
       * Format date
       *
       * @param date The date to format
       * @param output The output stream
       */
      void format(const anch::date::Date& date, std::ostream& output) const;

      /**
       * Build date from string.<br>
       * You can build the output {@link Date} with Date(false) constructor.
       *
       * @param strDate String formatted date
       * @param date The output date
       */
      void parse(const std::string& strDate, anch::date::Date& date) const;

      /**
       * Build date from string.<br>
       * You had to delete {@link Date} not to make memory leaks.
       *
       * @param strDate String formatted date
       *
       * @return A date
       */
      anch::date::Date* parse(const std::string& strDate) const;

    private:
      /**
       * Add formatter to formatter list
       *
       * @param strFormatter The formatter part to instanciate
       */
      void addFormatter(const std::string& strFormatter);
      // Methods -

    };

  }
}

#endif // _ANCH_DATE_DATE_FORMAT_H_
