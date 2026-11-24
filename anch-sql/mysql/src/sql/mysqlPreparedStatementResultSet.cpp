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
#ifdef ANCH_SQL_MYSQL

#include "errmsg.h"

#include "sql/mysqlPreparedStatementResultSet.hpp"
#include "sql/mysqlConnection.hpp"

#include <algorithm>
#include <cstring>

#include <string>
#include <cstring>

#include <iostream>

using anch::sql::MySQLPreparedStatementResultSet;
using anch::sql::ResultSet;


namespace {

  /*! Maximum size of a column buffer (longer values are read again with their real size) */
  constexpr unsigned long MAX_BUFFER_SIZE = 4096;

}

MySQLPreparedStatementResultSet::MySQLPreparedStatementResultSet(MYSQL_STMT* stmt):
  ResultSet(),
  _stmt(stmt),
  _buffers(),
  _values(),
  _lengths(),
  _nulls(),
  _errors(),
  _binds() {
  MYSQL_RES* result = mysql_stmt_result_metadata(_stmt);
  if(result == NULL) {
    if(mysql_stmt_errno(_stmt) != 0) {
      throw anch::sql::mysql::error("Fail to get MySQL statement result metadata", mysql_stmt_errno(_stmt), mysql_stmt_error(_stmt));
    }
    throw SqlException("MySQL statement does not return a result set", true);
  }
  unsigned int nbFields = mysql_num_fields(result);
  _buffers.resize(nbFields);
  _values.resize(nbFields);
  _lengths.assign(nbFields, 0);
  _nulls.assign(nbFields, 0);
  _errors.assign(nbFields, 0);
  _binds.resize(nbFields);
  if(!_binds.empty()) { // memset of a null pointer (no column) is undefined
    std::memset(_binds.data(), 0, _binds.size() * sizeof(MYSQL_BIND));
  }
  for(unsigned int i = 0 ; i < nbFields ; ++i) {
    MYSQL_FIELD* field = mysql_fetch_field(result);
    _fields[field->name] = i;
    _buffers[i].resize(std::min(static_cast<unsigned long>(field->length) + 1, MAX_BUFFER_SIZE));
    MYSQL_BIND& bind = _binds[i];
    bind.buffer_type = MYSQL_TYPE_STRING;
    bind.buffer = _buffers[i].data();
    bind.buffer_length = _buffers[i].size();
    bind.is_null = &_nulls[i];
    bind.length = &_lengths[i];
    bind.error = &_errors[i];
    bind.is_unsigned = field->flags & UNSIGNED_FLAG ? 1 : 0;
  }
  mysql_free_result(result);
  if(mysql_stmt_bind_result(_stmt, _binds.data()) != 0) {
    throw anch::sql::mysql::error("Fail to bind MySQL statement result", mysql_stmt_errno(_stmt), mysql_stmt_error(_stmt));
  }
}

MySQLPreparedStatementResultSet::~MySQLPreparedStatementResultSet() noexcept {
  mysql_stmt_free_result(_stmt);
}

bool
MySQLPreparedStatementResultSet::getValue(std::size_t idx, std::string& out) {
  if(idx >= _values.size()) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_values.size() - 1) << "): " << idx;
    throw SqlException(msg.str(), true);
  }
  if(!_values[idx].has_value()) {
    return true;
  }
  out = _values[idx].value();
  return false;
}

std::optional<std::string>
MySQLPreparedStatementResultSet::getValue(std::size_t idx) {
  if(idx >= _values.size()) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_values.size() - 1) << "): " << idx;
    throw SqlException(msg.str(), true);
  }
  return _values[idx];
}

bool
MySQLPreparedStatementResultSet::fetchNext() {
  int res = mysql_stmt_fetch(_stmt);
  if(res == MYSQL_NO_DATA) {
    return false;
  }
  if(res != 0 && res != MYSQL_DATA_TRUNCATED) {
    throw anch::sql::mysql::error("Error while fetching MySQL statement row", mysql_stmt_errno(_stmt), mysql_stmt_error(_stmt));
  }
  for(std::size_t i = 0 ; i < _binds.size() ; ++i) {
    if(_nulls[i] != 0) {
      _values[i].reset();
    } else if(_lengths[i] <= _buffers[i].size()) {
      _values[i] = std::string(_buffers[i].data(), _lengths[i]);
    } else { // truncated value: read it again with its real size
      std::string value(_lengths[i], '\0');
      unsigned long length = 0;
      MYSQL_BIND bind;
      std::memset(&bind, 0, sizeof(bind));
      bind.buffer_type = MYSQL_TYPE_STRING;
      bind.buffer = value.data();
      bind.buffer_length = value.size();
      bind.length = &length;
      if(mysql_stmt_fetch_column(_stmt, &bind, static_cast<unsigned int>(i), 0) != 0) {
	throw anch::sql::mysql::error("Error while fetching MySQL statement column", mysql_stmt_errno(_stmt), mysql_stmt_error(_stmt));
      }
      _values[i] = std::move(value);
    }
  }
  return true;
}

const anch::date::DateFormatter&
MySQLPreparedStatementResultSet::getTimestampFormatter() {
  return getDefaultTimestampFormatter();
}

#endif // ANCH_SQL_MYSQL
