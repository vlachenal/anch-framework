/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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
#include "rest/headers.hpp"

#include <sstream>

#include "stringUtils.hpp"

using anch::rest::Headers;


std::optional<std::vector<std::string>>
Headers::get(const std::string& key) {
  std::string header(key);
  anch::toLower(header);
  auto iter = find(header);
  if(iter == end()) {
    return std::optional<std::vector<std::string>>();
  } else {
    return std::optional<std::vector<std::string>>(iter->second);
  }
}

const std::optional<const std::vector<std::string>>
Headers::get(const std::string& key) const {
  std::string header(key);
  anch::toLower(header);
  auto iter = const_cast<Headers*>(this)->find(header);
  if(iter == cend()) {
    return std::optional<const std::vector<std::string>>();
  } else {
    return std::optional<const std::vector<std::string>>(iter->second);
  }
}

bool
Headers::has(const std::string& key) const {
  std::string header(key);
  anch::toLower(header);
  return contains(header);
}

bool
Headers::add(const std::string& name, const std::vector<std::string>& values) {
  std::string header(name);
  anch::toLower(header);
  return insert({header, values}).second;
}

bool
Headers::add(const std::string& name, const std::string& value) {
  std::string header(name);
  anch::toLower(header);
  return insert({header, { value } }).second;
}

void
Headers::put(const std::string& name, const std::vector<std::string>& values) {
  std::string header(name);
  anch::toLower(header);
  auto iter = find(header);
  if(iter != end()) {
    iter->second = values;
  } else {
    insert({header, values}).first;
  }
}

void
Headers::put(const std::string& name, const std::string& value) {
  std::string header(name);
  anch::toLower(header);
  auto iter = find(header);
  if(iter != end()) {
    iter->second = { value };
  } else {
    insert({header, { value } }).first;
  }
}

std::optional<std::string>
Headers::format(const std::string& name) const {
  std::string lname(name);
  anch::toLower(lname);
  auto header = find(lname);
  if(header == end() || header->second.empty()) {
    return std::optional<std::string>();
  } else {
    std::ostringstream oss;
    for(auto iter = header->second.begin() ; iter != header->second.end() ; ++ iter) {
      oss << ',' << *iter;
    }
    return std::optional<std::string>(oss.str().substr(1));
  }
}
