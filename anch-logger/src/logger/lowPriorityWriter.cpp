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
#include "logger/lowPriorityWriter.hpp"

#include <chrono>


using std::string;
using std::ostream;
using std::mutex;
using std::queue;

using anch::logger::LowPriorityWriter;
using anch::logger::Writer;
using anch::logger::Level;


// Constructors +
/*!
 * \ref LowPriorityWriter constructor
 *
 * \param fileName The file name
 * \param linePattern The line pattern
 * \param maxSize The file maximum size before file rotation
 * \param maxIndex The maximum number of log files to keep
 */
LowPriorityWriter::LowPriorityWriter(const string& fileName,
				     const string& linePattern,
				     int maxSize,
				     int maxIndex) : Writer(fileName,linePattern,maxSize,maxIndex),
						     _messages(),
						     _mutex() {
  // Nothing to do
}

/*!
 * \ref LowPriorityWriter constructor
 *
 * \param output The output to use
 * \param linePattern The line pattern
 */
LowPriorityWriter::LowPriorityWriter(ostream* output,
				     const string& linePattern) : Writer(output,linePattern),
								  _messages(),
								  _mutex() {
  // Nothing to do
}
// Constructors -


// Destructor +
/*!
 * \ref LowPriorityWriter destructor
 */
LowPriorityWriter::~LowPriorityWriter() {
  _running = false;
  _thread->join();
}
// Destructor -


// Methods +
/*!
 * Write message in the file
 *
 * \param category The logger category
 * \param level The message level
 * \param message Message to write
 */
void
LowPriorityWriter::write(const string& category,
			 const Level& level,
			 const string& message) {
  _mutex.lock();
  _messages.push(_formatter.formatMessage(category, level, message) + "\n");
  _mutex.unlock();
}

/*!
 * Thread process method
 */
void
LowPriorityWriter::process() {
  while(_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    _mutex.lock();
    bool empty = _messages.empty();
    _mutex.unlock();
    while(!empty) {
      _mutex.lock();
      const string& msg = _messages.front();
      _mutex.unlock();
      *_output << msg;
      if(rotate()) {
	rotateFiles();
      }
      _mutex.lock();
      _messages.pop();
      empty = _messages.empty();
      _mutex.unlock();
    }
  }
}

/*!
 * Start messages queue poolingg
 */
void
LowPriorityWriter::startTreatment() {
  _running = true;
  _thread = new std::thread(&LowPriorityWriter::process, this);
}
// Methods -
