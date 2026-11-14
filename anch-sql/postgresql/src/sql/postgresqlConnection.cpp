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

#include "sql/postgresqlConnection.hpp"
#include "sql/postgresqlResultSet.hpp"
#include "sql/postgresqlPreparedStatement.hpp"

#include <charconv>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#include "libpq-fe.h"


using anch::sql::Connection;
using anch::sql::SqlException;
using anch::sql::SqlConnectionConfiguration;
using anch::sql::PostgreSQLConnection;
using anch::sql::ResultSet;
using anch::sql::PostgreSQLResultSet;
using anch::sql::PreparedStatement;
using anch::sql::PostgreSQLPreparedStatement;


// Constructors +
PostgreSQLConnection::PostgreSQLConnection(const std::string& connStr): _conn() {
  _conn = PQconnectdb(connStr.data());
  if(PQstatus(_conn) != CONNECTION_OK) {
    _valid = false;
    std::ostringstream msg;
    msg << "Fail to connect PostgreSQL database: " << PQerrorMessage(_conn);
    PQfinish(_conn);
    throw SqlException(msg.str(), _valid);
  }
}

PostgreSQLConnection::PostgreSQLConnection(const SqlConnectionConfiguration& config):
  Connection(config.statementCacheSize),
  _conn(NULL) {
  // Parameters are given one by one: values are used as they are, whatever characters they contain
  // (a connection URI would need them to be percent-encoded)
  const std::string port = config.port > 0 ? std::to_string(config.port) : std::string();
  std::vector<const char*> keywords;
  std::vector<const char*> values;
  auto add = [&keywords, &values](const char* keyword, const std::string& value) {
    if(!value.empty()) {
      keywords.push_back(keyword);
      values.push_back(value.data());
    }
  };
  add("host", config.hostname);
  add("port", port);
  add("dbname", config.database);
  add("user", config.user);
  if(!config.user.empty()) {
    add("password", config.password);
  }
  add("application_name", config.application);
  keywords.push_back("keepalives");
  values.push_back("1");
  keywords.push_back(NULL);
  values.push_back(NULL);
  _conn = PQconnectdbParams(keywords.data(), values.data(), 0); // dbname is not expanded as a connection string
  if(PQstatus(_conn) != CONNECTION_OK) {
    _valid = false;
    std::ostringstream msg;
    msg << "Fail to connect PostgreSQL database " << config.database
	<< ":" << config.port << " with user " << config.user << ": " << PQerrorMessage(_conn);
    PQfinish(_conn);
    throw SqlException(msg.str(), _valid);
  }
}
// Constructors -

// Destructor +
PostgreSQLConnection::~PostgreSQLConnection() noexcept {
  if(_conn != NULL) {
    release();
    PQfinish(_conn);
  }
}
// Destructor -

// Methods +
/*!
 * Build SQL exception from PostgreSQL error: it is fatal when connection is broken
 *
 * \param message the error context
 * \param conn the PostgreSQL connection
 * \param res the PostgreSQL result (its error message is used when not \c NULL ; it is cleared)
 *
 * \return the exception
 */
static SqlException
pgError(const std::string& message, PGconn* conn, PGresult* res = NULL) {
  std::ostringstream msg;
  msg << message << ": " << (res != NULL ? PQresultErrorMessage(res) : PQerrorMessage(conn));
  if(res != NULL) {
    PQclear(res);
  }
  return SqlException(msg.str(), PQstatus(conn) == CONNECTION_OK);
}

/*!
 * Execute command which does not return rows
 *
 * \param conn the PostgreSQL connection
 * \param command the command
 * \param context the error context
 *
 * \throw SqlException any error
 */
static void
execCommand(PGconn* conn, const char* command, const std::string& context) {
  PGresult* res = PQexec(conn, command);
  if(PQresultStatus(res) != PGRES_COMMAND_OK) { // PQresultStatus(NULL) is PGRES_FATAL_ERROR
    throw pgError(context, conn, res);
  }
  PQclear(res);
}

bool
PostgreSQLConnection::checkConnection() const noexcept {
  return _conn != NULL && PQstatus(_conn) == CONNECTION_OK;
}

void
PostgreSQLConnection::sendCommit() {
  execCommand(_conn, "COMMIT", "Unable to commit transaction");
}

void
PostgreSQLConnection::sendRollback() {
  execCommand(_conn, "ROLLBACK", "Unable to rollback transaction");
}

void
PostgreSQLConnection::sendStartTransaction() {
  execCommand(_conn, "START TRANSACTION", "Unable to start transaction");
}

ResultSet*
PostgreSQLConnection::executeQuery(const std::string& query) {
  if(PQsendQuery(_conn, query.data()) != 1) {
    throw pgError("Unable to execute query " + query, _conn);
  }
  return new PostgreSQLResultSet(_conn);
}

uint64_t
PostgreSQLConnection::executeUpdate(const std::string& query) {
  PGresult* res = PQexec(_conn, query.data());
  switch(PQresultStatus(res)) {
  case PGRES_COMMAND_OK:
  case PGRES_TUPLES_OK: // UPDATE ... RETURNING
    break;
  case PGRES_EMPTY_QUERY:
  case PGRES_COPY_OUT:
  case PGRES_COPY_IN:
  case PGRES_COPY_BOTH:
    PQclear(res);
    throw SqlException("Unexpected update query: " + query, PQstatus(_conn) == CONNECTION_OK);
  default:
    throw pgError("Unable to execute query " + query, _conn, res);
  }
  uint64_t nbRows = anch::sql::postgresql::affectedRows(res);
  PQclear(res);
  return nbRows;
}

PreparedStatement*
PostgreSQLConnection::makePrepared(const std::string& query) {
  return new PostgreSQLPreparedStatement(_conn, query);
}

uint64_t
anch::sql::postgresql::affectedRows(PGresult* res) noexcept {
  const char* tuples = PQcmdTuples(res); // empty for commands without row count
  uint64_t nbRows = 0;
  std::from_chars(tuples, tuples + std::strlen(tuples), nbRows);
  return nbRows;
}

SqlException
anch::sql::postgresql::error(const std::string& message, PGconn* conn, PGresult* res) {
  return pgError(message, conn, res);
}
// Methods -

// C shared library definition +
/*!
 * C shared library connection maker
 *
 * \return the PostgreSQL connection
 */
extern "C"
std::shared_ptr<Connection>
create_shared_connection(const SqlConnectionConfiguration& config) {
  return std::make_shared<PostgreSQLConnection>(config);
}

/*!
 * C shared library connection maker
 *
 * \return the PostgreSQL connection
 */
extern "C"
Connection*
create_connection(const SqlConnectionConfiguration& config) {
  return new PostgreSQLConnection(config);
}

/*!
 * C shared library PostgreSQL connection free
 *
 * \param conn the connection to free
 */
extern "C"
void
delete_connection(PostgreSQLConnection* conn) {
  delete conn;
}
// C shared library definition -

#endif //ANCH_SQL_POSTGRESQL
