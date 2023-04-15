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
MySQLPreparedStatement::MySQLPreparedStatement(MYSQL* dbCon, const std::string& query): PreparedStatement(), _conn(dbCon) {
  _stmt = mysql_stmt_init(dbCon);
  if(_stmt == NULL) {
    throw SqlException("MySQL out of memory", false);
  }
  int res = mysql_stmt_prepare(_stmt, query.data(), query.length());
  if(res != 0) {
    bool ok = res != CR_SERVER_GONE_ERROR && res != CR_SERVER_LOST && res != CR_OUT_OF_MEMORY;
    std::ostringstream msg;
    msg << "Fail to prepare MySQL statement " << mysql_stmt_errno(_stmt) << ": " << mysql_stmt_error(_stmt);
    mysql_stmt_close(_stmt);
    throw SqlException(msg.str(), ok);
  }
  std::set<std::size_t> pos = getPlaceholders(query);
  _nbPlaceholders = pos.size();
}
// Constructors -

// Destructor +
MySQLPreparedStatement::~MySQLPreparedStatement() noexcept {
  mysql_stmt_close(_stmt);
}
// Destructor -

// Functions +
/*!
 * Bind prepared statement parameters and send query to server
 *
 * \param stmt the MySQL prepared statement
 * \param nbPlaceholders the number of placeholders in query
 * \param paramValues the values to bind
 *
 * \throw SqlException any error
 */
void
bindParamsAndSend(MYSQL_STMT* stmt,
		  std::size_t nbPlaceholders,
		  const std::map<std::size_t,std::string>& paramValues) {
  // Bind parameters +
  MYSQL_BIND* bind = new MYSQL_BIND[nbPlaceholders];
  std::memset(bind, 0, nbPlaceholders * sizeof(MYSQL_BIND));
  std::size_t idx = 0;
  for(auto iter = paramValues.cbegin() ; iter != paramValues.cend() ; ++iter) {
    bind[idx].buffer_type = MYSQL_TYPE_STRING;
    bind[idx].buffer = (void*)iter->second.data();
    bind[idx].buffer_length = iter->second.length();
    bind[idx].length = 0;
    bind[idx].is_null = 0;
    ++idx;
  }
  int res = mysql_stmt_bind_param(stmt, bind);
  if(res != 0) {
    delete[] bind;
    bool ok = res != CR_OUT_OF_MEMORY;
    std::ostringstream msg;
    msg << "Fail to bind MySQL statement parameters " << mysql_stmt_errno(stmt) << ": " << mysql_stmt_error(stmt);
    throw SqlException(msg.str(), ok);
  }
  // Bind parameters -
  // Execute statement +
  res = mysql_stmt_execute(stmt);
  if(res != 0) {
    bool ok = res != CR_SERVER_GONE_ERROR && res != CR_SERVER_LOST && res != CR_OUT_OF_MEMORY;
    delete[] bind;
    std::ostringstream msg;
    msg << "Fail to execute MySQL statement " << mysql_stmt_errno(stmt) << ": " << mysql_stmt_error(stmt);
    throw SqlException(msg.str(), ok);
  }
  delete[] bind;
  // Execute statement -
}
// Functions -

// Methods +
ResultSet*
MySQLPreparedStatement::executeQuery() {
  bindParamsAndSend(_stmt, _nbPlaceholders, _values);
  return new MySQLPreparedStatementResultSet(_stmt);
}

uint64_t
MySQLPreparedStatement::executeUpdate() {
  bindParamsAndSend(_stmt, _nbPlaceholders, _values);
  return static_cast<uint64_t>(mysql_stmt_affected_rows(_stmt));
}
// Methods -

#endif // ANCH_SQL_MYSQL
