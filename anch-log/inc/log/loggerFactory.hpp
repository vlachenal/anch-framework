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

#include "log/loggerConfiguration.hpp"
#include "log/logger.hpp"
#include "log/writer.hpp"

#include "singleton.hpp"

#include "conf/configuration.hpp"
#include "conf/section.hpp"


namespace anch::log {

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

  private:
    // Attributes +
    /*! Loggers configuration */
    std::map<std::string, anch::log::LoggerConfiguration> _loggersConfig;

    /*! Register loggers to free them */
    std::map<std::string, anch::log::Logger*> _loggers;
    // Attributes -

  private:
    // Constructor +
    /*!
     * \ref LoggerFactory private constructor
     */
    LoggerFactory();
    // Constructor -

  private:
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
    static const anch::log::Logger& getLogger(const std::string& loggerName);

  private:
    /*!
     * Initialize writers
     *
     * \param writers The writers container
     * \param conf the configuration file
     */
    void initializeWriters(std::map<std::string, anch::log::Writer*>& writers,
			   const anch::conf::Section* conf);

    /*!
     * Create default logger
     *
     * \param writers the writers to use
     */
    void createDefaultLogger(const std::map<std::string, anch::log::Writer*>& writers);

    /*!
     * Initialize loggers configuration
     *
     * \param writers The configured writers
     * \param conf The configuration file
     */
    void initializeLoggersConfiguration(const std::map<std::string, anch::log::Writer*>& writers,
					const anch::conf::Section* conf);

    /*!
     * Load default configuration
     */
    void loadDefaultConfiguration();
    // Methods -

  };

}
