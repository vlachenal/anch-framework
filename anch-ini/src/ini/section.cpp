/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "ini/section.hpp"

#include <algorithm>
#include <cctype>
#include <string_view>

#include "ini/parserError.hpp"
#include "convert.hpp"

using anch::ini::Section;
using anch::ini::ParserError;


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

template<>
std::optional<std::string>
Section::getValue(const std::string& path) const {
  return Section::getValue(path, *this);
}

template<typename T>
void
toNumber(const std::string val, T& num) {
  std::expected<T, std::errc> res = anch::tryConvert<T>(val);
  if(!res) {
    throw ParserError(val + (res.error() == std::errc::result_out_of_range ? " is out of range" : " is invalid number"),
		      ParserError::ErrorCode::BAD_CAST);
  }
  num = *res;
}

template<>
std::optional<int16_t>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<int16_t> val;
  if(!str.has_value()) {
    return val;
  }
  int16_t num = 0;
  toNumber(str.value(), num);
  val = num;
  return val;
}

template<>
std::optional<int32_t>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<int32_t> val;
  if(!str.has_value()) {
    return val;
  }
  int32_t num = 0;
  toNumber(str.value(), num);
  val = num;
  return val;
}

template<>
std::optional<int64_t>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<int64_t> val;
  if(!str.has_value()) {
    return val;
  }
  int64_t num = 0;
  toNumber(str.value(), num);
  val = num;
  return val;
}

template<>
std::optional<uint16_t>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<uint16_t> val;
  if(!str.has_value()) {
    return val;
  }
  uint16_t num = 0;
  toNumber(str.value(), num);
  val = num;
  return val;
}

template<>
std::optional<uint32_t>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<uint32_t> val;
  if(!str.has_value()) {
    return val;
  }
  uint32_t num = 0;
  toNumber(str.value(), num);
  val = num;
  return val;
}

template<>
std::optional<uint64_t>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<uint64_t> val;
  if(!str.has_value()) {
    return val;
  }
  uint64_t num = 0;
  toNumber(str.value(), num);
  val = num;
  return val;
}

template<>
std::optional<float>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<float> val;
  if(!str.has_value()) {
    return val;
  }
  float num = 0;
  toNumber(str.value(), num);
  val = num;
  return val;
}

template<>
std::optional<double>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<double> val;
  if(!str.has_value()) {
    return val;
  }
  double num = 0;
  toNumber(str.value(), num);
  val = num;
  return val;
}

template<>
std::optional<bool>
Section::getValue(const std::string& path) const {
  auto str = Section::getValue(path, *this);
  std::optional<bool> val;
  if(!str.has_value()) {
    return val;
  }
  // Case is ignored ; any other value is an error, as invalid numbers
  const std::string& value = str.value();
  auto equals = [&value](std::string_view expected) {
    return std::ranges::equal(value, expected, [](char a, char b) {
      return std::tolower(static_cast<unsigned char>(a)) == b;
    });
  };
  if(equals("true")) {
    val = true;
  } else if(equals("false")) {
    val = false;
  } else {
    throw ParserError(value + " is not a boolean (true or false)", ParserError::ErrorCode::BAD_CAST);
  }
  return val;
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

static void
addIndent(uint32_t indent, std::ostringstream& oss) {
  for(uint32_t i = 0 ; i < indent ; ++i) {
    oss << ' ';
  }
}

static void
printSection(const anch::ini::Section& section,
	     std::ostringstream& oss,
	     const std::string& name = "root",
	     uint32_t indent = 0) {
  addIndent(indent, oss);
  oss << '[' << name << ']' << std::endl;
  for(auto iter = section.getValues().begin() ; iter != section.getValues().end() ; ++iter) {
    addIndent(indent + 1, oss);
    oss << iter->first << '=' << iter->second << std::endl;
  }
  for(auto iter = section.getSections().begin() ; iter != section.getSections().end() ; ++iter) {
    printSection(iter->second, oss, iter->first, indent + 2);
  }
}

std::string
Section::debug() const {
  std::ostringstream oss;
  printSection(*this, oss);
  return oss.str();
}
