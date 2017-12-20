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

#include "sql/postgresqlConnection.hpp"
#include "sql/postgresqlResultSet.hpp"
#include "sql/postgresqlPreparedStatement.hpp"
#include "convert.hpp"

#include <sstream>

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
PostgreSQLConnection::PostgreSQLConnection(const std::string& host,
					   const std::string& user,
					   const std::string& password,
					   const std::string& database,
					   int port,
					   const std::string& app):
  Connection(),
  _conn(NULL) {
  std::ostringstream conninfo;
  conninfo << "postgresql://";
  if(!user.empty()) {
    conninfo << user;
    if(!password.empty()) {
      conninfo << ":";
      conninfo << password;
    }
    conninfo << "@";
  }
  if(!host.empty()) {
    conninfo << host;
  }
  if(port > 0) {
    conninfo << ":";
    conninfo << port;
  }
  if(!database.empty()) {
    conninfo << "/";
    conninfo << database;
  }
  conninfo << "?keepalives=1";
  if(!app.empty()) {
    conninfo << "&application_name=";
    conninfo << app;
  }
  _conn = PQconnectdb(conninfo.str().data());
  if(PQstatus(_conn) != CONNECTION_OK) {
    _valid = false;
    std::ostringstream msg;
    msg << "Fail to connect PostgreSQL database " << database
	<< ":" << port << " with user " << user << ": " << PQerrorMessage(_conn);
    PQfinish(_conn);
    throw SqlException(msg.str(), _valid);
  }
}

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
  Connection(),
  _conn(NULL) {
  std::ostringstream conninfo;
  conninfo << "postgresql://";
  if(!config.user.empty()) {
    conninfo << config.user;
    if(!config.password.empty()) {
      conninfo << ":";
      conninfo << config.password;
    }
    conninfo << "@";
  }
  if(!config.hostname.empty()) {
    conninfo << config.hostname;
  }
  if(config.port > 0) {
    conninfo << ":";
    conninfo << config.port;
  }
  if(!config.database.empty()) {
    conninfo << "/";
    conninfo << config.database;
  }
  conninfo << "?keepalives=1";
  if(!config.application.empty()) {
    conninfo << "&application_name=";
    conninfo << config.application;
  }
  _conn = PQconnectdb(conninfo.str().data());
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
    PQfinish(_conn);
  }
}
// Destructor -

// Methods +
void
PostgreSQLConnection::sendCommit() {
  PGresult* res = PQexec(_conn, "COMMIT");
  ExecStatusType status = PQresultStatus(res);
  if(status == PGRES_FATAL_ERROR) {
    _valid = false;
    std::ostringstream msg;
    msg << "Unable to commit transaction: " << PQresultErrorMessage(res);
    PQclear(res);
    throw SqlException(msg.str(), _valid);
  }
  PQclear(res);
}

void
PostgreSQLConnection::sendRollback() {
  PGresult* res = PQexec(_conn, "ROLLBACK");
  ExecStatusType status = PQresultStatus(res);
  if(status == PGRES_FATAL_ERROR) {
    std::ostringstream msg;
    msg << "Unable to rollback transaction: " << PQresultErrorMessage(res);
    PQclear(res);
    throw SqlException(msg.str(), false);
  }
  PQclear(res);
}

void
PostgreSQLConnection::sendStartTransaction() {
  PGresult* res = PQexec(_conn, "START TRANSACTION");
  ExecStatusType status = PQresultStatus(res);
  if(status == PGRES_FATAL_ERROR) {
    _valid = false;
    std::ostringstream msg;
    msg << "Unable to start transaction: " << PQresultErrorMessage(res);
    PQclear(res);
    throw SqlException(msg.str(), false);
  }
  PQclear(res);
}

ResultSet*
PostgreSQLConnection::executeQuery(const std::string& query) {
  int res = PQsendQuery(_conn, query.data());
  if(res != 1) {
    _valid = (PQstatus(_conn) == CONNECTION_OK);
    std::ostringstream msg;
    msg << "Unable to execute query " << query << ": " << PQerrorMessage(_conn);
    throw SqlException(msg.str(), _valid);
  }
  return new PostgreSQLResultSet(_conn);
}

uint64_t
PostgreSQLConnection::executeUpdate(const std::string& query) {
  PGresult* res = PQexec(_conn, query.data());
  if(res == NULL) {
    _valid = (PQstatus(_conn) == CONNECTION_OK);
    std::ostringstream msg;
    msg << "Unable to execute query " << query << ": " << PQerrorMessage(_conn);
    throw SqlException(msg.str(), _valid);
  }
  ExecStatusType status = PQresultStatus(res);
  switch(status) {
  case PGRES_COMMAND_OK:
    // OK => continue
    break;
  case PGRES_EMPTY_QUERY:
  case PGRES_TUPLES_OK:
  case PGRES_COPY_OUT:
  case PGRES_COPY_IN:
  case PGRES_COPY_BOTH:
    {
      std::ostringstream msg;
      msg << "Unexpected update query: " << query;
      PQclear(res);
      throw SqlException(msg.str(), true);
    }
    break;
  case PGRES_FATAL_ERROR:
    _valid = false; // Do not break => go to default and throw execption
    [[fallthrough]];
  default:
    {
      std::ostringstream msg;
      msg << "Unable to execute query " << query << ": " << PQerrorMessage(_conn);
      PQclear(res);
      throw SqlException(msg.str(), _valid);
    }
  }
  std::string strRes = PQcmdTuples(res);
  PQclear(res);
  uint64_t nbRows = 0;
  try {
    nbRows = convert<uint64_t>(strRes);
  } catch(std::bad_cast&) {
    // Nothing to do
  }
  return nbRows;
}

PreparedStatement*
PostgreSQLConnection::makePrepared(const std::string& query) {
  return new PostgreSQLPreparedStatement(_conn, query);
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
