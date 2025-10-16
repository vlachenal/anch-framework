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

#include <map>
#include <vector>
#include <cstdint>

#include "logger/loggerConfiguration.hpp"
#include "logger/logger.hpp"
#include "logger/writer.hpp"
#include "resource/resource.hpp"
#include "singleton.hpp"


namespace anch::logger {

  /*!
   * \brief Logger factory.
   *
   * This class aims to manage logger according to the loggers name and configuration.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class LoggerFactory: public anch::Singleton<LoggerFactory> {
    friend class anch::Singleton<LoggerFactory>;

  public:
    /*! Configuration file path. Default: \c anch-logger.conf */
    static std::string CONF_FILE;

  private:
    // Attributes +
    /*! Loggers configuration */
    std::vector<anch::logger::LoggerConfiguration> _loggersConfig;

    /*! Register loggers to free them */
    std::vector<anch::logger::Logger*> _loggers;
    // Attributes -

  private:
    // Constructor +
    /*!
     * \ref LoggerFactory private constructor
     */
    LoggerFactory();
    // Constructor -

    // Destructor +
    /*!
     * \ref LoggerFactory destructor.\n
     * Clean every \ref Writer to flush their output and close files.
     */
    virtual ~LoggerFactory();
    // Destructor -

  public:
    // Methods +
    /*!
     * Retrieve a registered logger from a logger name or create a new one
     * and register it.
     *
     * \param loggerName The logger name to retrieve
     *
     * \return The loggerr instance
     */
    static const anch::logger::Logger& getLogger(const std::string& loggerName);

  private:
    /*!
     * Create writer instance according to writter configuration
     *
     * \param threadSafe Use a thread safe logger
     * \param lowPriority Use a low priority logger
     * \param out the output stream to use
     * \param pattern the logger pattern configuration
     */
    anch::logger::Writer* createWriterInstance(bool threadSafe, bool lowPriority, std::ostream* out, const std::string& pattern);

    /*!
     * Create writer instance according to writter configuration
     *
     * \param threadSafe use a thread safe logger
     * \param lowPriority use a low priority logger
     * \param path the file path
     * \param pattern the logger pattern configuration
     * \param maxSize the log files maximum size
     * \param maxIndex the maximum number of log file to keep
     */
    anch::logger::Writer* createWriterInstance(bool threadSafe, bool lowPriority, const std::string& path, const std::string& pattern, uint32_t maxSize, int maxIndex);

    /*!
     * Initialize writers
     *
     * \param writers The writers container
     * \param resource The resource configuration file
     */
    void initializeWriters(std::map<std::string,anch::logger::Writer*>& writers, const anch::resource::Resource& resource);

    /*!
     * Initialize loggers configuration
     *
     * \param writers The configured writers
     * \param resource The resource configuration file
     */
    void initializeLoggersConfiguration(const std::map<std::string,anch::logger::Writer*>& writers, const anch::resource::Resource& resource);

    /*!
     * Load default configuration
     */
    void loadDefaultConfiguration();
    // Methods -

  };

}
