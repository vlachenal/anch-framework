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

#include "sql/mysqlPreparedStatement.hpp"

#include <sstream>
#include <cstring>

#include "sql/mysqlPreparedStatementResultSet.hpp"

using anch::sql::PreparedStatement;
using anch::sql::MySQLPreparedStatement;
using anch::sql::MySQLPreparedStatementResultSet;
using anch::sql::SqlException;
using anch::sql::ResultSet;

std::atomic<std::uint64_t> MySQLPreparedStatement::_counter(0);

// Constructors +
MySQLPreparedStatement::MySQLPreparedStatement(MYSQL* dbCon, const std::string& query) throw(SqlException): PreparedStatement(), _conn(dbCon) {
  _stmt = mysql_stmt_init(dbCon);
  if(_stmt == NULL) {
    throw SqlException("MySQL out of memory");
  }
  int res = mysql_stmt_prepare(_stmt, query.data(), query.length());
  if(res != 0) {
    std::ostringstream msg;
    msg << "Fail to prepare MySQL statement " << mysql_stmt_errno(_stmt) << ": " << mysql_stmt_error(_stmt);
    mysql_stmt_close(_stmt);
    throw SqlException(msg.str());
  }
  std::set<std::size_t> pos = getPlaceholders(query);
  _nbPlaceholders = pos.size();
}
// Constructors -

// Destructor +
MySQLPreparedStatement::~MySQLPreparedStatement() {
  mysql_stmt_close(_stmt);
}
// Destructor -

// Methods +
ResultSet*
MySQLPreparedStatement::execute() throw(SqlException) {
  // Bind parameters +
  MYSQL_BIND* bind = new MYSQL_BIND[_nbPlaceholders];
  std::memset(bind, 0, _nbPlaceholders * sizeof(MYSQL_BIND));
  std::size_t idx = 0;
  for(auto iter = _values.cbegin() ; iter != _values.cend() ; ++iter) {
    bind[idx].buffer_type = MYSQL_TYPE_STRING;
    bind[idx].buffer = (void*)iter->second.data();
    bind[idx].buffer_length = iter->second.length();
    bind[idx].length = 0;
    bind[idx].is_null = 0;
    ++idx;
  }
  if(mysql_stmt_bind_param(_stmt, bind) != 0) {
    delete[] bind;
    std::ostringstream msg;
    msg << "Fail to bind MySQL statement parameters " << mysql_stmt_errno(_stmt) << ": " << mysql_stmt_error(_stmt);
    throw SqlException(msg.str());
  }
  // Bind parameters -
  // Execute statement +
  if(mysql_stmt_execute(_stmt) != 0) {
    delete[] bind;
    std::ostringstream msg;
    msg << "Fail to execute MySQL statement " << mysql_stmt_errno(_stmt) << ": " << mysql_stmt_error(_stmt);
    throw SqlException(msg.str());
  }
  delete[] bind;
  // Execute statement -
  return new MySQLPreparedStatementResultSet(_stmt);
}

#endif // ANCH_SQL_MYSQL
