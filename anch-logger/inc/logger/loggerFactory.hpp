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

#include "logger/loggerConfiguration.hpp"
#include "logger/logger.hpp"
#include "logger/writer.hpp"
#include "resource/resource.hpp"


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

      /** {@link LoggerFactory} unique instance */
      static LoggerFactory* _self;

      /** Loggers configuration */
      static std::vector<anch::logger::LoggerConfiguration> CONFIG;

      /** Loggers/writters configuration */
      anch::resource::Resource _config;

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

      /**
       * Clean every {@link anch::logger::Writer} to flush their output and close files.
       *
       * {@link std::atexit} is already map on it. But you can use it if you trap signals.
       */
      static void cleanWriters();

    private:
      /**
       * Initialize writers
       */
      void initializeWriters(std::map<std::string,anch::logger::Writer*>& writers);

      /**
       * Initialize loggers configuration
       *
       * @param writers The configured writers
       */
      void initializeLoggersConfiguration(const std::map<std::string,anch::logger::Writer*>& writers);

    };

  }
}

#endif // _ANCH_LOGGER_LOGGER_FACTORY_H_
