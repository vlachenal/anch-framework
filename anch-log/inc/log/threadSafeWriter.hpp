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

#include <mutex>

#include "log/fileWriter.hpp"


namespace anch::log {

  /*!
   * File writer which manage conccurency access
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ThreadSafeWriter: public FileWriter {
  private:
    // Attributes +
    /*! Writer mutex */
    std::mutex _mutex;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref ThreadSafeWriter constructor
     *
     * \param conf the writer's configuration
     */
    ThreadSafeWriter(const anch::conf::Section& conf);
    // Constructors -

  public:
    // Destructor +
    /*!
     * \ref ThreadSafeWriter destructor
     */
    virtual ~ThreadSafeWriter();
    // Destructor -

  public:
    /*!
     * Write message in the file.\n
     * Rotate log file when needed.
     *
     * \param message Message to write
     */
    virtual void write(const std::string& message) override;

  };

}
