/*
    This file is part of ANCH Framework.

    ANCH Framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ANCH Framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "logger/loggerFactory.hpp"

#include <vector>

#include "logger/writer.hpp"
#include "logger/levels.hpp"


using std::string;
using std::mutex;
using std::map;
using std::vector;
using std::ostream;

using anch::logger::LoggerFactory;
using anch::logger::Logger;
using anch::logger::Writer;
using anch::logger::Level;


// Static attributes initialization +
mutex LoggerFactory::MUTEX;
map<string,Logger*> LoggerFactory::LOGGERS;
LoggerFactory* LoggerFactory::_self = NULL;
// Static attributes initialization -


/**
 * {@link LoggerFactory} private constructor
 */
LoggerFactory::LoggerFactory() {
  // Nothing to do
}

/**
 * {@link LoggerFactory} destructor
 */
LoggerFactory::~LoggerFactory() {
  
}

/**
 * Retrieve a registered logger from a logger name or create a new one
 * and register it.
 *
 * @param loggerName The logger name to retrieve
 *
 * @return The loggerr instance
 */
const Logger&
LoggerFactory::getLogger(const string& loggerName) {
  Logger* logger = NULL;
  MUTEX.lock();
  if(_self == NULL) {
    _self = new LoggerFactory();
  }
  auto iter = LOGGERS.find(loggerName);
  if(iter == LOGGERS.cend()) {
    // TODO manage logger configurations ...
    vector<Writer*> writers;
    writers.push_back(new Writer((ostream*)(&std::cout), "$d{%Y-%m-%d %H:%M:%S} - $c [Thread $t] - $p - $m"));
    logger = new Logger(loggerName, Level::TRACE, writers);
    LOGGERS[loggerName] = logger;
  } else {
    logger = iter->second;
  }
  MUTEX.unlock();
  return *logger;
}
