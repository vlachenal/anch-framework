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

#include "log/levels.hpp"
#include "log/writer.hpp"

namespace anch::log {

  /*!
   * Loggers configuration extract from configuration file
   *
   * \author Vincent Lachenal
   */
  class LoggerConfiguration {
  private:
    /*! Logger category */
    std::string _category;

    /*! Logging level */
    anch::log::Level _level;

    /*! Writers */
    std::vector<anch::log::Writer*> _writers;

  public:
    // Constructor +
    /*!
     * \ref LoggerConfiguration constructor
     *
     * \param category The category
     * \param level The logging level
     * \param writers The writers
     */
    LoggerConfiguration(const std::string& category,
			const anch::log::Level& level,
			const std::vector<anch::log::Writer*>& writers);
    // Constructor -

    // Destructor +
    /*!
     * \ref LoggerConfiguration destructor
     */
    ~LoggerConfiguration();
    // Destructor -

  public:
    // Accessors +
    /*!
     * Category getter
     *
     * \return The category
     */
    const std::string& getCategory() const;

    /*!
     * Logging level getter
     *
     * \return The logging level
     */
    const anch::log::Level& getLevel() const;

    /*!
     * Writers getter
     *
     * \return The writers
     */
    const std::vector<anch::log::Writer*>& getWriters() const;
    // Accessors -

  };

}

#include "log/impl/loggerConfiguration.hpp"
