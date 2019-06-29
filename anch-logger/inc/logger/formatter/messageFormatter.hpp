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
#include <vector>
#ifdef ANCH_BOOST_REGEX
#include <boost/regex.hpp>
#else
#include <regex>
#endif

#include "logger/levels.hpp"
#include "logger/formatter/iFormatter.hpp"


namespace anch {
  namespace logger {
    namespace formatter {

      /*!
       * Format a message for logging acording writer configuration\n
       * \n
       * Paterns are:
       * - %c: The category (logger name)
       * - %p: The logging level
       * - %m: The message
       * - %d{C++ time standard}: The date
       * - %D{C++ time standard}: The high precision date
       * - %t: The current thread ID
       * - %f: The function name (not imlpemented)
       *
       * \author Vincent Lachenal
       */
      class MessageFormatter {
      private:
	// Attributes +
#ifdef ANCH_BOOST_REGEX
	/*! Writter configuration pattern */
	const static boost::regex CONFIG_PATTERN;
#else
	/*! Writter configuration pattern */
	const static std::regex CONFIG_PATTERN;
#endif

	/*! Formatters list */
	std::vector<anch::logger::formatter::IFormatter*> _formatters;
	// Attributes -

      public:
	// Constructors +
	/*!
	 * \ref MessageFormatter constructor
	 *
	 * \param linePattern The message line pattern
	 */
	MessageFormatter(const std::string& linePattern);
	// Constructors -

	// Destructor +
	/*!
	 * \ref MessageFormatter destructor
	 */
	virtual ~MessageFormatter();
	// Destructor -

	// Methods +
      public:
	/*!
	 * Format message.
	 *
	 * \param category The logger category
	 * \param level The logging level
	 * \param message The logging message
	 *
	 * \return The formatted message
	 */
	const std::string formatMessage(const std::string& category,
					const anch::logger::Level& level,
					const std::string& message) const;

      private:
	/*!
	 * Add formatter element to message formatter
	 *
	 * \param str The formatter string
	 */
	void addFormatter(const std::string& strFormatter);
	// Methods -

      };

    }
  }
}
