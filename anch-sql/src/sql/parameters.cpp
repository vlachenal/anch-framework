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
// Values conversion -

// Parameters +
Parameters::Parameters(std::initializer_list<Parameter> parameters): _parameters() {
  _parameters.reserve(parameters.size());
  for(const Parameter& parameter : parameters) {
    put(parameter.name, parameter.value);
  }
}

Parameters&
Parameters::addNull(std::string_view name) {
  put(name, std::nullopt);
  return *this;
}

const std::vector<Parameter>&
Parameters::values() const noexcept {
  return _parameters;
}

void
Parameters::put(std::string_view name, ParameterValue value) {
  auto iter = std::find_if(_parameters.begin(), _parameters.end(), [name](const Parameter& parameter) { return parameter.name == name; });
  if(iter != _parameters.end()) {
    iter->value = std::move(value);
  } else {
    _parameters.push_back(Parameter(std::string(name), std::nullopt));
    _parameters.back().value = std::move(value);
  }
}
// Parameters -
