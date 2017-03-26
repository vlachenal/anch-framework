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
#ifdef ANCH_SQL_SQLITE3

#include "sql/sqlite3PreparedStatement.hpp"
#include "sql/sqlite3ResultSet.hpp"

#include <sstream>

#include "sqlite3.h"

using anch::sql::PreparedStatement;
using anch::sql::SQLite3PreparedStatement;
using anch::sql::SQLite3ResultSet;
using anch::sql::SqlException;
using anch::sql::ResultSet;

std::atomic<std::uint64_t> SQLite3PreparedStatement::_counter(0);

// Constructors +
SQLite3PreparedStatement::SQLite3PreparedStatement(sqlite3* dbCon, const std::string& query) throw(SqlException): PreparedStatement(), _conn(dbCon), _stmt() {
  int res = sqlite3_prepare_v2(_conn, query.data(), -1, &_stmt, NULL);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Error while preparing statement " << query << ": " << sqlite3_errmsg(_conn);
    throw SqlException(msg.str());
  }
  std::set<std::size_t> pos = getPlaceholders(query);
  _nbPlaceholders = pos.size();
}
// Constructors -

// Destructor +
SQLite3PreparedStatement::~SQLite3PreparedStatement() {
  if(_stmt != NULL) {
    sqlite3_clear_bindings(_stmt);
    sqlite3_finalize(_stmt);
  }
}
// Destructor -

// Functions +
/*!
 * Bind prepared statement parameters and send query to server
 *
 * \param conn the SQLite3 connection
 * \param stmt the prepared statement
 * \param paramValues the values to bind
 *
 * \throw SqlException any error
 */
void
bindParamsAndSend(sqlite3* conn,
		  sqlite3_stmt* stmt,
		  const std::map<std::size_t,std::string>& paramValues) throw(SqlException) {
  sqlite3_clear_bindings(stmt);
  // Bind parameters +
  int res = SQLITE_OK;
  for(auto iter = paramValues.cbegin() ; iter != paramValues.cend() ; ++iter) {
    sqlite3_bind_text(stmt, static_cast<int>(iter->first), iter->second.data(), static_cast<int>(iter->second.length()), SQLITE_STATIC);
    if(res != SQLITE_OK) {
      std::ostringstream msg;
      msg << "Error while binding parameter [" << iter->first << "] = " << iter->second << ": " << sqlite3_errmsg(conn);
      throw SqlException(msg.str());
    }
  }
  // Bind parameters -
}
// Functions -

// Methods +
ResultSet*
SQLite3PreparedStatement::executeQuery() throw(SqlException) {
  bindParamsAndSend(_conn, _stmt, _values);
  return new SQLite3ResultSet(_stmt, true);
}

uint64_t
SQLite3PreparedStatement::executeUpdate() throw(SqlException) {
  bindParamsAndSend(_conn, _stmt, _values);
  return static_cast<uint64_t>(sqlite3_changes(_conn));
}
// Methods -

#endif // ANCH_SQL_SQLITE3
