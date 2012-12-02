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
#ifndef _ANCH_DATE_FORMATTER_I_DATE_PART_FORMATTER_H_
#define _ANCH_DATE_FORMATTER_I_DATE_PART_FORMATTER_H_

#include <iostream>
#include <ostream>

#include "date/date.hpp"


namespace anch {
  namespace date {
    namespace formatter {

      /**
       * Date part formatter interface
       *
       * @author Vincent Lachenal
       */
      class IDatePartFormatter {
      public:
	/**
	 * {@link IDatePartFormatter} destructor
	 */
	virtual ~IDatePartFormatter() {
	  // Nothing to do
	}

	/**
	 * Format date
	 *
	 * @param date The {@link Date} to format
	 * @param ouput The output stream to write in
	 */
	virtual void format(const anch::date::Date& date, std::ostream& output) const noexcept = 0;

	/**
	 * Get size of formatter part
	 *
	 * @return The size of formatter part
	 */
	virtual size_t getSize() const noexcept = 0;

	/**
	 * Set value to {@link Date}
	 *
	 * @param date The {@link Date} to modify
	 * @param value The value to set
	 */
	virtual bool setValue(anch::date::Date& date, const std::string& value) const noexcept = 0;

	/**
	 * Get formatter part pattern
	 *
	 * @return The formatter part pattern
	 */
	virtual const std::string& getPattern() const noexcept = 0;

	// Accessors +
      protected:
	inline static int32_t getYear(const anch::date::Date& date) {
	  return date._years;
	}
	inline static void setYear(anch::date::Date& date, int32_t year) {
	  date._years = year;
	}

	inline static uint16_t getMonth(const anch::date::Date& date) {
	  return date._months;
	}
	inline static void setMonth(anch::date::Date& date, uint16_t month) {
	  date._months = month;
	}

	inline static uint16_t getDay(const anch::date::Date& date) {
	  return date._mdays;
	}
	inline static void setDay(anch::date::Date& date, uint16_t mday) {
	  date._mdays = mday;
	}

	inline static uint16_t getHour(const anch::date::Date& date) {
	  return date._hours;
	}
	inline static void setHour(anch::date::Date& date, uint16_t hour) {
	  date._hours = hour;
	}

	inline static uint16_t getMinute(const anch::date::Date& date) {
	  return date._minutes;
	}
	inline static void setMinute(anch::date::Date& date, uint16_t minute) {
	  date._minutes = minute;
	}

	inline static uint16_t getSecond(const anch::date::Date& date) {
	  return date._seconds;
	}
	inline static void setSecond(anch::date::Date& date, uint16_t second) {
	  date._seconds = second;
	}

	inline static uint16_t getMillisecond(const anch::date::Date& date) {
	  return date._milliseconds;
	}
	inline static void setMillisecond(anch::date::Date& date, uint16_t milli) {
	  date._milliseconds = milli;
	}
	// Accessors -
      };

    }
  }
}

#endif // _ANCH_DATE_FORMATTER_I_DATE_PART_FORMATTER_H_
