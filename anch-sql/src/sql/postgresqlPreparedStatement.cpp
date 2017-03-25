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
  std::set<std::size_t> pos = getWildCards(query);
  _nbWildcards = pos.size();
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
  PGresult* res = PQprepare(dbCon, _stmtId.data(), pgQuery.data(), static_cast<int>(_nbWildcards), NULL);
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

// Methods +
ResultSet*
PostgreSQLPreparedStatement::execute() throw(SqlException) {
  // Bind parameters +
  const char** values = new const char*[_nbWildcards];
  int* lengths = new int[_nbWildcards];
  std::size_t idx = 0;
  for(auto iter = _values.cbegin() ; iter != _values.cend() ; ++iter) {
    lengths[idx] = static_cast<int>(iter->second.length());
    values[idx] = iter->second.data();
    ++idx;
  }
  // Bind parameters -
  // Execute statement +
  int res = PQsendQueryPrepared(_conn, _stmtId.data(), static_cast<int>(_nbWildcards), values, lengths, NULL, 0);
  if(res == 0) {
    delete[] values;
    delete[] lengths;
    std::ostringstream msg;
    msg << "Fail to execute PostgreSQL statement: " << PQerrorMessage(_conn);
    throw SqlException(msg.str());
  }
  delete[] values;
  delete[] lengths;
  // Execute statement -
  return new PostgreSQLResultSet(_conn);
}

#endif // ANCH_SQL_POSTGRESQL
