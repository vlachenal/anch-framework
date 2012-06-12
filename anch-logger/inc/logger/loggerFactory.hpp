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
#ifndef _ANCH_LOGGER_LOGGER_FACTORY_H_
#define _ANCH_LOGGER_LOGGER_FACTORY_H_

#include <iostream>
#include <map>
#include <mutex>

#include "logger/logger.hpp"


namespace anch {
  namespace logger {

    /**
     * Logger factory.<br>
     * This class aims to manage logger according to the loggers name and configuration.
     *
     * @author Vincent Lachenal
     */
    class LoggerFactory {
    private:
      /** Mutex for conccurency access */
      static std::mutex MUTEX;

      /** Registered logger */
      static std::map<std::string,anch::logger::Logger*> LOGGERS;

      /** {@link LoggerFactory} unique instance */
      static LoggerFactory* _self;

    private:
      /**
       * {@link LoggerFactory} private constructor
       */
      LoggerFactory();

      /**
       * {@link LoggerFactory} destructor
       */
      virtual ~LoggerFactory();

    public:
      /**
       * Retrieve a registered logger from a logger name or create a new one
       * and register it.
       *
       * @param loggerName The logger name to retrieve
       *
       * @return The loggerr instance
       */
      static const anch::logger::Logger& getLogger(const std::string& loggerName);
    };

  }
}

#endif // _ANCH_LOGGER_LOGGER_FACTORY_H_
