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
#include "logger/threadSafeWriter.hpp"


using std::string;
using std::ostream;

using anch::logger::Writer;
using anch::logger::ThreadSafeWriter;
using anch::logger::Level;


/*!
 * \ref ThreadSafeWriter constructor
 *
 * \param fileName The file name
 * \param linePattern The line pattern
 * \param maxSize The file maximum size before file rotation
 * \param maxIndex The maximum number of log files to keep
 */
ThreadSafeWriter::ThreadSafeWriter(const string& fileName,
				   const string& linePattern,
				   int maxSize,
				   int maxIndex):  Writer(fileName,linePattern,maxSize,maxIndex),
						   _mutex() {
  // Nothing to do
}

/*!
 * \ref ThreadSafeWriter constructor
 *
 * \param output The output to use
 * \param linePattern The line pattern
 */
ThreadSafeWriter::ThreadSafeWriter(ostream* output,
				   const string& linePattern): Writer(output,linePattern),
							       _mutex() {
  // Nothing to do
}

/*!
 * \ref ThreadSafeWriter destructor
 */
ThreadSafeWriter::~ThreadSafeWriter() {
  // Nothing to do
}

/*!
 * Write message in the file
 *
 * \param category The logger category
 * \param level The message level
 * \param message Message to write
 */
void
ThreadSafeWriter::write(const string& category,
			const Level& level,
			const string& message) {
  _mutex.lock();
  Writer::write(category,level,message);
  _mutex.unlock();
}
