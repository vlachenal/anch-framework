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
#include <queue>
#include <mutex>
#include <thread>

#include "log/fileWriter.hpp"


namespace anch::log {

  /*!
   * File writer with QoS implementation to avoid performance lose
   * when files are written.
   *
   * \author Vincent Lachenal
   */
  class LowPriorityWriter: public anch::log::FileWriter {

    // Attributes +
  private:
    /*! Messages queue */
    std::queue<std::string> _messages;

    /*! Running state */
    bool _running;

    /*! Treatment thread */
    std::thread* _thread;

    /*! Writer mutex */
    std::mutex _mutex;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref LowPriorityWriter constructor
     *
     * \param conf the writer's configuration
     */
    LowPriorityWriter(const anch::conf::Section& conf);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref LowPriorityWriter destructor
     */
    virtual ~LowPriorityWriter();
    // Destructor -

    // Method +
  public:
    /*!
     * Write message in the file.\n
     * Rotate log file when needed.
     *
     * \param message Message to write
     */
    virtual void write(const std::string& message) override;

    /*!
     * Start messages queue pooling
     */
    void startTreatment();

  private:
    /*!
     * Process messages in queue in a separated thread
     */
    void process();
    // Method -

  };

}
