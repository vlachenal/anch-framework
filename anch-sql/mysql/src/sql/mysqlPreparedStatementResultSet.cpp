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
#ifdef ANCH_SQL_MYSQL

//#include "mysql.h"
#include "errmsg.h"

#include "sql/mysqlPreparedStatementResultSet.hpp"

#include <string>
#include <cstring>

#include <iostream>

using anch::sql::MySQLPreparedStatementResultSet;
using anch::sql::ResultSet;


MySQLPreparedStatementResultSet::MySQLPreparedStatementResultSet(MYSQL_STMT* stmt):
  ResultSet(),
  _stmt(stmt) {
  MYSQL_RES* result = mysql_stmt_result_metadata(_stmt);
  if(result == NULL) {
    unsigned int res = mysql_stmt_errno(_stmt);
    bool ok = res != CR_OUT_OF_MEMORY;
    std::ostringstream msg;
    msg << "Fail to get MySQL statement result metadata " << res << ": " << mysql_stmt_error(_stmt);
    throw SqlException(msg.str(), ok);
  }
  unsigned int nbFields = mysql_num_fields(result);
  _binds = new MYSQL_BIND[nbFields];
  std::memset(_binds, 0, sizeof(*_binds) * nbFields);
  _lengths = new long unsigned int[nbFields];
  //_nulls = new char[nbFields];
  _nulls = new bool[nbFields];
  for(unsigned int i = 0 ; i < nbFields ; ++i) {
    MYSQL_BIND* bind = &_binds[i];
    MYSQL_FIELD* field = mysql_fetch_field(result);
    _fields[field->name] = i;
    bind->buffer_type = MYSQL_TYPE_STRING;
    long unsigned int bufferSize = field->length + 1;
    char* value = new char[bufferSize];
    std::memset(value, 0, bufferSize);
    bind->buffer = value;
    bind->buffer_length = bufferSize;
    bind->is_null = &_nulls[i];
    bind->length = &_lengths[i];
    bind->is_unsigned = field->flags & UNSIGNED_FLAG ? 1 : 0;
  }
  int res = mysql_stmt_bind_result(_stmt, _binds);
  if(res != 0) {
    bool ok = res != CR_OUT_OF_MEMORY;
    std::ostringstream msg;
    msg << "Fail to bind MySQL statement result " << mysql_stmt_errno(_stmt) << ": " << mysql_stmt_error(_stmt);
    throw SqlException(msg.str(), ok);
  }
  mysql_free_result(result);
}

MySQLPreparedStatementResultSet::~MySQLPreparedStatementResultSet() noexcept {
  delete[] _lengths;
  delete[] _nulls;
  for(std::size_t i = 0 ; i < _fields.size() ; ++i) {
    MYSQL_BIND* bind = &_binds[i];
    delete[] reinterpret_cast<char*>(bind->buffer);
    bind->buffer = NULL;
  }
  delete[] _binds;
}

bool
MySQLPreparedStatementResultSet::getValue(std::size_t idx, std::string& out) {
  if(idx >= _fields.size()) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_fields.size() - 1) << "): " << idx;
    throw SqlException(msg.str(), true);
  }
  bool null = (_nulls[idx] != 0);
  if(!null) {
    out = std::string(static_cast<char*>(_binds[idx].buffer), _lengths[idx]);
  }
  return null;
}

#ifdef ANCH_STD_OTP
std::optional<std::string>
MySQLPreparedStatementResultSet::getValue(std::size_t idx) {
  if(idx >= _fields.size()) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_fields.size() - 1) << "): " << idx;
    throw SqlException(msg.str(), true);
  }
  std::optional<std::string> res;
  if(_nulls[idx] != 0) {
    res = std::string(static_cast<char*>(_binds[idx].buffer), _lengths[idx]);
  }
  return res;
}
#endif // ANCH_STD_OTP

bool
MySQLPreparedStatementResultSet::next() {
  return (mysql_stmt_fetch(_stmt) == 0);
}

const anch::date::DateFormatter&
MySQLPreparedStatementResultSet::getDateFormatter() {
  return getDefaultDateFormatter();
}

const anch::date::DateFormatter&
MySQLPreparedStatementResultSet::getTimeFormatter() {
  return getDefaultTimeFormatter();
}

const anch::date::DateFormatter&
MySQLPreparedStatementResultSet::getTimestampFormatter() {
  return getDefaultTimestampFormatter();
}

#endif // ANCH_SQL_MYSQL
