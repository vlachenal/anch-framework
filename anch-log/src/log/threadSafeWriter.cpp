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
#include "log/threadSafeWriter.hpp"


using std::string;
using std::ostream;

using anch::log::ThreadSafeWriter;
using anch::log::Level;


// Constructors +
/*
ThreadSafeWriter::ThreadSafeWriter(const string& fileName,
				   const string& linePattern,
				   uint32_t maxSize,
				   int maxIndex):
  Writer(fileName,linePattern,maxSize,maxIndex), _mutex() {
  // Nothing to do
}

ThreadSafeWriter::ThreadSafeWriter(ostream* output, const string& linePattern):
  Writer(output,linePattern), _mutex() {
  // Nothing to do
}
*/
ThreadSafeWriter::ThreadSafeWriter(const anch::conf::Section& conf):
  FileWriter(conf),
  _mutex() {
  // Nothing to do
}
// Constructors -


// Destructor +
ThreadSafeWriter::~ThreadSafeWriter() {
  // Nothing to do
}
// Destructor -


// Methods +
void
ThreadSafeWriter::write(const string& message) {
  _mutex.lock();
  FileWriter::write(message);
  _mutex.unlock();
}
// Methods -
