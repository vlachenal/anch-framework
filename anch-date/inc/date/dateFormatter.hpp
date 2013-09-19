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
#include <map>
#include <vector>
#include <boost/regex.hpp>

#include "date/date.hpp"


namespace anch {
  namespace date {

    /*! \ref IDateFormatter instance getter function/method prototype */
    typedef anch::date::formatter::IDatePartFormatter* (*getInstance)();

    /*!
     * \ref Date string formatter.
     *
     * Paterns are:
     * - \%Y: 4 digits year
     * - \%y: 2 digits year
     * - \%m: Month (1-12)
     * - \%d: Day in month (1-31)
     * - \%H: 00-23 hours
     * - \%h: 00-11 hours
     * - \%p: AM/PM marker
     * - \%M: minutes (00-59)
     * - \%S: seconds (00-59)
     * - \%s: milliseconds (000-999)
     *
     * Do not use characters '\%' in your string pattern.<br>
     *
     * Warning: This class is not thread safe.
     *
     * \author Vincent Lachenal
     */
    class DateFormatter {

      // Attributes +
      /*! Date formatter pattern */
      const static boost::regex DATE_PATTERN;

      /*! Formatter registration map */
      static std::map<std::string, getInstance> FORMATTERS;

      /*! Formatter list */
      std::vector<anch::date::formatter::IDatePartFormatter*> _formatters;

      /*! String size */
      size_t _size;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref DateFormatter constructor
       *
       * \param dateFormat The date format
       */
      DateFormatter(const std::string& dateFormat);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref DateFormatter destructor
       */
      virtual ~DateFormatter();
      // Destructor -

      // Methods +
    public:
      /*!
       * Register a new formatter part
       *
       * \param pattern The formatter part pattern
       * \param instGetter The formatter part new instance getter
       */
      static void registerFormatterPart(const std::string& pattern,
					getInstance instGetter);

      /*!
       * Format date
       *
       * \param date The date to format
       * \param output The output string
       */
      void format(const anch::date::Date& date, std::string& output) const;

      /*!
       * Format date
       *
       * \param date The date to format
       * \param output The output stream
       */
      void format(const anch::date::Date& date, std::ostream& output) const;

      /*!
       * Build date from string.\n
       * You can build the output \ref Date with Date(false) constructor.
       *
       * \param strDate String formatted date
       * \param date The output date
       */
      void parse(const std::string& strDate, anch::date::Date& date) const;

      /*!
       * Build date from string.\n
       * You had to delete \ref Date not to make memory leaks.
       *
       * \param strDate String formatted date
       *
       * \return A date
       */
      anch::date::Date* parse(const std::string& strDate) const;

    private:
      /*!
       * Add formatter to formatter list
       *
       * \param strFormatter The formatter part to instanciate
       */
      void addFormatter(const std::string& strFormatter);
      // Methods -

    };

  }
}

#endif // _ANCH_DATE_DATE_FORMAT_H_
