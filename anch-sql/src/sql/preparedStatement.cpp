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
#include "sql/preparedStatement.hpp"

#include <string>

#include "date/dateFormatter.hpp"
#include "sql/connection.hpp"

using anch::sql::PreparedStatement;
using anch::sql::ResultSet;
using anch::sql::SqlException;


PreparedStatement::PreparedStatement() noexcept: _values(), _bound(), _nbPlaceholders(0), _connection(NULL) {
  // Nothing to do
}

PreparedStatement::~PreparedStatement() noexcept {
  // Nothing to do
}

ResultSet*
PreparedStatement::executeQuery() {
  checkBindings();
  if(_connection == NULL) {
    return doExecuteQuery();
  }
  return _connection->guard([this]() {
    ResultSet* res = doExecuteQuery();
    res->_connection = _connection;
    return res;
  });
}

uint64_t
PreparedStatement::executeUpdate() {
  checkBindings();
  if(_connection == NULL) {
    return doExecuteUpdate();
  }
  return _connection->guard([this]() { return doExecuteUpdate(); });
}

void
PreparedStatement::setParameterCount(std::size_t count) {
  _nbPlaceholders = count;
  _values.assign(count, std::nullopt);
  _bound.assign(count, false);
}

std::size_t
PreparedStatement::getParameterCount() const noexcept {
  return _nbPlaceholders;
}

void
PreparedStatement::clearParameters() noexcept {
  _values.assign(_nbPlaceholders, std::nullopt);
  _bound.assign(_nbPlaceholders, false);
}

void
PreparedStatement::bind(std::size_t idx, std::optional<std::string> value) {
  if(idx == 0 || idx > _nbPlaceholders) {
    throw SqlException("Parameter index " + std::to_string(idx) + " is out of range [1.." + std::to_string(_nbPlaceholders) + "]", true);
  }
  _values[idx - 1] = std::move(value);
  _bound[idx - 1] = true;
}

void
PreparedStatement::checkBindings() const {
  for(std::size_t i = 0 ; i < _nbPlaceholders ; ++i) {
    if(!_bound[i]) {
      throw SqlException("Parameter " + std::to_string(i + 1) + " has not been bound", true);
    }
  }
}

void
PreparedStatement::set(std::size_t idx, int16_t value) {
  bind(idx, std::to_string(value));
}

void
PreparedStatement::set(std::size_t idx, uint16_t value) {
  bind(idx, std::to_string(value));
}

void
PreparedStatement::set(std::size_t idx, int32_t value) {
  bind(idx, std::to_string(value));
}

void
PreparedStatement::set(std::size_t idx, uint32_t value) {
  bind(idx, std::to_string(value));
}

void
PreparedStatement::set(std::size_t idx, int64_t value) {
  bind(idx, std::to_string(value));
}

void
PreparedStatement::set(std::size_t idx, uint64_t value) {
  bind(idx, std::to_string(value));
}

void
PreparedStatement::set(std::size_t idx, const std::string& value) {
  bind(idx, value);
}

void
PreparedStatement::set(std::size_t idx, const char* const value) {
  if(value == NULL) {
    bind(idx, std::nullopt);
  } else {
    bind(idx, std::string(value));
  }
}

void
PreparedStatement::setNull(std::size_t idx) {
  bind(idx, std::nullopt);
}

void
PreparedStatement::set(std::size_t idx, const anch::sql::Date& value) {
  static const anch::date::DateFormatter formatter("%Y-%m-%d");
  std::string str;
  formatter.format(value, str);
  bind(idx, std::move(str));
}

void
PreparedStatement::set(std::size_t idx, const anch::sql::Time& value) {
  static const anch::date::DateFormatter formatter("%H:%M:%S.%s");
  std::string str;
  formatter.format(value, str);
  bind(idx, std::move(str));
}

void
PreparedStatement::set(std::size_t idx, const anch::sql::Timestamp& value) {
  static const anch::date::DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");
  std::string str;
  formatter.format(value, str);
  bind(idx, std::move(str));
}
