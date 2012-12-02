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
#ifndef _ANCH_LOGGER_FORMATTER_CONST_FORMATTER_H_
#define _ANCH_LOGGER_FORMATTER_CONST_FORMATTER_H_

#include "logger/formatter/iFormatter.hpp"


namespace anch {
  namespace logger {
    namespace formatter {

      /**
       * Constant string formatter
       *
       * @author Vincent Lachenal
       */
      class ConstFormatter: public anch::logger::formatter::IFormatter {
      private:
	// Attributes +
	/** The string to print */
	std::string _strToPrint;
	// Attributes -

      public:
	// Constructor +
	/**
	 * {@link ConstFormatter} constructor
	 *
	 * @param strToPrint The string to print
	 */
	ConstFormatter(const std::string strToPrint);
	// Constructor -

	// Destructor +
	/**
	 * {@link ConstFormatter} destructor
	 */
	~ConstFormatter();
	// Destructor -

      public:
	/**
	 * Return the constant string
	 *
	 * @param value Nothing (NULL will be passed every time)
	 * @param out The output stream to write in
	 */
	virtual void formatValue(const void* const value,
				 std::ostream& out)
	  const noexcept;

	/**
	 * Get the formatter type
	 *
	 * @return The formatter type
	 */
	virtual anch::logger::formatter::FormatterType getType() const noexcept;

      };

    }
  }
}

#endif // _ANCH_LOGGER_FORMATTER_CONST_FORMATTER_H_
