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
#include "rest/headers.hpp"

#include <algorithm>

using anch::rest::CaseInsensitiveLess;
using anch::rest::Headers;

namespace {

  constexpr char
  lower(char c) noexcept {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
  }

  const std::vector<std::string> NO_VALUE;

}

bool
CaseInsensitiveLess::operator()(std::string_view left, std::string_view right) const noexcept {
  return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end(),
				      [](char l, char r) { return lower(l) < lower(r); });
}

void
Headers::add(std::string_view name, std::string_view value) {
  auto iter = _values.find(name);
  if(iter == _values.end()) {
    iter = _values.emplace(std::string(name), std::vector<std::string>()).first;
  }
  iter->second.emplace_back(value);
}

void
Headers::set(std::string_view name, std::string_view value) {
  auto iter = _values.find(name);
  if(iter == _values.end()) {
    _values.emplace(std::string(name), std::vector<std::string>{std::string(value)});
  } else {
    iter->second.assign(1, std::string(value));
  }
}

bool
Headers::remove(std::string_view name) {
  auto iter = _values.find(name);
  if(iter == _values.end()) {
    return false;
  }
  _values.erase(iter);
  return true;
}

bool
Headers::has(std::string_view name) const {
  return _values.find(name) != _values.end();
}

std::optional<std::string_view>
Headers::get(std::string_view name) const {
  auto iter = _values.find(name);
  if(iter == _values.end() || iter->second.empty()) {
    return std::nullopt;
  }
  return std::string_view(iter->second.front());
}

const std::vector<std::string>&
Headers::getAll(std::string_view name) const {
  auto iter = _values.find(name);
  return iter == _values.end() ? NO_VALUE : iter->second;
}

std::optional<std::string>
Headers::format(std::string_view name) const {
  auto iter = _values.find(name);
  if(iter == _values.end()) {
    return std::nullopt;
  }
  std::string res;
  for(const std::string& value : iter->second) {
    if(!res.empty()) {
      res += ", ";
    }
    res += value;
  }
  return res;
}

std::size_t
Headers::size() const noexcept {
  return _values.size();
}

bool
Headers::empty() const noexcept {
  return _values.empty();
}

Headers::Map::const_iterator
Headers::begin() const noexcept {
  return _values.begin();
}

Headers::Map::const_iterator
Headers::end() const noexcept {
  return _values.end();
}
