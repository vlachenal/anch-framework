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

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "log/fileWriter.hpp"


namespace anch::log {

  /*!
   * File writer with QoS implementation to avoid performance lose
   * when files are written.\n
   * Messages are formatted in the logging thread and written by a dedicated thread.
   * The queued messages are written before destruction.
   *
   * \author Vincent Lachenal
   */
  class LowPriorityWriter: public anch::log::FileWriter {

    // Attributes +
  private:
    /*! Messages queue */
    std::queue<std::string> _messages;

    /*! Writer mutex (protects messages queue and stop flag) */
    std::mutex _mutex;

    /*! The writing thread waits for messages on this condition */
    std::condition_variable _wait;

    /*! The writing thread stops when \c true and the queue is empty */
    bool _stop;

    /*! Writing thread (declared last: joined first on destruction) */
    std::jthread _thread;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref LowPriorityWriter constructor.\n
     * Start the writing thread.
     *
     * \param conf the writer's configuration
     */
    LowPriorityWriter(const anch::conf::Section& conf);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref LowPriorityWriter destructor.\n
     * Write the queued messages and stop the writing thread.
     */
    virtual ~LowPriorityWriter();
    // Destructor -

    // Method +
  public:
    /*!
     * Queue message to write in the file.
     *
     * \param message Message to write
     */
    virtual void write(const std::string& message) override;

    /*!
     * Start messages queue pooling.\n
     * The writing thread is started by the constructor: this method does nothing when it is running.
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
