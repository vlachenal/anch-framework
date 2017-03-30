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

#include <sstream>

#include "libpq-fe.h"


using anch::sql::PreparedStatement;
using anch::sql::PostgreSQLPreparedStatement;
using anch::sql::PostgreSQLResultSet;
using anch::sql::SqlException;
using anch::sql::ResultSet;

std::atomic<std::uint64_t> PostgreSQLPreparedStatement::_counter(0);

// Constructors +
PostgreSQLPreparedStatement::PostgreSQLPreparedStatement(PGconn* dbCon, const std::string& query) throw(SqlException): PreparedStatement(), _conn(dbCon) {
  std::set<std::size_t> pos = getPlaceholders(query);
  _nbPlaceholders = pos.size();
  std::size_t offset = 0;
  std::ostringstream oss;
  std::size_t idx = 0;
  for(auto iter = pos.cbegin() ; iter != pos.cend() ; ++iter) {
    oss << query.substr(offset, *iter - offset) << '$' << ++idx;
    offset = *iter + 1;
  }
  if(offset < query.length()) {
    oss << query.substr(offset);
  }
  std::string pgQuery = oss.str();
  std::ostringstream ids;
  ids << std::addressof(dbCon) << '_' << _counter.fetch_add(1);
  _stmtId = ids.str();
  PGresult* res = PQprepare(dbCon, _stmtId.data(), pgQuery.data(), static_cast<int>(_nbPlaceholders), NULL);
  if(res == NULL || PQresultStatus(res) != PGRES_COMMAND_OK) {
    std::ostringstream msg;
    msg << "Fail to prepare PostgreSQL statement: " << PQerrorMessage(dbCon);
    if(res != NULL) {
      PQclear(res);
    }
    throw SqlException(msg.str());
  }
  PQclear(res);
}
// Constructors -

// Destructor +
PostgreSQLPreparedStatement::~PostgreSQLPreparedStatement() {
  // Nothing to do
}
// Destructor -

// Functions +
/*!
 * Bind prepared statement parameters and send query to server
 *
 * \param conn the PostgreSQL connection
 * \param stmtId the prepared statement identifier
 * \param nbPlaceholders the number of placeholders in query
 * \param paramValues the values to bind
 *
 * \throw SqlException any error
 */
void
bindParamsAndSend(PGconn* conn,
		  const std::string& stmtId,
		  std::size_t nbPlaceholders,
		  const std::map<std::size_t,std::string>& paramValues) throw(SqlException) {
  // Bind parameters +
  const char** values = new const char*[nbPlaceholders];
  int* lengths = new int[nbPlaceholders];
  std::size_t idx = 0;
  for(auto iter = paramValues.cbegin() ; iter != paramValues.cend() ; ++iter) {
    lengths[idx] = static_cast<int>(iter->second.length());
    values[idx] = iter->second.data();
    ++idx;
  }
  // Bind parameters -
  // Execute statement +
  int res = PQsendQueryPrepared(conn, stmtId.data(), static_cast<int>(nbPlaceholders), values, lengths, NULL, 0);
  if(res == 0) {
    delete[] values;
    delete[] lengths;
    std::ostringstream msg;
    msg << "Fail to execute PostgreSQL statement: " << PQerrorMessage(conn);
    throw SqlException(msg.str());
  }
  delete[] values;
  delete[] lengths;
  // Execute statement -
}
// Functions -

// Methods +
ResultSet*
PostgreSQLPreparedStatement::executeQuery() throw(SqlException) {
  bindParamsAndSend(_conn, _stmtId, _nbPlaceholders, _values);
  return new PostgreSQLResultSet(_conn);
}

uint64_t
PostgreSQLPreparedStatement::executeUpdate() throw(SqlException) {
  bindParamsAndSend(_conn, _stmtId, _nbPlaceholders, _values);
  PGresult* pgRes = PQgetResult(_conn);
  if(pgRes == NULL || PQresultStatus(pgRes) != PGRES_COMMAND_OK) {
    std::ostringstream msg;
    msg << "Error while executing PostgreSQL command: " << PQerrorMessage(_conn);
    PQclear(pgRes);
    throw SqlException(msg.str());
  }
  uint64_t nbRows = static_cast<uint64_t>(std::atoll(PQcmdTuples(pgRes)));
  PQclear(pgRes);
  // Consume next result to avoid error ... +
  if((pgRes = PQgetResult(_conn)) != NULL) {
    PQclear(pgRes);
    while((pgRes = PQgetResult(_conn)) != NULL) {
      PQclear(pgRes); // free PostgreSQL result
    }
    throw SqlException("Error: next result should be NULL ...");
  }
  // Consume next result to avoid error ... -
  return nbRows;
}
// Methods -

#endif // ANCH_SQL_POSTGRESQL
