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

#include "mysql.h"

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
    std::ostringstream msg;
    msg << "Fail to get MySQL statement result metadata " << mysql_stmt_errno(_stmt) << ": " << mysql_stmt_error(_stmt);
    throw SqlException(msg.str());
  }
  unsigned int nbFields = mysql_num_fields(result);
  _binds = new MYSQL_BIND[nbFields];
  std::memset(_binds, 0, sizeof(*_binds) * nbFields);
  _lengths = new long unsigned int[nbFields];
  _nulls = new char[nbFields];
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
  if(mysql_stmt_bind_result(_stmt, _binds) != 0) {
    std::ostringstream msg;
    msg << "Fail to bind MySQL statement result " << mysql_stmt_errno(_stmt) << ": " << mysql_stmt_error(_stmt);
    throw SqlException(msg.str());
  }
  mysql_free_result(result);
}

MySQLPreparedStatementResultSet::~MySQLPreparedStatementResultSet() {
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
MySQLPreparedStatementResultSet::getValue(std::size_t idx, std::string& out) throw(SqlException) {
  if(idx >= _fields.size()) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_fields.size() - 1) << "): " << idx;
    throw SqlException(msg.str());
  }
  bool null = (_nulls[idx] != 0);
  if(!null) {
    out = std::string(static_cast<char*>(_binds[idx].buffer), _lengths[idx]);
  }
  return null;
}

bool
MySQLPreparedStatementResultSet::next() throw(SqlException) {
  return (mysql_stmt_fetch(_stmt) == 0);
}

#endif // ANCH_SQL_MYSQL
