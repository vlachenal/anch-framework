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
#include "sql/mysqlConnection.hpp"

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
  if(mysql_stmt_prepare(_stmt, query.data(), query.length()) != 0) {
    SqlException error = anch::sql::mysql::error("Fail to prepare MySQL statement " + query, mysql_stmt_errno(_stmt), mysql_stmt_error(_stmt));
    mysql_stmt_close(_stmt);
    throw error;
  }
  setParameterCount(mysql_stmt_param_count(_stmt)); // counted by MySQL server
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
 * \param paramValues the values to bind (\c std::nullopt for \c NULL )
 *
 * \throw SqlException any error
 */
static void
bindParamsAndSend(MYSQL_STMT* stmt, const std::vector<std::optional<std::string>>& paramValues) {
  std::vector<MYSQL_BIND> binds(paramValues.size());
  std::memset(binds.data(), 0, binds.size() * sizeof(MYSQL_BIND));
  for(std::size_t i = 0 ; i < paramValues.size() ; ++i) {
    if(paramValues[i].has_value()) {
      binds[i].buffer_type = MYSQL_TYPE_STRING;
      binds[i].buffer = const_cast<char*>(paramValues[i]->data());
      binds[i].buffer_length = paramValues[i]->length();
    } else {
      binds[i].buffer_type = MYSQL_TYPE_NULL;
    }
  }
  if(!binds.empty() && mysql_stmt_bind_param(stmt, binds.data()) != 0) {
    throw anch::sql::mysql::error("Fail to bind MySQL statement parameters", mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
  }
  if(mysql_stmt_execute(stmt) != 0) {
    throw anch::sql::mysql::error("Fail to execute MySQL statement", mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
  }
}
// Functions -

// Methods +
ResultSet*
MySQLPreparedStatement::doExecuteQuery() {
  bindParamsAndSend(_stmt, _values);
  return new MySQLPreparedStatementResultSet(_stmt);
}

uint64_t
MySQLPreparedStatement::doExecuteUpdate() {
  bindParamsAndSend(_stmt, _values);
  return static_cast<uint64_t>(mysql_stmt_affected_rows(_stmt));
}
// Methods -

#endif // ANCH_SQL_MYSQL
