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

#include <mutex>

#include "log/writer.hpp"


namespace anch::log {

  /*!
   * \brief Log file writer
   *
   * Manage file writer with file rotation on size, date and max rotate index
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ConsoleWriter: public Writer {

    // Attributes +
  private:
    /*! Mutual exclusion object */
    std::mutex _mutex;

    /*! Console color flag */
    bool _color;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ConsoleWriter constructor
     *
     * \param conf the writer's configuration
     */
    ConsoleWriter(const anch::conf::Section& conf);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ConsoleWriter destructor
     */
    virtual ~ConsoleWriter();
    // Destructor -

    // Methods +
  public:
    /*!
     * Write message in the file.\n
     * Rotate log file when needed.
     *
     * \param message Message to write
     */
    virtual void write(const std::string& message);
    // Methods -

  };

}
