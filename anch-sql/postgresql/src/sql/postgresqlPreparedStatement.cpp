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
  // PostgreSQL uses numbered parameters: ':name' -> $n, repeated for each use of a name
  doPrepare(parse(query, anch::sql::SqlSyntax::POSTGRESQL, anch::sql::ParameterStyle::DOLLAR));
}
// Constructors -

// Destructor +
PostgreSQLPreparedStatement::~PostgreSQLPreparedStatement() noexcept {
  // Server side statement is deallocated with the session (connection may already be closed here)
}
// Destructor -

// Functions +
/*!
 * Deallocate statement on the server side (best effort: the statement is deallocated with the session)
 *
 * \param conn the PostgreSQL connection
 * \param stmtId the prepared statement identifier
 */
static void
closeStatement(PGconn* conn, const std::string& stmtId) noexcept {
#ifdef LIBPQ_HAS_CLOSE_PREPARED
  PQclear(PQclosePrepared(conn, stmtId.data()));
#else
  if(PQtransactionStatus(conn) != PQTRANS_IDLE && PQtransactionStatus(conn) != PQTRANS_INTRANS) {
    return; // aborted transaction or connection busy: DEALLOCATE would fail
  }
  try {
    PQclear(PQexec(conn, ("DEALLOCATE " + stmtId).data())); // identifier is generated: no quoting needed
  } catch(...) {
    // Statement is deallocated with the session
  }
#endif
}

/*!
 * Bind prepared statement parameters and send query to server
 *
 * \param conn the PostgreSQL connection
 * \param stmtId the prepared statement identifier
 * \param statement the statement ( \c $n values)
 *
 * \throw SqlException any error
 */
static void
bindParamsAndSend(PGconn* conn, const std::string& stmtId, const PostgreSQLPreparedStatement& statement) {
  std::vector<const char*> values(statement.getBindingCount(), NULL);
  std::vector<int> lengths(values.size(), 0);
  for(std::size_t i = 0 ; i < values.size() ; ++i) {
    const anch::sql::ParameterValue& value = statement.getBindingValue(i);
    if(value.has_value()) {
      values[i] = value->data();
      lengths[i] = static_cast<int>(value->size());
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
  bindParamsAndSend(_conn, _stmtId, *this);
  return new PostgreSQLResultSet(_conn);
}

uint64_t
PostgreSQLPreparedStatement::doExecuteUpdate() {
  bindParamsAndSend(_conn, _stmtId, *this);
  // Read every result (the connection can not be used until the last one has been read) +
  PGresult* res = PQgetResult(_conn);
  PGresult* next = NULL;
  while((next = PQgetResult(_conn)) != NULL) { // should not happen for a single command
    PQclear(next);
  }
  // Read every result (the connection can not be used until the last one has been read) -
  ExecStatusType status = PQresultStatus(res);
  if(status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
    if(anch::sql::postgresql::invalidatesStatement(res)) {
      invalidate();
    }
    throw anch::sql::postgresql::error("Error while executing PostgreSQL command", _conn, res);
  }
  uint64_t nbRows = anch::sql::postgresql::affectedRows(res);
  PQclear(res);
  return nbRows;
}

void
PostgreSQLPreparedStatement::deallocate() noexcept {
  closeStatement(_conn, _stmtId);
}

void
PostgreSQLPreparedStatement::doPrepare(const std::string& query) {
  // A new statement replaces the previous one (bound lists have changed the query)
  std::string stmtId = "anch_stmt_" + std::to_string(_counter.fetch_add(1));
  PGresult* res = PQprepare(_conn, stmtId.data(), query.data(), 0, NULL); // parameter types are inferred
  if(PQresultStatus(res) != PGRES_COMMAND_OK) {
    throw anch::sql::postgresql::error("Fail to prepare PostgreSQL statement " + query, _conn, res);
  }
  PQclear(res);
  if(!_stmtId.empty()) {
    closeStatement(_conn, _stmtId);
  }
  _stmtId = std::move(stmtId);
}
// Methods -

#endif // ANCH_SQL_POSTGRESQL
