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
#include "log/logger.hpp"

#include "log/mdc.hpp"

using std::string;
using std::vector;

using anch::log::Logger;
using anch::log::Level;
using anch::log::Writer;


Logger::Logger(const string& name,
	       const Level level,
	       const vector<Writer*>& writers):
  _name(name),
  _level(level),
  _writers() {
  for(size_t i = 0 ; i < writers.size() ; i++) {
    _writers.push_back(writers[i]);
  }
}

Logger::~Logger() {
  // Nothing to do
}

bool
Logger::isEnabled(const Level& level) const noexcept {
  return level >= _level && !_writers.empty();
}

void
Logger::write(const Level& level, const string& message) const noexcept {
  try {
    anch::log::MDC.put("level", anch::log::LEVEL_LABEL.at(level));
    anch::log::MDC.put("logger", _name);
  } catch(...) {
    // Logging must never stop the application (this method is noexcept)
    return;
  }
  for(Writer* writer : _writers) {
    try {
      writer->write(message);
    } catch(...) {
      // A writer error must not prevent other writers to log, nor stop the application
    }
  }
}
