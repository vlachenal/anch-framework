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
#include "resource/resource.hpp"

#include "resource/file/configurationFileParser.hpp"

using std::mutex;
using std::string;
using std::pair;
using std::map;

using anch::resource::Resource;
using anch::resource::Section;
using anch::resource::file::ConfigurationFileParser;

namespace anch {
  namespace resource {
    class FakeSection: public Section {
      virtual const std::string& getParameter(const std::string&) const override {
	return Section::DEFAULT_VALUE;
      }
#ifdef ANCH_STD_OTP
      virtual std::optional<std::string> parameter(const std::string&) const override {
	return std::optional<std::string>();
      }
#endif
    };
  }
}

using anch::resource::FakeSection;

// Static initialization +
map<string,Resource> Resource::RESOURCES;
mutex Resource::MUTEX;

FakeSection FAKE_SEC;
// Static initialization -


Resource::Resource(): _resources() {
  // Nothing to do
}

Resource::~Resource() {
  // Nothing to do
}

const Resource&
Resource::getResource(const string& filePath) {
  std::lock_guard<mutex> lock(MUTEX);
  auto iter = RESOURCES.find(filePath);
  if(iter == RESOURCES.cend()) {
    Resource res;
    ConfigurationFileParser configParser(filePath);
    configParser.getConfiguration(res._resources);
    iter = RESOURCES.insert(pair<string, Resource>(filePath,res)).first;
  }
  return iter->second;
}

bool
Resource::getParameter(string& value,
		       const string& param,
		       const string& section) const {
  bool found = false;
  auto iterSection = _resources.find(section);
  if(iterSection != _resources.end()) {
    value = iterSection->second.getParameter(param);
    if(value != Section::DEFAULT_VALUE) {
      found = true;
    }
  }
  return found;
}

#ifdef ANCH_STD_OTP
std::optional<Section>
Resource::section(const std::string& section) const {
  std::optional<Section> value;
  auto iter = _resources.find(section);
  if(iter != _resources.end()) {
    value = iter->second;
  }
  return value;
}

std::optional<std::string>
Resource::parameter(const std::string& param) const {
  return section("").value_or(FAKE_SEC).parameter(param);
}
#endif
