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
#ifdef ANCH_SQL_POSTGRESQL

#include "sql/postgresqlPreparedStatement.hpp"
#include "sql/postgresqlResultSet.hpp"
#include "sql/postgresqlConnection.hpp"
#include "sql/placeholders.hpp"

#include <sstream>

#include "libpq-fe.h"


using anch::sql::PreparedStatement;
using anch::sql::PostgreSQLPreparedStatement;
using anch::sql::PostgreSQLResultSet;
using anch::sql::SqlException;
using anch::sql::ResultSet;

std::atomic<std::uint64_t> PostgreSQLPreparedStatement::_counter(0);

// Constructors +
PostgreSQLPreparedStatement::PostgreSQLPreparedStatement(PGconn* dbCon, const std::string& query): PreparedStatement(), _conn(dbCon) {
  // PostgreSQL uses numbered placeholders: '?' (outside literals, quoted identifiers and comments) are replaced by $n
  std::string pgQuery = anch::sql::numberPlaceholders(query, anch::sql::findPlaceholders(query, anch::sql::SqlSyntax::POSTGRESQL));
  _stmtId = "anch_stmt_" + std::to_string(_counter.fetch_add(1));
  PGresult* res = PQprepare(dbCon, _stmtId.data(), pgQuery.data(), 0, NULL); // parameter types are inferred
  if(PQresultStatus(res) != PGRES_COMMAND_OK) {
    throw anch::sql::postgresql::error("Fail to prepare PostgreSQL statement " + pgQuery, _conn, res);
  }
  PQclear(res);
  // Number of parameters is given by the server +
  res = PQdescribePrepared(dbCon, _stmtId.data());
  if(PQresultStatus(res) != PGRES_COMMAND_OK) {
    throw anch::sql::postgresql::error("Fail to describe PostgreSQL statement " + pgQuery, _conn, res);
  }
  setParameterCount(static_cast<std::size_t>(PQnparams(res)));
  PQclear(res);
  // Number of parameters is given by the server -
}
// Constructors -

// Destructor +
PostgreSQLPreparedStatement::~PostgreSQLPreparedStatement() noexcept {
  // Server side statement is deallocated with the session (connection may already be closed here)
}
// Destructor -

// Functions +
/*!
 * Bind prepared statement parameters and send query to server
 *
 * \param conn the PostgreSQL connection
 * \param stmtId the prepared statement identifier
 * \param paramValues the values to bind (\c std::nullopt for \c NULL )
 *
 * \throw SqlException any error
 */
static void
bindParamsAndSend(PGconn* conn, const std::string& stmtId, const std::vector<std::optional<std::string>>& paramValues) {
  std::vector<const char*> values(paramValues.size(), NULL);
  std::vector<int> lengths(paramValues.size(), 0);
  for(std::size_t i = 0 ; i < paramValues.size() ; ++i) {
    if(paramValues[i].has_value()) {
      values[i] = paramValues[i]->data();
      lengths[i] = static_cast<int>(paramValues[i]->size());
    }
  }
  if(PQsendQueryPrepared(conn, stmtId.data(), static_cast<int>(values.size()), values.data(), lengths.data(), NULL, 0) == 0) {
    throw anch::sql::postgresql::error("Fail to execute PostgreSQL statement", conn);
  }
}
// Functions -

// Methods +
ResultSet*
PostgreSQLPreparedStatement::doExecuteQuery() {
  bindParamsAndSend(_conn, _stmtId, _values);
  return new PostgreSQLResultSet(_conn);
}

uint64_t
PostgreSQLPreparedStatement::doExecuteUpdate() {
  bindParamsAndSend(_conn, _stmtId, _values);
  // Read every result (the connection can not be used until the last one has been read) +
  PGresult* res = PQgetResult(_conn);
  PGresult* next = NULL;
  while((next = PQgetResult(_conn)) != NULL) { // should not happen for a single command
    PQclear(next);
  }
  // Read every result (the connection can not be used until the last one has been read) -
  ExecStatusType status = PQresultStatus(res);
  if(status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
    throw anch::sql::postgresql::error("Error while executing PostgreSQL command", _conn, res);
  }
  uint64_t nbRows = anch::sql::postgresql::affectedRows(res);
  PQclear(res);
  return nbRows;
}
// Methods -

#endif // ANCH_SQL_POSTGRESQL
