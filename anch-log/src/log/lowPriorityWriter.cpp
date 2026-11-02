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
#include "log/lowPriorityWriter.hpp"

using anch::log::LowPriorityWriter;

// Constructors +
LowPriorityWriter::LowPriorityWriter(const anch::conf::Section& conf):
  FileWriter(conf),
  _messages(),
  _mutex(),
  _wait(),
  _stop(false),
  _thread() {
  startTreatment();
}
// Constructors -

// Destructor +
LowPriorityWriter::~LowPriorityWriter() {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _stop = true;
  }
  _wait.notify_all();
  if(_thread.joinable()) {
    _thread.join(); // queued messages are written before the thread stops
  }
}
// Destructor -

// Methods +
void
LowPriorityWriter::write(const std::string& message) {
  std::string line = format(message); // MDC is thread local: format in the logging thread
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _messages.push(std::move(line));
  }
  _wait.notify_one();
}

void
LowPriorityWriter::process() {
  std::unique_lock<std::mutex> lock(_mutex);
  while(true) {
    _wait.wait(lock, [this]() { return _stop || !_messages.empty(); });
    if(_messages.empty()) { // stopped and every message has been written
      return;
    }
    std::queue<std::string> messages;
    messages.swap(_messages); // write without lock
    lock.unlock();
    while(!messages.empty()) {
      writeLine(messages.front());
      messages.pop();
    }
    _output.flush(); // once per batch
    lock.lock();
  }
}

void
LowPriorityWriter::startTreatment() {
  std::lock_guard<std::mutex> lock(_mutex);
  if(!_thread.joinable()) {
    _thread = std::jthread(&LowPriorityWriter::process, this);
  }
}
// Methods -
