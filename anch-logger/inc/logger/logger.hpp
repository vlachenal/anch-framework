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
#ifndef _ANCH_LOGGER_LOGGER_H_
#define _ANCH_LOGGER_LOGGER_H_

#include <iostream>
#include <sstream>
#include <vector>

#include "logger/levels.hpp"
#include "logger/writer.hpp"

namespace anch {
  namespace logger {

    /**
     * Logger class.<br>
     * Aims to provide logging facilities.<br>
     * <br>
     * Logging levels are:<br>
     * <ul>
     *   <li>TRACE</li>
     *   <li>DEBUG</li>
     *   <li>INFO</li>
     *   <li>WARN</li>
     *   <li>ERROR</li>
     *   <li>FATAL</li>
     * </ul>
     *
     * @author Vincent Lachenal
     */
    class Logger {
    private:
      /** Logger name */
      std::string _name;

      /** The current logging level */
      anch::logger::Level _level;

      /** Logger writers */
      std::vector<anch::logger::Writer*> _writers;

    public:
      /**
       * {@link Logger} constructor.
       *
       * @param name Logger name
       * @param level The logging level to set
       * @param writers The writers list
       */
      Logger(const std::string& name,
	     const anch::logger::Level level,
	     const std::vector<anch::logger::Writer*>& writers);

      /**
       * {@link Logger} destructor
       */
      virtual ~Logger();

    public:
      /**
       * Concatenate message and log it in TRACE level
       *
       * @param value The first value of the message to concatenate
       * @param values The other values of the message to concatenate
       */
      template<typename T, typename... Q>
      void trace(const T& value, const Q&... values) const throw() {
	log(Level::TRACE, value, values...);
      };

      /**
       * Concatenate message and log it in DEBUG level
       *
       * @param value The first value of the message to concatenate
       * @param values The other values of the message to concatenate
       */
      template<typename T, typename... Q>
      void debug(const T& value, const Q&... values) const throw() {
	log(Level::DEBUG, value, values...);
      };

      /**
       * Concatenate message and log it in INFO level
       *
       * @param value The first value of the message to concatenate
       * @param values The other values of the message to concatenate
       */
      template<typename T, typename... Q>
      void info(const T& value, const Q&... values) const throw() {
	log(Level::INFO, value, values...);
      };

      /**
       * Concatenate message and log it in WARN level
       *
       * @param value The first value of the message to concatenate
       * @param values The other values of the message to concatenate
       */
      template<typename T, typename... Q>
      void warn(const T& value, const Q&... values) const throw() {
	log(Level::WARN, value, values...);
      };

      /**
       * Concatenate message and log it in ERROR level
       *
       * @param value The first value of the message to concatenate
       * @param values The other values of the message to concatenate
       */
      template<typename T, typename... Q>
      void error(const T& value, const Q&... values) const throw() {
	log(Level::ERROR, value, values...);
      };

      /**
       * Concatenate message and log it in FATAL level
       *
       * @param value The first value of the message to concatenate
       * @param values The other values of the message to concatenate
       */
      template<typename T, typename... Q>
      void fatal(const T& value, const Q&... values) const throw() {
	log(Level::FATAL, value, values...);
      };

    private:
      /**
       * Log a message<br>
       * Logger last iteration
       *
       * @param level The logging level
       * @param out The output stream to concatenate message
       * @param value The message to log 
       */
      template<typename T>
      void log(const Level& level,
	       std::ostringstream& out,
	       const T& value) const throw() {
	out << value;
	const std::string& message = out.str();
	for(size_t i = 0 ; i < _writers.size() ; i++) {
	  _writers[i]->write(_name, level, message);
	}
      };

      /**
       * Concatenate message and log it
       *
       * @param level The message level
       * @param value The first value of the message to concatenate
       * @param values The other values of the message to concatenate
       */
      template<typename T, typename... Q>
      void log(const Level& level,
	       const T& value,
	       const Q&... values) const throw() {
	if(level >= _level && !_writers.empty()) {
	  std::ostringstream out;
	  log(level, out, value, values...);
	}
      };

      /**
       * Concatenate message and log it
       *
       * @param level The logging level
       * @param out The output stream to concatenate message
       * @param value The first value of the message to concatenate
       * @param values The other values of the message to concatenate
       */
      template<typename T, typename... Q>
      void log(const Level& level,
	       std::ostringstream& out,
	       const T& value,
	       const Q&... values) const throw() {
	out << value;
	log(level, out, values...);
      };

    };

  }
}

#endif // _ANCH_LOGGER_LOGGER_H_
