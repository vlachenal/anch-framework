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
#include "sql/builder/sqlQuery.hpp"

#include <algorithm>
#include <vector>

#include "sql/placeholders.hpp"

#include "parametersMerge.hpp"

using anch::sql::Parameter;
using anch::sql::Parameters;
using anch::sql::SQLQuery;

SQLQuery::SQLQuery(const SQLQuery& other): _query(other._query), _parameters(other._parameters) {
  // Nothing to do
}

SQLQuery::SQLQuery(const std::string& query, const Parameters& parameters): _query(query), _parameters(parameters) {
  // Nothing to do
}

namespace {

  bool
  used(const Parameters& parameters, std::string_view name) {
    const std::vector<Parameter>& values = parameters.values();
    return std::any_of(values.begin(), values.end(), [name](const Parameter& parameter) { return parameter.name == name; });
  }

}

std::string
anch::sql::detail::parameterName(const Parameters& parameters, const std::string& column, std::string_view suffix) {
  std::string base;
  base.reserve(column.size() + suffix.size());
  for(char c : column) {
    const bool valid = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
    base += valid ? c : '_';
  }
  if(base.empty() || (base[0] >= '0' && base[0] <= '9')) {
    base.insert(0, "p");
  }
  base += suffix;
  std::string name = base;
  for(std::size_t i = 2 ; used(parameters, name) ; ++i) {
    name = base + '_' + std::to_string(i);
  }
  return name;
}

std::string
anch::sql::detail::mergeParameters(Parameters& target, const std::string& part, const Parameters& parameters) {
  if(parameters.values().empty()) {
    return part;
  }
  anch::sql::NamedQuery named(part, anch::sql::SqlSyntax::STANDARD);
  std::vector<std::string> names = named.names();
  bool renamed = false;
  for(std::string& name : names) {
    auto iter = std::find_if(parameters.values().begin(), parameters.values().end(),
			     [&name](const Parameter& parameter) { return parameter.name == name; });
    if(iter == parameters.values().end()) {
      continue; // not a parameter of the part
    }
    if(used(target, name)) {
      name = parameterName(target, name);
      renamed = true;
    }
    anch::sql::Parameter parameter = *iter;
    parameter.name = name;
    target.add(std::move(parameter));
  }
  return renamed ? named.rename(names) : part;
}
