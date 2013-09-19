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
using anch::resource::file::ConfigurationFileParser;

// Static initialization +
map<string,Resource> Resource::RESOURCES;
mutex Resource::MUTEX;
// Static initialization -


/*!
 * \ref Resource private constructor
 */
Resource::Resource(): _resources() {
  // Nothing to do
}

/*!
 * \ref Resource destructor
 */
Resource::~Resource() {
  // Nothing to do
}

/*!
 * Get Resource unique instance
 *
 * \return The \ref Resource unique instance
 */
const Resource&
Resource::getResource(const string& filePath) {
  MUTEX.lock();
  auto iter = RESOURCES.find(filePath);
  if(iter == RESOURCES.cend()) {
    Resource res;
    ConfigurationFileParser configParser(filePath);
    configParser.getConfiguration(res._resources);
    iter = RESOURCES.insert(pair<string, Resource>(filePath,res)).first;
  }
  MUTEX.unlock();
  return iter->second;
}

/*!
 * Get parameter value from its name and section
 *
 * \param value The value to set
 * \param param The parameter to find
 * \param section The parameter section (optional)
 *
 * \return \c true if value has been found, \c false otherwise.
 */
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
