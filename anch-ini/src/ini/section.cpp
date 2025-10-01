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
#include "ini/section.hpp"

using anch::ini::Section;


// Static methods +
std::optional<std::string>
Section::getValue(const std::string& path, const Section& section) {
  auto pos = path.find('.');
  // Subsection +
  if(pos != path.npos) {
    auto subSection = section._sections.find(path.substr(0, pos));
    if(subSection == section._sections.end()) {
      return std::optional<std::string>();
    }
    return Section::getValue(path.substr(pos + 1), subSection->second);
  }
  // Subsection -
  // Value +
  auto value = section._values.find(path);
  if(value == section._values.end()) {
    return std::optional<std::string>();
  }
  return std::optional<std::string>(value->second);
  // Value -
}
// Static methods -

// Constructors +
Section::Section(): _sections(), _values() {
  // Nothing to do
}

Section::Section(const Section& section):
  _sections(section._sections),
  _values(section._values) {
  // Nothing to do
}

Section::Section(Section&& section):
  _sections(std::move(section._sections)),
  _values(std::move(section._values)) {
  // Nothing to do
}
// Constructors -

// Destructor +
Section::~Section() {
  // Nothing to do
}
// Destructor +

// Methods +
Section&
Section::section(const std::string& key) {
  auto iter = _sections.find(key);
  if(iter == _sections.end()) {
    _sections.insert(std::pair{key, Section()});
    iter = _sections.find(key);
  }
  return iter->second;
}

Section&
Section::putValue(const std::string& key, const std::string& value) {
  _values[key] = value;
  return *this;
}
// Methods -

// Operators +
Section&
Section::operator = (const Section& other) {
  _sections = other._sections;
  _values = other._values;
  return *this;
}
// Operators +
