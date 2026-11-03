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
#include "sql/sqlite3Connection.hpp"

#include <sstream>

#include "sqlite3.h"

using anch::sql::PreparedStatement;
using anch::sql::SQLite3PreparedStatement;
using anch::sql::SQLite3ResultSet;
using anch::sql::SqlException;
using anch::sql::ResultSet;

std::atomic<std::uint64_t> SQLite3PreparedStatement::_counter(0);

// Constructors +
SQLite3PreparedStatement::SQLite3PreparedStatement(sqlite3* dbCon, const std::string& query): PreparedStatement(), _conn(dbCon), _stmt() {
  int res = sqlite3_prepare_v2(_conn, query.data(), static_cast<int>(query.size()), &_stmt, NULL);
  if(res != SQLITE_OK) {
    throw anch::sql::sqlite::error("Error while preparing statement " + query, _conn, res);
  }
  setParameterCount(static_cast<std::size_t>(sqlite3_bind_parameter_count(_stmt))); // counted by SQLite parser
}
// Constructors -

// Destructor +
SQLite3PreparedStatement::~SQLite3PreparedStatement() noexcept {
  if(_stmt != NULL) {
    sqlite3_finalize(_stmt);
  }
}
// Destructor -

// Functions +
/*!
 * Bind prepared statement parameters
 *
 * \param conn the SQLite3 connection
 * \param stmt the prepared statement
 * \param paramValues the values to bind (\c std::nullopt for \c NULL )
 *
 * \throw SqlException any error
 */
static void
bindParams(sqlite3* conn, sqlite3_stmt* stmt, const std::vector<std::optional<std::string>>& paramValues) {
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  for(std::size_t i = 0 ; i < paramValues.size() ; ++i) {
    int idx = static_cast<int>(i + 1);
    int res = SQLITE_OK;
    if(paramValues[i].has_value()) {
      res = sqlite3_bind_text64(stmt, idx, paramValues[i]->data(), paramValues[i]->size(), SQLITE_TRANSIENT, SQLITE_UTF8);
    } else {
      res = sqlite3_bind_null(stmt, idx);
    }
    if(res != SQLITE_OK) {
      throw anch::sql::sqlite::error("Error while binding parameter " + std::to_string(idx), conn, res);
    }
  }
}
// Functions -

// Methods +
ResultSet*
SQLite3PreparedStatement::doExecuteQuery() {
  bindParams(_conn, _stmt, _values);
  return new SQLite3ResultSet(_stmt, true);
}

uint64_t
SQLite3PreparedStatement::doExecuteUpdate() {
  bindParams(_conn, _stmt, _values);
  int res = SQLITE_ROW;
  while((res = sqlite3_step(_stmt)) == SQLITE_ROW) {
    // Ignore rows
  }
  if(res != SQLITE_DONE) { // constraint violation, busy database, ...
    SqlException error = anch::sql::sqlite::error("Error while executing prepared statement", _conn, res);
    sqlite3_reset(_stmt);
    throw error;
  }
  sqlite3_reset(_stmt);
  return static_cast<uint64_t>(sqlite3_changes64(_conn));
}
// Methods -

#endif // ANCH_SQL_SQLITE3
