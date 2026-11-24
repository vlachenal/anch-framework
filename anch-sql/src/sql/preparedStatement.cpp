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
#include "sql/preparedStatement.hpp"

#include <algorithm>

#include <string>

#include "sql/connection.hpp"

using anch::sql::PreparedStatement;
using anch::sql::ResultSet;
using anch::sql::SqlException;


PreparedStatement::PreparedStatement() noexcept:
  _query(), _names(), _values(), _lists(), _bound(), _bindings(), _named(), _style(anch::sql::ParameterStyle::POSITIONAL), _prepared(),
  _shape(), _connection(NULL), _openResults(0), _invalid(false) {
  // Nothing to do
}

PreparedStatement::~PreparedStatement() noexcept {
  // Nothing to do
}

ResultSet*
PreparedStatement::executeQuery() {
  checkBindings();
  ResultSet* res = NULL;
  if(_connection == NULL) {
    reshape();
    res = doExecuteQuery();
  } else {
    res = _connection->guard([this]() {
      reshape();
      return doExecuteQuery();
    });
    res->_connection = _connection;
  }
  res->_statement = this; // statement is kept in connection cache while the result set is alive
  ++_openResults;
  return res;
}

uint64_t
PreparedStatement::executeUpdate() {
  checkBindings();
  if(_connection == NULL) {
    reshape();
    return doExecuteUpdate();
  }
  return _connection->guard([this]() {
    reshape();
    return doExecuteUpdate();
  });
}

std::string
PreparedStatement::parse(const std::string& query, const anch::sql::SqlSyntax& syntax, anch::sql::ParameterStyle style) {
  _named.emplace(query, syntax);
  anch::sql::NamedQuery::Rendered rendered = _named->render(style);
  _query = query;
  _names = _named->names();
  _style = style;
  _bindings = std::move(rendered.bindings);
  _prepared = rendered.query;
  _shape.assign(_names.size(), anch::sql::NamedQuery::SCALAR);
  clearParameters();
  return std::move(rendered.query);
}

void
PreparedStatement::reshape() {
  std::vector<std::size_t> shape(_names.size(), anch::sql::NamedQuery::SCALAR);
  bool lists = false;
  for(std::size_t slot = 0 ; slot < _names.size() ; ++slot) {
    if(_lists[slot].has_value()) {
      // PostgreSQL arrays do not depend on their size
      shape[slot] = (_style == anch::sql::ParameterStyle::DOLLAR) ? 0 : _lists[slot]->size();
      lists = true;
    }
  }
  if(shape == _shape) {
    return;
  }
  anch::sql::NamedQuery::Rendered rendered = lists ? _named->render(_style, shape) : _named->render(_style);
  std::vector<anch::sql::NamedQuery::Binding> previous = std::move(_bindings);
  _bindings = std::move(rendered.bindings); // drivers check the parameters count of the new query
  if(rendered.query != _prepared) {
    try {
      doPrepare(rendered.query);
    } catch(...) {
      _bindings = std::move(previous); // the previous query is still prepared
      throw;
    }
    _prepared = std::move(rendered.query);
  }
  _shape = std::move(shape);
}

std::size_t
PreparedStatement::getParameterCount() const noexcept {
  return _names.size();
}

const std::vector<std::string>&
PreparedStatement::getParameterNames() const noexcept {
  return _names;
}

const std::string&
PreparedStatement::getQuery() const noexcept {
  return _query;
}

std::size_t
PreparedStatement::getBindingCount() const noexcept {
  return _bindings.size();
}

const anch::sql::ParameterValue&
PreparedStatement::getBindingValue(std::size_t idx) const {
  const anch::sql::NamedQuery::Binding& binding = _bindings[idx];
  if(binding.element == anch::sql::NamedQuery::SCALAR) {
    return _values[binding.slot]; // value, or PostgreSQL array literal
  }
  return (*_lists[binding.slot])[binding.element];
}

void
PreparedStatement::deallocate() noexcept {
  // Nothing to do by default
}

void
PreparedStatement::invalidate() noexcept {
  _invalid = true;
}

void
PreparedStatement::clearParameters() noexcept {
  _values.assign(_names.size(), std::nullopt);
  _lists.assign(_names.size(), std::nullopt);
  _bound.assign(_names.size(), false);
}

void
PreparedStatement::bind(std::string_view name, anch::sql::BoundValue value) {
  auto iter = std::find(_names.begin(), _names.end(), name);
  if(iter == _names.end()) {
    std::string names;
    for(const std::string& known : _names) {
      names += (names.empty() ? "" : ", ") + known;
    }
    throw SqlException("no parameter '" + std::string(name) + "' in query (parameters: " + (names.empty() ? std::string("none") : names)
		       + "): " + _query, true);
  }
  const std::size_t slot = static_cast<std::size_t>(iter - _names.begin());
  _values[slot] = std::move(value.value);
  _lists[slot] = std::move(value.list);
  _bound[slot] = true;
}

void
PreparedStatement::checkBindings() const {
  std::string missing;
  std::size_t count = 0;
  for(std::size_t i = 0 ; i < _names.size() ; ++i) {
    if(!_bound[i]) {
      missing += (count++ == 0 ? "'" : ", '") + _names[i] + "'";
    }
  }
  if(count > 0) {
    throw SqlException((count == 1 ? "parameter " : "parameters ") + missing + (count == 1 ? " has" : " have")
		       + " not been bound: " + _query, true);
  }
}

void
PreparedStatement::setNull(std::string_view name) {
  bind(name, anch::sql::BoundValue{std::nullopt, std::nullopt});
}

void
PreparedStatement::set(const anch::sql::Parameters& parameters) {
  clearParameters();
  for(const anch::sql::Parameter& parameter : parameters.values()) {
    bind(parameter.name, anch::sql::BoundValue{parameter.value, parameter.list});
  }
}
