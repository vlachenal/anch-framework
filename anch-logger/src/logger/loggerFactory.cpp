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
#include "logger/loggerFactory.hpp"

#include <iostream>
#include <set>
#include <regex>

#include "logger/levels.hpp"
#include "logger/threadSafeWriter.hpp"
#include "logger/lowPriorityWriter.hpp"

using anch::logger::LoggerFactory;

std::string LoggerFactory::CONF_FILE("anch-logger.conf");

// Constructors +
LoggerFactory::LoggerFactory(): _loggersConfig() {
  try {
    auto resource = anch::resource::Resource::getResource(CONF_FILE);
    std::map<std::string,anch::logger::Writer*> writers;
    initializeWriters(writers, resource);
    initializeLoggersConfiguration(writers, resource);

  } catch(...) {
    loadDefaultConfiguration();
  }
}
// Constructors -

// Destructor +
LoggerFactory::~LoggerFactory() {
  std::set<anch::logger::Writer*> writers;
  for(size_t i = 0 ; i < _loggersConfig.size() ; i++) {
    const std::vector<anch::logger::Writer*>& confWriters = _loggersConfig[i].getWriters();
    for(anch::logger::Writer* writer : confWriters) {
      writers.insert(writer);
    }
  }
  for(anch::logger::Writer* writer : writers) {
    delete writer;
  }
  for(anch::logger::Logger* logger : _loggers) {
    delete logger;
  }
}
// Destructor -

