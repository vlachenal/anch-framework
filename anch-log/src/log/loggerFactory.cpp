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
#include "log/loggerFactory.hpp"

#include <iostream>
#include <set>
#include <regex>

#include "log/levels.hpp"
#include "log/threadSafeWriter.hpp"
#include "log/lowPriorityWriter.hpp"
#include "log/writerRegistry.hpp"
#include "log/constants.hpp"

#include "conf/configuration.hpp"


using anch::log::LoggerFactory;
using anch::log::WriterRegistry;


// Constructors +
LoggerFactory::LoggerFactory(): _loggersConfig() {
  try {
    const anch::conf::Section* conf = anch::conf::Configuration::inst().section(anch::log::NS_LOG);
    if(conf == NULL) {
      std::cout << "Section " << anch::log::NS_LOG << " has not been found. Load default configuration." << std::endl;
      loadDefaultConfiguration();
    } else {
      std::map<std::string,anch::log::Writer*> writers;
      initializeWriters(writers, conf);
      initializeLoggersConfiguration(writers, conf);
    }

  } catch(...) {
    std::cerr << "Error while loading logger configuration. Load default configuration." << std::endl;
    loadDefaultConfiguration();
  }
}
// Constructors -

// Destructor +
LoggerFactory::~LoggerFactory() {
  // Writers will be deleted on application exit
  for(auto iter = _loggers.begin() ; iter != _loggers.end() ; ++iter) {
    delete iter->second;
  }
}
// Destructor -

// Methods +
std::vector<std::string>
tokenizeLoggerName(const std::string& loggerName) {
  std::vector<std::string> tokens;
  if(loggerName == anch::log::DEFAULT) {
    tokens.push_back(anch::log::DEFAULT);
    return tokens;
  }
  tokens.push_back(loggerName);
  std::string tmp(loggerName);
  std::size_t pos = loggerName.size() - 1;
  while((pos = tmp.rfind("::", pos)) != std::string::npos) {
    tmp = tmp.substr(0, pos);
    tokens.push_back(tmp);
  }
  tokens.push_back(anch::log::DEFAULT);
  return tokens;
}

const anch::log::Logger&
LoggerFactory::getLogger(const std::string& loggerName) {
  LoggerFactory& self = LoggerFactory::getInstance();

  // Return already registered logger instance +
  if(self._loggers.contains(loggerName)) {
    return *self._loggers.at(loggerName);
  }
  // Return already registered logger instance -

  // Create logger according loggers configuration +
  for(auto token : tokenizeLoggerName(loggerName)) {
    auto iter = self._loggersConfig.find(token);
    if(iter == self._loggersConfig.end()) {
      continue;
    }
    self._loggers.insert({loggerName, new anch::log::Logger(loggerName,
							    iter->second.getLevel(),
							    iter->second.getWriters())});
    break;
  }
  // Create logger according loggers configuration -

  // Raise error when logger has not been created (which means that default logger has not been declared) +
  if(!self._loggers.contains(loggerName)) {
    std::ostringstream oss;
    oss << "Logger " << loggerName << " not found and default logger has not been defined";
    throw std::invalid_argument(oss.str());
  }
  // Raise error when logger has not been created (which means that default logger has not been declared) -

  return *self._loggers.at(loggerName);
}

anch::log::Writer*
createDefaultWriter(const std::string& name) {
  // Create writer +
  // Create writer section +
  anch::conf::Section defSec;
  anch::conf::Section& writerSec = defSec.section(anch::log::LOGGER).section(name);
  writerSec.putValue(anch::log::MODULE, anch::log::WRITER_TERM)
    .putValue("pattern", "$d{%Y-%m-%d %H:%M:%S} - $m");
  // Create writer section -
  return WriterRegistry::getInstance().create(name, true, false, writerSec);
  // Create writer -
}


void
LoggerFactory::initializeWriters(std::map<std::string, anch::log::Writer*>& writers,
				 const anch::conf::Section* resource) {
  // Writer QoS configuration +
  bool threadSafe = resource->getValue<bool>(anch::log::THREAD_SAFE, true);
  bool lowPriority = resource->getValue<bool>(anch::log::LOW_PRIORITY, false);
  // Writer QoS configuration -

  // Register writers +
  auto wIter = resource->getSections().find(anch::log::WRITER);
  if(wIter == resource->getSections().end()) {
    std::cout << "Section " << anch::log::WRITER << " has not been found. Load default writer." << std::endl;
    writers[anch::log::WRITER_TERM] = createDefaultWriter(anch::log::WRITER_TERM);
    return;
  }
  const anch::conf::Section& wSec = wIter->second;
  for(auto iter = wSec.getSections().begin() ; iter != wSec.getSections().end() ; ++iter) {
    std::string name = iter->first;
    const anch::conf::Section& writer = iter->second;
    std::optional<std::string> type = writer.getValue<std::string>(anch::log::MODULE);
    if(!type.has_value()) {
      std::cerr << "Writer " << name << " type has not been found" << std::endl;
      continue;
    }
    if(!WriterRegistry::getInstance().contains(type.value())) {
      std::cerr << "Unkown writer type (" << type.value() << " has been found for writer " << name << std::endl;
      continue;
    }
    writers[name] = WriterRegistry::getInstance().create(name, threadSafe, lowPriority, writer);
  }
  // Register writers -

  // Register default writer when empty +
  if(writers.empty()) {
    std::cout << "No writer has not been registered. Load default writer." << std::endl;
    writers[anch::log::WRITER_TERM] = createDefaultWriter(anch::log::WRITER_TERM);
  }
  // Register default writer when empty -
}

