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
#ifndef _ANCH_LOGGER_LOW_PRIORITY_WRITER_H_
#define _ANCH_LOGGER_LOW_PRIORITY_WRITER_H_

#include <iostream>
#include <queue>
#include <mutex>
#include <thread>

#include "logger/writer.hpp"


namespace anch {
  namespace logger {

    /*!
     * File writer with QoS implementation to avoid performance lose
     * when files are written.
     *
     * \author Vincent Lachenal
     */
    class LowPriorityWriter: public Writer {

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

    public:
      // Constructors +
      /*!
       * \ref LowPriorityWriter constructor
       *
       * \param fileName The file name
       * \param linePattern The line pattern
       * \param maxSize The file maximum size before file rotation
       * \param maxIndex The maximum number of log files to keep
       */
      LowPriorityWriter(const std::string& fileName,
			const std::string& linePattern,
			unsigned int maxSize = 0,
			int maxIndex = 0);

      /*!
       * \ref LowPriorityWriter constructor
       *
       * \param output The output to use
       * \param linePattern The line pattern
       */
      LowPriorityWriter(std::ostream* output, const std::string& linePattern);
      // Constructors -

      // Destructor +
      /*!
       * \ref LowPriorityWriter destructor
       */
      virtual ~LowPriorityWriter();
      // Destructor -


      // Method +
    public:
      /*!
       * Write message in the file
       *
       * \param category The logger category
       * \param level The message level
       * \param message Message to write
       */
      virtual void write(const std::string& category,
			 const anch::logger::Level& level,
			 const std::string& message);

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
}

#endif // _ANCH_LOGGER_LOW_PRIORITY_WRITER_H_
