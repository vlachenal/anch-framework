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
#include "logger/logger.hpp"

using std::string;
using std::vector;

using anch::logger::Logger;
using anch::logger::Level;
using anch::logger::Writer;


/**
 * {@link Logger} constructor.
 *
 * @param name Logger name
 * @param level The logging level to set
 * @param writers The writers list
 */
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

/**
 * {@link Logger} destructor
 */
Logger::~Logger() {
  // Nothing to do
}
