/*
  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _ANCH_LOGGER_FORMATTER_CATEGORY_FORMATTER_H_
#define _ANCH_LOGGER_FORMATTER_CATEGORY_FORMATTER_H_

#include "logger/formatter/iFormatter.hpp"


namespace anch {
  namespace logger {
    namespace formatter {

      /**
       * String formatter
       *
       * @author Vincent Lachenal
       */
      class CategoryFormatter: public anch::logger::formatter::IFormatter {
      public:
	// Constructors +
	/**
	 * {@link CategoryFormatter} default constructor
	 */
	CategoryFormatter();
	// Constructors -

	// Destructor +
	/**
	 * {@link CategoryFormatter} destructor
	 */
	~CategoryFormatter();
	// Destructor -

      public:
	/**
	 * Return the input string
	 *
	 * @param value The input string
	 * @param out The output stream to write in
	 */
	virtual void formatValue(const void* const value,
				 std::ostream& out)
	  const throw();

	/**
	 * Get the formatter type
	 *
	 * @return The formatter type
	 */
	virtual anch::logger::formatter::FormatterType getType() const throw();

      };

    }
  }
}

#endif // _ANCH_LOGGER_FORMATTER_CATEGORY_FORMATTER_H_
