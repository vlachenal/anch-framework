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

#include <sstream>

using anch::sql::PreparedStatement;
using anch::sql::PostgreSQLPreparedStatement;
using anch::sql::SqlException;
using anch::sql::ResultSet;


// Constructors +
PostgreSQLPreparedStatement::PostgreSQLPreparedStatement(PGconn* dbCon, const std::string& query) throw(SqlException): PreparedStatement() {
  std::set<std::size_t> pos = getWildCards(query);
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
  PGresult* res = PQprepare(dbCon, query.data(), query.data(), -1, NULL);
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
  return NULL;
}

void
PostgreSQLPreparedStatement::set(std::size_t /*idx*/, int16_t /*value*/) throw(SqlException) {

}

void
PostgreSQLPreparedStatement::set(std::size_t /*idx*/, uint16_t /*value*/) throw(SqlException) {

}

void
PostgreSQLPreparedStatement::set(std::size_t /*idx*/, int32_t /*value*/) throw(SqlException) {

}

void
PostgreSQLPreparedStatement::set(std::size_t /*idx*/, uint32_t /*value*/) throw(SqlException) {

}

void
PostgreSQLPreparedStatement::set(std::size_t /*idx*/, int64_t /*value*/) throw(SqlException) {

}

void
PostgreSQLPreparedStatement::set(std::size_t /*idx*/, uint64_t /*value*/) throw(SqlException) {

}

void
PostgreSQLPreparedStatement::set(std::size_t /*idx*/, const std::string& /*value*/) throw(SqlException) {

}

#endif // ANCH_SQL_POSTGRESQL
