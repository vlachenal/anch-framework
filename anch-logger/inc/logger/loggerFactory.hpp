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
#ifndef _ANCH_LOGGER_LOGGER_FACTORY_H_
#define _ANCH_LOGGER_LOGGER_FACTORY_H_

// Add default logger configuration file to anch-logger.conf +
#ifndef _ANCH_LOGGER_CONFIG_FILE_
#define _ANCH_LOGGER_CONFIG_FILE_ "anch-logger.conf"
#endif // _ANCH_LOGGER_CONFIG_FILE_
// Add default logger configuration file to anch-logger.conf -

#include <iostream>
#include <map>
#include <mutex>
#include <vector>
#include <set>

#include <boost/regex.hpp>

#include "logger/levels.hpp"
#include "logger/loggerConfiguration.hpp"
#include "logger/logger.hpp"
#include "logger/writer.hpp"
#include "resource/resource.hpp"


namespace anch {
  namespace logger {

    /**
     * Logger factory.<br>
     * This class aims to manage logger according to the loggers name and configuration.
     *
     * @author Vincent Lachenal
     */
    class LoggerFactory {
    private:
      // Attributes +
      /** Mutex for conccurency access */
      static std::mutex MUTEX;

      /** {@link LoggerFactory} unique instance */
      static LoggerFactory* _self;

      /** Loggers configuration */
      std::vector<LoggerConfiguration> _loggersConfig;
      // Attributes -

    private:
      // Constructor +
      /**
       * {@link LoggerFactory} private constructor
       */
      LoggerFactory(): _loggersConfig() {
        anch::resource::Resource resource = anch::resource::Resource::getResource(_ANCH_LOGGER_CONFIG_FILE_);
	std::atexit(LoggerFactory::cleanWriters);
	std::map<std::string,anch::logger::Writer*> writers;
	initializeWriters(writers, resource);
	initializeLoggersConfiguration(writers, resource);
      }
      // Constructor -

      // Destructor +
      /**
       * {@link LoggerFactory} destructor
       */
      virtual ~LoggerFactory() {
	// Nothing to do
      };
      // Destructor -

    public:
      // Methods +
      /**
       * Retrieve a registered logger from a logger name or create a new one
       * and register it.
       *
       * @param loggerName The logger name to retrieve
       *
       * @return The loggerr instance
       */
      static const anch::logger::Logger& getLogger(const std::string& loggerName) {
	MUTEX.lock();
	if(_self == NULL) {
	  _self = new LoggerFactory();
	}
	MUTEX.unlock();

	const anch::logger::LoggerConfiguration* loggerConfig = NULL;
	for(size_t i = 0 ; i < _self->_loggersConfig.size() ; i++) {
	  const anch::logger::LoggerConfiguration& config = _self->_loggersConfig[i];
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
	}
	// Instanciate a new logger from configuration -

	return *logger;
      }

      /**
       * Clean every {@link anch::logger::Writer} to flush their output and close files.
       *
       * {@link std::atexit} is already map on it. But you can use it if you trap signals.
       */
      static void cleanWriters() {
	std::set<anch::logger::Writer*> writers;
	for(size_t i = 0 ; i < _self->_loggersConfig.size() ; i++) {
	  const std::vector<anch::logger::Writer*>& confWriters = _self->_loggersConfig[i].getWriters();
	  for(size_t j = 0 ; j < confWriters.size() ; j++) {
	    writers.insert(confWriters[j]);
	  }
	}
	for(auto iter = writers.begin() ; iter != writers.end() ; ++iter) {
	  delete *iter;
	}
      }

    private:
      /**
       * Initialize writers
       *
       * @param writers The writers container
       * @param resource The resource configuration file
       */
      void initializeWriters(std::map<std::string,anch::logger::Writer*>& writers,
			     anch::resource::Resource& resource) {
	std::map<std::string,unsigned int> sizeMap = std::map<std::string,unsigned int>({
	    {"K",1024},
	    {"M",1024*1024},
	    {"G",1024*1024*1024}
	  });
	const boost::regex sizeRegex = boost::regex("^([0-9]+)(K|M|G)?$");

	boost::smatch match;
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

	    if(name == "console") { // Console writer will put logs on standard output
	      writers[name] = new anch::logger::Writer((std::ostream*)(&std::cout),
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
		  maxSize = std::stoi(std::string(match[1].first,
						  match[1].second).data());
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
		writers[name] = new anch::logger::Writer(path,
							 pattern,
							 maxSize,
							 maxIndex);
	      }
	    }
	  }
	}
      }

      /**
       * Initialize loggers configuration
       *
       * @param writers The configured writers
       * @param resource The resource configuration file
       */
      void initializeLoggersConfiguration(const std::map<std::string,anch::logger::Writer*>& writers,
					  anch::resource::Resource& resource) {
	boost::regex upperRegex = boost::regex("[a-z]");
	std::string upperRep = "[A-Z]";
	boost::smatch match;

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
		_loggersConfig.push_back(LoggerConfiguration(name,
							     level,
							     loggerWriters));
	      }
	      // Retrieve level -
	    }
	  }
	}
      }
      // Methods -

    };

  }
}

// This macro has to be called by the programs only once
#define ANCH_LOGGER_INIT std::mutex anch::logger::LoggerFactory::MUTEX;\
anch::logger::LoggerFactory* anch::logger::LoggerFactory::_self = NULL;


#endif // _ANCH_LOGGER_LOGGER_FACTORY_H_
