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
      loadDefaultConfiguration();
    } else {
      std::map<std::string,anch::log::Writer*> writers;
      initializeWriters(writers, conf);
      initializeLoggersConfiguration(writers, conf);
    }

  } catch(...) {
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
    /*self._loggers[loggerName] = new anch::log::Logger(loggerName,
      iter->second.getLevel(),
      iter->second.getWriters());*/
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
    return;
  }
  const anch::conf::Section& wSec = wIter->second;
  for(auto iter = wSec.getSections().begin() ; iter != wSec.getSections().end() ; ++iter) {
    std::string name = iter->first;
    const anch::conf::Section& writer = iter->second;
    std::optional<std::string> type = writer.getValue<std::string>(anch::log::MODULE);
    if(!type.has_value()) {
      continue;
    }
    if(!WriterRegistry::getInstance().contains(type.value())) {
      continue;
    }
    writers[name] = WriterRegistry::getInstance().create(name, threadSafe, lowPriority, writer);
  }
  // Register writers -
}

anch::log::Level
toLevel(std::string& levelLbl) {
  std::transform(levelLbl.begin(),
		 levelLbl.end(),
		 levelLbl.begin(),
		 ::toupper);
  anch::log::Level level = anch::log::Level::FATAL;
  auto iterLvl = anch::log::LABEL_LEVEL.find(levelLbl);
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

void
LoggerFactory::initializeLoggersConfiguration(const std::map<std::string, anch::log::Writer*>& writers,
					      const anch::conf::Section* conf) {
  if(!conf->getSections().contains(anch::log::LOGGER)) {
    loadDefaultConfiguration(); // \todo ???
    return;
  }

  auto loggers = conf->getSections().at(anch::log::LOGGER);
  std::optional<std::string> defaultWriters = loggers.getValue<std::string>(anch::log::WRITERS);
  if(defaultWriters.has_value()) {
    std::vector<anch::log::Writer*> loggerWriters = parseWriters(defaultWriters.value(), writers);
    if(loggerWriters.empty()) {
      std::string level = loggers.getValue<std::string>(anch::log::LEVEL, "WARN");
      _loggersConfig.insert({anch::log::DEFAULT, anch::log::LoggerConfiguration(anch::log::DEFAULT, toLevel(level), loggerWriters)});
    }
  }

  for(auto iter = loggers.getSections().begin() ; iter != loggers.getSections().end() ; ++iter) {
    std::string name = iter->first;
    const anch::conf::Section& logger = iter->second;
    std::optional<std::string> opWriters = logger.getValue<std::string>(anch::log::WRITERS);
    if(!opWriters.has_value()) {
      continue;
    }

    std::vector<anch::log::Writer*> loggerWriters = parseWriters(opWriters.value(), writers);
    // Retrieve writers -

    // Retrieve level +
    if(loggerWriters.empty()) {
      std::cerr << "No writers has been found for " << name << ": "
		<< opWriters.value() << std::endl; // \todo remove
      continue;
    }

    std::string levelLbl = logger.getValue<std::string>(anch::log::LEVEL, "FATAL");
    // Retrieve level -
    _loggersConfig.insert({name, anch::log::LoggerConfiguration(name, toLevel(levelLbl), loggerWriters)});
  }
}

void
LoggerFactory::loadDefaultConfiguration() {
  _loggersConfig.clear();
  std::cout << "No configuration has been found. Everything will be logged in console." << std::endl;
  // Create writer +
  // Create writer section +
  anch::conf::Section defSec;
  anch::conf::Section& writerSec = defSec.section(anch::log::LOGGER).section(anch::log::WRITER_TERM);
  writerSec.putValue(anch::log::MODULE, anch::log::WRITER_TERM)
    .putValue("pattern", "$d{%Y-%m-%d %H:%M:%S} - $m");
  // Create writer section -
  auto console = WriterRegistry::getInstance().create(anch::log::WRITER_TERM, true, false, writerSec);
  // Create writer -
  // Register default logger on default writer +
  std::vector<anch::log::Writer*> loggerWriters;
  loggerWriters.push_back(console);
  loggerWriters.shrink_to_fit();
  _loggersConfig.insert({
      anch::log::DEFAULT,
      anch::log::LoggerConfiguration(anch::log::DEFAULT, anch::log::Level::TRACE, loggerWriters)
    });
  // Register default logger on default writer -
}
// Methods -
