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

#include "anch-log-export.hpp"

#include <iostream>
#include <vector>
#include <regex>

#include "log/levels.hpp"
#include "log/fmt/iFormatter.hpp"


namespace anch::log {
  class Writer;
}

namespace anch::log::fmt {

  /*!
   * Format a message for logging acording writer configuration\n
   * \n
   * Paterns are:
   * - $c: The category (logger name)
   * - $p: The logging level
   * - $m: The message
   * - $d{strftime format}: The date
   * - $D{anch::date format}: The high precision date
   * - $C{key}: The value of \c key in the map diagnostic context
   * - $t: The current thread ID
   *
   * Other characters (including a \c $ which does not start a pattern) are written as is.
   *
   * \author Vincent Lachenal
   */
  class ANCH_LOG_EXPORT MessageFormatter {
    friend class anch::log::Writer;

  private:
    // Attributes +
    /*! Writter configuration pattern */
    const static std::regex CONFIG_PATTERN;

    /*! Formatters list */
    std::vector<anch::log::fmt::IFormatter*> _formatters;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref MessageFormatter default constructor
     */
    MessageFormatter();

    /*!
     * \ref MessageFormatter constructor
     *
     * \param linePattern The message line pattern
     */
    MessageFormatter(const std::string& linePattern);

    /*!
     * Prohibits \ref MessageFormatter copy constructor (it owns its formatters)
     */
    MessageFormatter(const MessageFormatter&) = delete;
    // Constructors -

    // Destructor +
    /*!
     * \ref MessageFormatter destructor
     */
    virtual ~MessageFormatter();
    // Destructor -

    // Operators +
  public:
    /*!
     * Prohibits \ref MessageFormatter copy assignment (it owns its formatters)
     */
    MessageFormatter& operator=(const MessageFormatter&) = delete;
    // Operators -

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
				    const anch::log::Level& level,
				    const std::string& message) const;

  private:
    /*!
     * Add formatter element to message formatter
     *
     * \param str The formatter string
     */
    void addFormatter(const std::string& strFormatter);

    /*!
     * Parse formatters' pattern
     *
     * \param pattern the formatters' pattern
     */
    void parserPattern(const std::string& pattern);
    // Methods -

  };

}
