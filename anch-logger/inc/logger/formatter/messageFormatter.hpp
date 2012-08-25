/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standart
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
#ifndef _ANCH_LOGGER_FORMATTER_MESSAGE_FORMATTER_H_
#define _ANCH_LOGGER_FORMATTER_MESSAGE_FORMATTER_H_

#include <iostream>
#include <vector>
#include <boost/regex.hpp>

#include "logger/levels.hpp"
#include "logger/formatter/iFormatter.hpp"


namespace anch {
  namespace logger {
    namespace formatter {

      /**
       * Format a message for logging acording writer configuration<br>
       * <br>
       * Paterns are:
       * <li>
       *   <ul>%c: The category (logger name)</ul>
       *   <ul>%p: The logging level</ul>
       *   <ul>%m: The message</ul>
       *   <ul>%d{C++ time standard}: The date</ul>
       *   <ul>%t: The current thread ID</ul>
       *   <ul>%f: The function name (not imlpemented)</ul>
       * </li>
       *
       * @author Vincent Lachenal
       */
      class MessageFormatter {
      private:
	// Attributes +
	/** Writter configuration pattern */
	static boost::regex CONFIG_PATTERN;

	/** Formatters list */
	std::vector<anch::logger::formatter::IFormatter*> _formatters;
	// Attributes -

      public:
	// Constructors +
	/**
	 * {@link MessageFormatter} constructor
	 *
	 * @param linePattern The message line pattern
	 */
	MessageFormatter(const std::string& linePattern);
	// Constructors -

	// Destructor +
	/**
	 * {@link MessageFormatter} destructor
	 */
	virtual ~MessageFormatter();
	// Destructor -

      public:
	/**
	 * Format message.
	 *
	 * @param category The logger category
	 * @param level The logging level
	 * @param message The logging message
	 *
	 * @return The formatted message
	 */
	const std::string formatMessage(const std::string& category,
					const anch::logger::Level& level,
					const std::string& message) const;

      private:
	/**
	 * Add formatter element to message formatter
	 *
	 * @param str The formatter string
	 */
	void addFormatter(const std::string& strFormatter);

      };

    }
  }
}

#endif // _ANCH_LOGGER_FORMATTER_MESSAGE_FORMATTER_H_
