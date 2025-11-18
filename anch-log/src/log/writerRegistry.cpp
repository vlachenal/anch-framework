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
#include "log/writerRegistry.hpp"

#include "log/termWriter.hpp"
#include "log/fileWriter.hpp"
#include "log/threadSafeWriter.hpp"
#include "log/lowPriorityWriter.hpp"
#include "log/constants.hpp"

using anch::log::WriterRegistry;

WriterRegistry::WriterRegistry(): _registry(), _writers() {
  // Console writer +
  _registry["console"] = [](bool, bool, const anch::conf::Section& section) {
    return new anch::log::ConsoleWriter(section);
  };
  // Console writer -
  // File writer +
  _registry["file"] = [](bool threadSafe, bool lowPriority, const anch::conf::Section& section) {
    anch::log::Writer* writer = NULL;
    bool low = section.getValue<bool>(anch::log::LOW_PRIORITY, false);
    if(lowPriority || low) {
      writer = new anch::log::LowPriorityWriter(section);
      return writer;
    }
    bool safe = section.getValue<bool>(anch::log::THREAD_SAFE, true);
    if(threadSafe || safe) {
      writer = new anch::log::ThreadSafeWriter(section);
      return writer;
    }
    writer = new anch::log::FileWriter(section);
    return writer;
  };
  // File writer -
}

WriterRegistry::~WriterRegistry() {
  for(auto iter = _writers.begin() ; iter != _writers.end() ; ++iter) {
    delete iter->second;
  }
  _writers.clear();
}

void
WriterRegistry::registerWriter(const std::string& name, const anch::log::CreateWriter& writerBuilder) {
  if(_registry.contains(name)) {
    std::ostringstream oss;
    oss << "Writer module " << name << " has already been registered";
    throw std::invalid_argument(oss.str());
  }
  _registry[name] = writerBuilder;
}

anch::log::Writer*
WriterRegistry::create(const std::string& name,
		       bool threadSafe,
		       bool lowPriority,
		       const anch::conf::Section& section) {
  if(_writers.contains(name)) {
    return _writers[name];
  }
  auto mod = section.getValue<std::string>(anch::log::MODULE);
  if(!mod.has_value()) {
    std::ostringstream oss;
    oss << "Writer module has not been defined in writer " << name << " section";
    throw std::invalid_argument(oss.str());
  }
  if(!_registry.contains(mod.value())) {
    std::ostringstream oss;
    oss << "Writer module " << mod.value() << " has not been registered";
    throw std::invalid_argument(oss.str());
  }
  _writers[name] = _registry[mod.value()](threadSafe, lowPriority, section);
  return _writers[name];
}