// Methods +
const anch::logger::Logger&
LoggerFactory::getLogger(const std::string& loggerName) {
  LoggerFactory& self = LoggerFactory::getInstance();

  const anch::logger::LoggerConfiguration* loggerConfig = NULL;
  for(size_t i = 0 ; i < self._loggersConfig.size() ; i++) {
    const anch::logger::LoggerConfiguration& config = self._loggersConfig[i];
    const std::string& confName = config.getCategory();
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
  anch::logger::Logger* logger = NULL;
  if(loggerConfig != NULL) {
    logger = new anch::logger::Logger(loggerName,
				      loggerConfig->getLevel(),
				      loggerConfig->getWriters());
    self._loggers.push_back(logger);
  }
  // Instanciate a new logger from configuration -

  return *logger;
}

anch::logger::Writer*
LoggerFactory::createWriterInstance(bool threadSafe,
				    bool lowPriority,
				    std::ostream* out,
				    const std::string& pattern) {
  anch::logger::Writer* writer = NULL;
  if(lowPriority) {
    writer = new anch::logger::LowPriorityWriter(out,pattern);
    static_cast<anch::logger::LowPriorityWriter*>(writer)->startTreatment();
  } else if(threadSafe) {
    writer = new anch::logger::ThreadSafeWriter(out,pattern);
  } else {
    writer = new anch::logger::Writer(out,pattern);
  }
  return writer;
}

anch::logger::Writer*
LoggerFactory::createWriterInstance(bool threadSafe,
				    bool lowPriority,
				    const std::string& path,
				    const std::string& pattern,
				    uint32_t maxSize,
				    int maxIndex) {
  anch::logger::Writer* writer = NULL;
  if(lowPriority) {
    writer = new anch::logger::LowPriorityWriter(path,pattern,maxSize,maxIndex);
    static_cast<anch::logger::LowPriorityWriter*>(writer)->startTreatment();
  } else if(threadSafe) {
    writer = new anch::logger::ThreadSafeWriter(path,pattern,maxSize,maxIndex);
  } else {
    writer = new anch::logger::Writer(path,pattern,maxSize,maxIndex);
  }
  return writer;
}

void
LoggerFactory::initializeWriters(std::map<std::string,anch::logger::Writer*>& writers,
				 const anch::resource::Resource& resource) {
  std::map<std::string,unsigned int> sizeMap = std::map<std::string,unsigned int>({
      {"K",1024},
      {"M",1024*1024},
      {"G",1024*1024*1024}
    });
  const std::regex sizeRegex = std::regex("^([0-9]+)(K|M|G)?$");

  // Writer QoS configuration +
  bool globalThreadSafe = true;
  std::string paramValue;
  if(resource.getParameter(paramValue, "thread.safe")) {
    globalThreadSafe = (paramValue == "1");
  }
  paramValue = "";
  bool globalLowPriority = false;
  if(resource.getParameter(paramValue, "low.priority")) {
    globalLowPriority = (paramValue == "1");
  }
  paramValue = "";
  // Writer QoS configuration -

  std::smatch match;
  const std::map<std::string,anch::resource::Section>& config = resource.getConfiguration();
  for(auto iter = config.cbegin() ; iter != config.cend() ; iter++) {
    if(iter->first.substr(0,8) == "WRITER::") {
      std::string writer = iter->first;
      std::string name = writer.substr(8);
      // Retrieve log pattern +
      std::string pattern;
      bool found = resource.getParameter(pattern,
					 "writer.pattern",
					 writer);
      if(!found) {
	pattern = "%m";
      }
      // Retrieve log pattern -

      // Writer QoS configuration +
      bool threadSafe = globalThreadSafe;
      if(resource.getParameter(paramValue, "writer.thread.safe", writer)) {
	threadSafe = (paramValue == "1");
      }
      paramValue = "";
      bool lowPriority = globalLowPriority;
      if(resource.getParameter(paramValue, "writer.low.priority", writer)) {
	lowPriority = (paramValue == "1");
      }
      // Writer QoS configuration -

      if(name == "console") { // Console writer will put logs on standard output
	writers[name] = createWriterInstance(threadSafe,
					     lowPriority,
					     (std::ostream*)(&std::cout),
					     pattern);

      } else { // File writer
	// File path +
	std::string path;
	found = resource.getParameter(path, "writer.filepath", writer);
	// File path -
	if(found) {
	  // Max file size +
	  std::string maxSizeStr;
	  unsigned int maxSize = 0;
	  resource.getParameter(maxSizeStr, "writer.max.size", writer);
	  if(regex_search(maxSizeStr, match, sizeRegex)) {
	    const std::string multStr = std::string(match[2].first,
						    match[2].second);
	    maxSize = static_cast<unsigned int>(std::stoi(std::string(match[1].first,
								      match[1].second).data()));
	    if(multStr != "") {
	      maxSize *= sizeMap[multStr];
	    }
	  }
	  // Max file size -

	  // Max file index on rotate +
	  int maxIndex = 0;
	  std::string maxIdxStr;
	  resource.getParameter(maxIdxStr,
				"writer.max.rotate.index",
				writer);
	  if(found) {
	    maxIndex = std::stoi(maxIdxStr);
	  }
	  // Max file index on rotate -
	  writers[name] = createWriterInstance(threadSafe,
					       lowPriority,
					       path,
					       pattern,
					       maxSize,
					       maxIndex);
	}
      }
    }
  }
}

void
LoggerFactory::initializeLoggersConfiguration(const std::map<std::string,anch::logger::Writer*>& writers,
					      const anch::resource::Resource& resource) {
  std::regex upperRegex = std::regex("[a-z]");
  std::smatch match;
  std::string upperRep = "[A-Z]";

  const std::map<std::string,anch::resource::Section>& config = resource.getConfiguration();
  for(auto iter = config.cbegin() ; iter != config.cend() ; iter++) {
    if(iter->first.substr(0,10) == "CATEGORY::") {
      std::string category = iter->first;
      std::string name = iter->first.substr(10);
      std::string writerStr;
      bool found = resource.getParameter(writerStr,
					 "logger.writers",
					 category);
      if(found) {
	// Retrieve writers +
	size_t pos = 0;
	std::vector<anch::logger::Writer*> loggerWriters;
	size_t nextPos = writerStr.find(',',pos);
	while(nextPos != std::string::npos && nextPos != pos) {
	  const std::string writerName = writerStr.substr(pos,
							  nextPos - pos);
	  auto iter = writers.find(writerName);
	  if(iter != writers.cend()) {
	    loggerWriters.push_back(iter->second);
	  }
	  pos = nextPos + 1;
	  nextPos = writerStr.find(',',pos);
	}
	const std::string writerName = writerStr.substr(pos);
	auto iter = writers.find(writerName);
	if(iter != writers.cend()) {
	  loggerWriters.push_back(iter->second);
	}
	// Retrieve writers -

	// Retrieve level +
	if(loggerWriters.empty()) {
	  std::cerr << "No writers has been found for " << name << ": "
		    << writerStr << std::endl;

	} else {
	  std::string levelLbl;
	  resource.getParameter(levelLbl, "logger.level", category);
	  std::transform(levelLbl.begin(),
			 levelLbl.end(),
			 levelLbl.begin(),
			 ::toupper);
	  anch::logger::Level level = anch::logger::Level::FATAL;
	  auto iterLvl = anch::logger::LABEL_LEVEL.find(levelLbl);
	  if(iterLvl == anch::logger::LABEL_LEVEL.cend()) {
	    std::cerr << "Invalid level: " << levelLbl << std::endl;
	  } else {
	    level = iterLvl->second;
	  }
	  _loggersConfig.push_back(anch::logger::LoggerConfiguration(name,
								     level,
								     loggerWriters));
	}
	// Retrieve level -
      }
    }
  }
}

void
LoggerFactory::loadDefaultConfiguration() {
  _loggersConfig.clear();
  std::cerr <<  "Logger configuration file " << CONF_FILE << " has not been found or is not readable." << std::endl;
  std::cerr << "Default configuration is loading." << std::endl;
  std::cerr << "Everything will be logged in console." << std::endl;
  ThreadSafeWriter* console = new ThreadSafeWriter((std::ostream*)(&std::cout), "$d{%Y-%m-%d %H:%M:%S} - $m");
  std::vector<anch::logger::Writer*> loggerWriters;
  loggerWriters.push_back(console);
  loggerWriters.shrink_to_fit();
  _loggersConfig.push_back(anch::logger::LoggerConfiguration("default",
							     anch::logger::Level::TRACE,
							     loggerWriters));
}
// Methods -
