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
#include <sstream>
#include <vector>

#include "log/levels.hpp"
#include "log/writer.hpp"

namespace anch::log {

  /*!
   * Logger class.\n
   * Aims to provide logging facilities.\n
   * \n
   * Logging levels are:\n
   * - TRACE
   * - DEBUG
   * - INFO
   * - WARN
   * - ERROR
   * - FATAL
   *
   * \author Vincent Lachenal
   */
  class ANCH_LOG_EXPORT Logger {
    // Attributes +
  private:
    /*! Logger name */
    std::string _name;

    /*! The current logging level */
    anch::log::Level _level;

    /*! Logger writers */
    std::vector<anch::log::Writer*> _writers;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Logger constructor.
     *
     * \param name Logger name
     * \param level The logging level to set
     * \param writers The writers list
     */
    Logger(const std::string& name,
	   const anch::log::Level level,
	   const std::vector<anch::log::Writer*>& writers);
    // Constructors -

    // Destructor +
    /*!
     * \ref Logger destructor
     */
    virtual ~Logger();
    // Destructor -

    // Methods +
  public:
    /*!
     * Concatenate message and log it in TRACE level.\n
     * Removed at compile time when TRACE is lower than \ref anch::log::MIN_LEVEL "MIN_LEVEL" ( \c ANCH_LOG_MIN_LEVEL ).\n
     * Values are always evaluated.
     *
     * \param value The first value of the message to concatenate
     * \param values The other values of the message to concatenate
     */
    template<typename T, typename... Q>
    void trace(const T& value, const Q&... values) const noexcept;

    /*!
     * Concatenate message and log it in DEBUG level.\n
     * Removed at compile time when DEBUG is lower than \ref anch::log::MIN_LEVEL "MIN_LEVEL" ( \c ANCH_LOG_MIN_LEVEL ).\n
     * Values are always evaluated.
     *
     * \param value The first value of the message to concatenate
     * \param values The other values of the message to concatenate
     */
    template<typename T, typename... Q>
    void debug(const T& value, const Q&... values) const noexcept;

    /*!
     * Concatenate message and log it in INFO level.\n
     * Removed at compile time when INFO is lower than \ref anch::log::MIN_LEVEL "MIN_LEVEL" ( \c ANCH_LOG_MIN_LEVEL ).\n
     * Values are always evaluated.
     *
     * \param value The first value of the message to concatenate
     * \param values The other values of the message to concatenate
     */
    template<typename T, typename... Q>
    void info(const T& value, const Q&... values) const noexcept;

    /*!
     * Concatenate message and log it in WARN level.\n
     * Removed at compile time when WARN is lower than \ref anch::log::MIN_LEVEL "MIN_LEVEL" ( \c ANCH_LOG_MIN_LEVEL ).\n
     * Values are always evaluated.
     *
     * \param value The first value of the message to concatenate
     * \param values The other values of the message to concatenate
     */
    template<typename T, typename... Q>
    void warn(const T& value, const Q&... values) const noexcept;

    /*!
     * Concatenate message and log it in ERROR level.\n
     * Removed at compile time when ERROR is lower than \ref anch::log::MIN_LEVEL "MIN_LEVEL" ( \c ANCH_LOG_MIN_LEVEL ).\n
     * Values are always evaluated.
     *
     * \param value The first value of the message to concatenate
     * \param values The other values of the message to concatenate
     */
    template<typename T, typename... Q>
    void error(const T& value, const Q&... values) const noexcept;

    /*!
     * Concatenate message and log it in FATAL level.\n
     * Removed at compile time when FATAL is lower than \ref anch::log::MIN_LEVEL "MIN_LEVEL" ( \c ANCH_LOG_MIN_LEVEL ).\n
     * Values are always evaluated.
     *
     * \param value The first value of the message to concatenate
     * \param values The other values of the message to concatenate
     */
    template<typename T, typename... Q>
    void fatal(const T& value, const Q&... values) const noexcept;

    /*!
     * Check if a message in this level will be written by this logger
     *
     * \param level The message level
     *
     * \return \c true when level is greater or equal to the logger level and the logger has writers
     *
     * \since 0.2
     */
    bool isEnabled(const anch::log::Level& level) const noexcept;

    /*!
     * Write an already formatted message on every writer.\n
     * The level is not checked: check it with \ref isEnabled before formatting the message.\n
     * Writer errors are ignored: a writer error does not prevent other writers to log.
     *
     * \param level The message level
     * \param message The message to write
     *
     * \since 0.2
     */
    void write(const anch::log::Level& level, const std::string& message) const noexcept;

  private:
    /*!
     * Log a message\n
     * Logger last iteration
     *
     * \param level The logging level
     * \param out The output stream to concatenate message
     * \param value The message to log
     */
    template<typename T>
    void log(const Level& level,
	     std::ostringstream& out,
	     const T& value) const noexcept;

    /*!
     * Concatenate message and log it
     *
     * \param level The message level
     * \param value The first value of the message to concatenate
     * \param values The other values of the message to concatenate
     */
    template<typename T, typename... Q>
    void log(const Level& level,
	     const T& value,
	     const Q&... values) const noexcept;

    /*!
     * Concatenate message and log it
     *
     * \param level The logging level
     * \param out The output stream to concatenate message
     * \param value The first value of the message to concatenate
     * \param values The other values of the message to concatenate
     */
    template<typename T, typename... Q>
    void log(const Level& level,
	     std::ostringstream& out,
	     const T& value,
	     const Q&... values) const noexcept;
    // Methods -

  };

}

#include "log/impl/logger.hpp"
