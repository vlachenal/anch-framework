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
#include <set>

#include <boost/regex.hpp>

#include "logger/writer.hpp"
#include "logger/levels.hpp"


using std::string;
using std::mutex;
using std::map;
using std::vector;
using std::set;
using std::ostream;
using std::cout;
using std::cerr;
using std::endl;

using boost::regex;
using boost::smatch;
using boost::regex_search;
using boost::regex_replace;

using anch::resource::Resource;
using anch::resource::Section;

using anch::logger::LoggerFactory;
using anch::logger::LoggerConfiguration;
using anch::logger::Logger;
using anch::logger::Writer;
using anch::logger::Level;
using anch::logger::LABEL_LEVEL;


// Static attributes initialization +
mutex LoggerFactory::MUTEX;
LoggerFactory* LoggerFactory::_self = NULL;
vector<LoggerConfiguration> LoggerFactory::CONFIG;
// Static attributes initialization -


/**
 * {@link LoggerFactory} private constructor
 */
LoggerFactory::LoggerFactory():
  _config(Resource::getResource("anch-logger.conf")) {
  std::atexit(LoggerFactory::cleanWriters);
  map<string,Writer*> writers;
  initializeWriters(writers);
  initializeLoggersConfiguration(writers);
}

/**
 * {@link LoggerFactory} destructor
 */
LoggerFactory::~LoggerFactory() {
  // Nothing to do
}

/**
 * Initialize writers
 */
void
LoggerFactory::initializeWriters(map<string,Writer*>& writers) {
  map<string,unsigned int> sizeMap = map<string,unsigned int>({
      {"K",1024},
      {"M",1024*1024},
      {"G",1024*1024*1024}
    });
  const regex sizeRegex = regex("^([0-9]+)(K|M|G)?$");

  smatch match;
  const map<string,Section>& config = _config.getConfiguration();
  for(auto iter = config.cbegin() ; iter != config.cend() ; iter++) {
    if(iter->first.substr(0,8) == "WRITER::") {
      string writer = iter->first;
      string name = writer.substr(8);
      // Retrieve log pattern +
      string pattern;
      bool found = _config.getParameter(pattern, "writer.pattern", writer);
      if(!found) {
	pattern = "%m";
      }
      // Retrieve log pattern -

      if(name == "console") { // Console writer will put logs on standard output
	writers[name] = new Writer((ostream*)(&cout), pattern);

      } else { // File writer
	// File path +
	string path;
	found = _config.getParameter(path, "writer.filepath", writer);
	// File path -
	if(found) {
	  // Max file size +
	  string maxSizeStr;
	  unsigned int maxSize = 0;
	  _config.getParameter(maxSizeStr, "writer.max.size", writer);
	  if(regex_search(maxSizeStr, match, sizeRegex)) {
	    const string multStr = string(match[2].first, match[2].second);
	    maxSize = std::stoi(string(match[1].first, match[1].second).c_str());
	    if(multStr != "") {
	      maxSize *= sizeMap[multStr];
	    }
	  }
	  // Max file size -

	  // Max file index on rotate +
	  int maxIndex = 0;
	  string maxIdxStr;
	  _config.getParameter(maxIdxStr, "writer.max.rotate.index", writer);
	  if(found) {
	    maxIndex = std::stoi(maxIdxStr);
	  }
	  // Max file index on rotate -
	  writers[name] = new Writer(path, pattern, maxSize, maxIndex);
	}
      }
    }
  }
}

/**
 * Initialize loggers configuration
 */
void
LoggerFactory::initializeLoggersConfiguration(const map<string,Writer*>& writers) {
  regex upperRegex = regex("[a-z]");
  string upperRep = "[A-Z]";
  smatch match;

  const map<string,Section>& config = _config.getConfiguration();
  for(auto iter = config.cbegin() ; iter != config.cend() ; iter++) {
    if(iter->first.substr(0,10) == "CATEGORY::") {
      string category = iter->first;
      string name = iter->first.substr(10);
      string writerStr;
      bool found = _config.getParameter(writerStr, "logger.writers", category);
      if(found) {
	// Retrieve writers +
	size_t pos = 0;
	vector<Writer*> loggerWriters;
	size_t nextPos = writerStr.find(',',pos);
	while(nextPos != string::npos && nextPos != pos) {
	  const string writerName = writerStr.substr(pos, nextPos - pos);
	  auto iter = writers.find(writerName);
	  if(iter != writers.cend()) {
	    loggerWriters.push_back(iter->second);
	  }
	  pos = nextPos + 1;
	  nextPos = writerStr.find(',',pos);
	}
	const string writerName = writerStr.substr(pos);
	auto iter = writers.find(writerName);
	if(iter != writers.cend()) {
	  loggerWriters.push_back(iter->second);
	}
	// Retrieve writers -

	// Retrieve level +
	if(loggerWriters.empty()) {
	  cerr << "No writers has been found for " << name << ": " << writerStr << endl;

	} else {
	  string levelLbl;
	  _config.getParameter(levelLbl, "logger.level", category);
	  std::transform(levelLbl.begin(), levelLbl.end(), levelLbl.begin(), ::toupper);
	  Level level = Level::FATAL;
	  auto iterLvl = LABEL_LEVEL.find(levelLbl);
	  if(iterLvl == LABEL_LEVEL.cend()) {
	    cerr << "Invalid level: " << levelLbl << endl;
	  } else {
	    level = iterLvl->second;
	  }
	  CONFIG.push_back(LoggerConfiguration(name, level, loggerWriters));
	}
	// Retrieve level -
      }
    }
  }
}

void
LoggerFactory::cleanWriters() {
  set<Writer*> writers;
  for(size_t i = 0 ; i < CONFIG.size() ; i++) {
    const vector<Writer*>& confWriters = CONFIG[i].getWriters();
    for(size_t j = 0 ; j < confWriters.size() ; j++) {
      writers.insert(confWriters[j]);
    }
  }
  for(auto iter = writers.begin() ; iter != writers.end() ; ++iter) {
    delete *iter;
  }
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
  MUTEX.unlock();
  const LoggerConfiguration* loggerConfig = NULL;
  for(size_t i = 0 ; i < CONFIG.size() ; i++) {
    const LoggerConfiguration& config = CONFIG[i];
    const string& confName = config.getCategory();
    // First check on size +
    if((loggerConfig != NULL
	&& loggerConfig->getCategory().size() > confName.size())
       || (confName.size() > loggerName.size()
	   && confName != "default")) {
      continue;
    }
    // First check on size -

    // Compare category name and logger name +
    if(loggerName.substr(0,confName.size()) == confName) {
      loggerConfig = &config;
      continue;
    }
    // Compare category name and logger name -

    // Check if default +
    if(loggerConfig == NULL && confName == "default") {
      loggerConfig = &config;
    }
    // Check if default -
  }

  // Instanciate a new logger from configuration +
  if(loggerConfig != NULL) {
    logger = new Logger(loggerName,
			loggerConfig->getLevel(),
			loggerConfig->getWriters());
  }
  // Instanciate a new logger from configuration -

  return *logger;
}
