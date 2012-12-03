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
#ifndef _ANCH_DATE_FORMATTER_HOUR24_FORMATTER_H_
#define _ANCH_DATE_FORMATTER_HOUR24_FORMATTER_H_

#include "date/formatter/iDatePartFormatter.hpp"

namespace anch {
  namespace date {
    namespace formatter {

      /**
       * 0-23 hours formatter part
       *
       * @author Vincent Lachenal
       */
      class Hour24Formatter: public IDatePartFormatter {
      public:
	/** The pattern */
	static const std::string PATTERN;

      public:
	/**
	 * {@link Hour24Formatter} default constructor
	 */
	Hour24Formatter();

	/**
	 * {@link Hour24Formatter} destructor
	 */
	virtual ~Hour24Formatter();

	/**
	 * Format date
	 *
	 * @param date The {@link Date} to format
	 * @param output The output stream to write in
	 */
	void format(const anch::date::Date& date, std::ostream& output) const noexcept;

	/**
	 * Get size of formatter part
	 *
	 * @return The size of formatter part
	 */
	size_t getSize() const noexcept;

	/**
	 * Set value to {@link Date}
	 *
	 * @param date The {@link Date} to modify
	 * @param value The value to set
	 */
	bool setValue(anch::date::Date& date, const std::string& value) const noexcept;

	/**
	 * Get formatter part pattern
	 *
	 * @return The formatter part pattern
	 */
	const std::string& getPattern() const noexcept;

	/**
	 * Return a new instance of {@link Hour24Formatter}
	 *
	 * @return A new instance of {@link Hour24Formatter}
	 */
	static IDatePartFormatter* getInstance();

      };

    }
  }
}

#endif // _ANCH_DATE_FORMATTER_HOUR24_FORMATTER_H_
