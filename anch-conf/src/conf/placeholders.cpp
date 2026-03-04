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
#include "conf/placeholders.hpp"

#include <regex>

#include "conf/resolvers.hpp"

#include "events/observer.hpp"

#include <iostream> // \todo remove

using anch::conf::Placeholders;
using anch::conf::Section;
using anch::conf::Resolvers;
using anch::conf::ConfError;


const std::regex PH_PATTERN("\\$\\{( )*([^}]+)( )*\\}");
const std::regex RSV_PATTERN("(([^=|]+)( )*=)?(( )*([^|]+))");


// Constructors +
Placeholders::Placeholders(anch::conf::Section& section) noexcept:
  anch::events::Observable<anch::conf::ResConfEvt>(),
  _section(section),
  _values() {
  // Nothing to do
}
// Constructors -

// Destructor +
Placeholders::~Placeholders() noexcept {
  // Nothing to do
}
// Destructor -

// Methods +
void
Placeholders::resolve() {
  if(_values.empty()) {
    std::cout << "No placeholder has been found" << std::endl;
    return;
  }
  std::cout << std::endl << "Resolve placeholders" << std::endl;
  for(auto iter = _values.begin() ; iter != _values.end() ; ++iter) {
    std::cout << "Try to resolve placeholder " << iter->first << std::endl;
  }
}

auto REND = std::sregex_iterator();

void
Placeholders::parsePlaceholder(anch::conf::ConfPlaceholders& cph, const std::string& placeholder) {
  std::cout << "Parse placeholder " << placeholder << " for key " << cph.path << std::endl;
  auto sub = std::sregex_iterator(placeholder.begin(), placeholder.end(), RSV_PATTERN);
  if(sub == REND) {
    return;
  }
  std::vector<anch::conf::SubPlaceholder> subs;
  for(std::size_t i = 0 ; sub != REND ; ++sub, ++i) {
    std::cout << "Found: resolver = '" << sub->str(2) << "' ; value = '" << sub->str(6) << '\'' << std::endl;
    anch::conf::SubPlaceholder item;
    item.res = sub->str(2);
    if(item.res == "") { // default value
      item.val = sub->str(6);
    } else if(!Resolvers::getInstance().hasResolver(item.res)) { // check if resolver exists
      std::ostringstream oss;
      oss << "Resolver " << item.res << " has not been found";
      throw ConfError(oss.str(), ConfError::ErrorCode::RES_NOT_FOUND);
    } else {
      item.arg = sub->str(6);
    }
    if(item.res == Resolvers::ENV) { // resolv environment variable
      item.val = std::invoke(Resolvers::getInstance().getResolver(Resolvers::ENV), item.arg, "");
      std::cout << "Found env " << item.arg << ": " << item.val.value_or("not found") << std::endl;
    } else if(item.res == Resolvers::CONF) {
      // TODO register placeholder as event's consumer
    }
    subs.push_back(item);
  }
  cph.placeholders[placeholder] = subs;
}

void
Placeholders::collect(Section& section, const std::string& path) {
  std::cout << std::endl << "Parse placeholder in section: " << (path.empty() ? "." : path) << std::endl;
  std::string basePath = path.empty() ? path : path + '.';
  for(auto iter = section.getValues().begin() ; iter != section.getValues().end() ; ++iter) {
    std::string valuePath = basePath + iter->first;
    std::cout << std::endl << "Parse value " << iter->second << " for key " << valuePath << std::endl;
    auto sub = std::sregex_iterator(iter->second.begin(), iter->second.end(), PH_PATTERN);
    if(sub == REND) {
      continue;
    }
    anch::conf::ConfPlaceholders cph;
    cph.path = valuePath;
    for(std::size_t i = 0 ; sub != REND ; ++sub, ++i) { // sub->str(0) matches the complete PH_PATTERN with ${}
      parsePlaceholder(cph, sub->str(2));
    }
    _values[valuePath] = cph;
  }
  for(auto iter = section.getSections().begin() ; iter != section.getSections().end() ; ++iter) {
    collect(section.section(iter->first), basePath + iter->first);
  }
}
// Methods -
