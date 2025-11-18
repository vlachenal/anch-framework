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

#include <string>

#include "log/fmt/messageFormatter.hpp"

#include "conf/section.hpp"


namespace anch::log {

  /*!
   * \brief Logger writter definition
   *
   * Provides format message function and define write function based on MDC.
   *
   * \author Vincent Lachenal
   */
  class Writer {

    // Attributes +
  private:
    /*! Message formatter */
    anch::log::fmt::MessageFormatter _formatter;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Writer constructor
     *
     * \param conf the writer's configuration
     */
    Writer(const anch::conf::Section& conf);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Writer destructor
     */
    virtual ~Writer();
    // Destructor -

    // Methods +
  public:
    /*!
     * Write message in the file
     *
     * \param message Message to write
     */
    virtual void write(const std::string& message) = 0;

  protected:
    /*!
     * Format message according to writer message formatter
     *
     * \param msg the message
     *
     * \return the formatted log line
     */
    std::string format(const std::string& msg);
    // Methods -

  };

}
