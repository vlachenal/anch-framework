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
#ifndef _ANCH_LOGGER_FORMATTER_I_FORMATTER_H_
#define _ANCH_LOGGER_FORMATTER_I_FORMATTER_H_

#include <iostream>

namespace anch {
  namespace logger {
    namespace formatter {

      /**
       * Formatter types
       *
       * @author Vincent Lachenal
       */
      enum FormatterType {
	LEVEL,
	CONST,
	STRING,
	CATEGORY,
	THREAD_ID,
	DATE
      };

    }
  }
}

namespace anch {
  namespace logger {
    namespace formatter {

      /**
       * Provide an interface for every item which will be format
       *
       * @author Vincent Lachenal
       */
      class IFormatter {
      public:
	/**
	 * Format value
	 *
	 * @param value The value to format
	 * @param out The output stream to write in
	 */
	virtual void formatValue(const void* const value,
				 std::ostream& out)
	  const throw() = 0;

	/**
	 * Get the formatter type
	 *
	 * @return The formatter type
	 */
	virtual anch::logger::formatter::FormatterType getType() const throw() = 0;

      };

    }
  }
}


#endif // _ANCH_LOGGER_FORMATTER_I_FORMATTER_H_