anch::log::Level
toLevel(const std::string& levelLbl) {
  std::string tmp(levelLbl);
  std::transform(tmp.begin(),
		 tmp.end(),
		 tmp.begin(),
		 ::toupper);
  anch::log::Level level = anch::log::Level::WARN;
  auto iterLvl = anch::log::LABEL_LEVEL.find(tmp);
  if(iterLvl == anch::log::LABEL_LEVEL.cend()) {
    std::cerr << "Invalid level: " << levelLbl << std::endl;
  } else {
    level = iterLvl->second;
  }
  return level;
}

std::vector<anch::log::Writer*>
parseWriters(const std::string& logWriters, const std::map<std::string, anch::log::Writer*>& writers) {
  size_t pos = 0;
  std::vector<anch::log::Writer*> loggerWriters;
  size_t nextPos = logWriters.find(',',pos);
  while(nextPos != std::string::npos && nextPos != pos) {
    const std::string writerName = logWriters.substr(pos, nextPos - pos);
    auto iter = writers.find(writerName);
    if(iter != writers.cend()) {
      loggerWriters.push_back(iter->second);
    } else {
      std::cout << "Writer " << writerName << " has not been registered" << std::endl;
    }
    pos = nextPos + 1;
    nextPos = logWriters.find(',', pos);
  }
  const std::string writerName = logWriters.substr(pos);
  auto iterStr = writers.find(writerName);
  if(iterStr != writers.cend()) {
    loggerWriters.push_back(iterStr->second);
  }
  return loggerWriters;
}

std::vector<anch::log::Writer*>
getAllWriters(const std::map<std::string, anch::log::Writer*>& writers) {
  std::vector<anch::log::Writer*> loggerWriters;
  for(auto iter = writers.begin() ; iter != writers.end() ; ++iter) {
    loggerWriters.push_back(iter->second);
  }
  return loggerWriters;
}

void
LoggerFactory::createDefaultLogger(const std::map<std::string, anch::log::Writer*>& writers) {
  _loggersConfig.insert({
      anch::log::DEFAULT,
      anch::log::LoggerConfiguration(anch::log::DEFAULT, anch::log::Level::TRACE, getAllWriters(writers))
    });
}

void
LoggerFactory::initializeLoggersConfiguration(const std::map<std::string, anch::log::Writer*>& writers,
					      const anch::conf::Section* conf) {
  // Check anch::log section existence +
  if(!conf->getSections().contains(anch::log::LOGGER)) {
    createDefaultLogger(writers);
    return;
  }
  // Check anch::log section existence -

  auto loggers = conf->getSections().at(anch::log::LOGGER);
  // Retrieve default writer +
  std::optional<std::string> optDefaultWriters = loggers.getValue<std::string>(anch::log::WRITERS);
  anch::log::Level defaultLevel = toLevel(loggers.getValue<std::string>(anch::log::LEVEL, "WARN"));
  if(optDefaultWriters.has_value()) {
    std::vector<anch::log::Writer*> loggerWriters = parseWriters(optDefaultWriters.value(), writers);
    // Add default writer when not found +
    if(!loggerWriters.empty()) {
      _loggersConfig.insert({anch::log::DEFAULT, anch::log::LoggerConfiguration(anch::log::DEFAULT, defaultLevel, loggerWriters)});
    } else {
      _loggersConfig.insert({anch::log::DEFAULT, anch::log::LoggerConfiguration(anch::log::DEFAULT, defaultLevel, std::move(getAllWriters(writers)))});
    }
    // Add default writer when not found -
  } else {
    std::cerr << "No writers has been found for default logger. Add all writers by default." << std::endl;
    _loggersConfig.insert({anch::log::DEFAULT, anch::log::LoggerConfiguration(anch::log::DEFAULT, defaultLevel, std::move(getAllWriters(writers)))});
  }
  // Retrieve default writer -

  // Parser writers +
  for(auto iter = loggers.getSections().begin() ; iter != loggers.getSections().end() ; ++iter) {
    // Retrieve writer +
    std::string name = iter->first;
    const anch::conf::Section& logger = iter->second;
    std::optional<std::string> opWriters = logger.getValue<std::string>(anch::log::WRITERS);
    std::vector<anch::log::Writer*> loggerWriters;
    if(opWriters.has_value()) {
      loggerWriters = parseWriters(opWriters.value(), writers);
    }
    // Retrieve writer -

    anch::log::Level level = toLevel(logger.getValue<std::string>(anch::log::LEVEL, "WARN"));
    if(loggerWriters.empty()) {
      _loggersConfig.insert({name, anch::log::LoggerConfiguration(name, level, loggerWriters)});
    } else {
      _loggersConfig.insert({name, anch::log::LoggerConfiguration(name, level, std::move(getAllWriters(writers)))});
    }
  }
  // Parser writers -
}

void
LoggerFactory::loadDefaultConfiguration() {
  _loggersConfig.clear();
  std::cout << "No configuration has been found. Everything will be logged in console." << std::endl;
  // Create default writer +
  anch::log::Writer* console = createDefaultWriter(anch::log::WRITER_TERM);
  // Create default writer -
  // Register default logger on default writer +
  createDefaultLogger({
      {anch::log::DEFAULT, console}
    });
  // Register default logger on default writer -
}
// Methods -
