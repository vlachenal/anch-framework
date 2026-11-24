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
#include "sql/parameters.hpp"

#include <algorithm>
#include <charconv>

#include "date/dateFormatter.hpp"

using anch::sql::Parameter;
using anch::sql::ParameterValue;
using anch::sql::Parameters;

// Values conversion +
ParameterValue
anch::sql::toParameter(int16_t value) {
  return std::to_string(value);
}

ParameterValue
anch::sql::toParameter(uint16_t value) {
  return std::to_string(value);
}

ParameterValue
anch::sql::toParameter(int32_t value) {
  return std::to_string(value);
}

ParameterValue
anch::sql::toParameter(uint32_t value) {
  return std::to_string(value);
}

ParameterValue
anch::sql::toParameter(int64_t value) {
  return std::to_string(value);
}

ParameterValue
anch::sql::toParameter(uint64_t value) {
  return std::to_string(value);
}

ParameterValue
anch::sql::toParameter(double value) {
  char buffer[32];
  auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
  return std::string(buffer, result.ptr);
}

ParameterValue
anch::sql::toParameter(const std::string& value) {
  return value;
}

ParameterValue
anch::sql::toParameter(const char* value) {
  if(value == NULL) {
    return std::nullopt;
  }
  return std::string(value);
}

ParameterValue
anch::sql::toParameter(const anch::sql::Date& value) {
  static const anch::date::DateFormatter formatter("%Y-%m-%d");
  std::string str;
  formatter.format(value, str);
  return str;
}

ParameterValue
anch::sql::toParameter(const anch::sql::Time& value) {
  static const anch::date::DateFormatter formatter("%H:%M:%S.%s");
  std::string str;
  formatter.format(value, str);
  return str;
}

ParameterValue
anch::sql::toParameter(const anch::sql::Timestamp& value) {
  static const anch::date::DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");
  std::string str;
  formatter.format(value, str);
  return str;
}
std::string
anch::sql::arrayLiteral(const anch::sql::ParameterList& elements) {
  std::string literal("{");
  for(std::size_t i = 0 ; i < elements.size() ; ++i) {
    if(i > 0) {
      literal += ',';
    }
    if(!elements[i].has_value()) {
      literal += "NULL";
      continue;
    }
    literal += '"';
    for(char c : *elements[i]) {
      if(c == '"' || c == '\\') {
	literal += '\\';
      }
      literal += c;
    }
    literal += '"';
  }
  literal += '}';
  return literal;
}
// Values conversion -

// Parameters +
Parameters::Parameters(std::initializer_list<Parameter> parameters): _parameters() {
  _parameters.reserve(parameters.size());
  for(const Parameter& parameter : parameters) {
    put(parameter.name, anch::sql::BoundValue{parameter.value, parameter.list});
  }
}

Parameters&
Parameters::addNull(std::string_view name) {
  put(name, anch::sql::BoundValue{std::nullopt, std::nullopt});
  return *this;
}

Parameters&
Parameters::add(Parameter parameter) {
  put(parameter.name, anch::sql::BoundValue{std::move(parameter.value), std::move(parameter.list)});
  return *this;
}

const std::vector<Parameter>&
Parameters::values() const noexcept {
  return _parameters;
}

void
Parameters::put(std::string_view name, anch::sql::BoundValue value) {
  auto iter = std::find_if(_parameters.begin(), _parameters.end(), [name](const Parameter& parameter) { return parameter.name == name; });
  if(iter == _parameters.end()) {
    _parameters.push_back(Parameter(std::string(name), std::nullopt));
    iter = _parameters.end() - 1;
  }
  iter->value = std::move(value.value);
  iter->list = std::move(value.list);
}
// Parameters -
