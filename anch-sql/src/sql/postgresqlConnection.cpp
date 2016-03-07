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

#include <sstream>

using anch::sql::Connection;
using anch::sql::SqlException;
using anch::sql::PostgreSQLConnection;
using anch::sql::ResultSet;
using anch::sql::PostgreSQLResultSet;


// Constructors +
PostgreSQLConnection::PostgreSQLConnection(const std::string& host,
					   const std::string& user,
					   const std::string& password,
					   const std::string& database,
					   int port,
					   const std::string& app)
  throw(SqlException):
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
    std::ostringstream msg;
    msg << "Fail to connect PostgreSQL database " << database
	<< ":" << port << " with user " << user << ": " << PQerrorMessage(_conn);
    PQfinish(_conn);
    throw SqlException(msg.str());
  }
}

PostgreSQLConnection::PostgreSQLConnection(const std::string& connStr) throw(SqlException): _conn() {
  _conn = PQconnectdb(connStr.data());
  if(PQstatus(_conn) != CONNECTION_OK) {
    std::ostringstream msg;
    msg << "Fail to connect PostgreSQL database: " << PQerrorMessage(_conn);
    PQfinish(_conn);
    throw SqlException(msg.str());
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
PostgreSQLConnection::sendCommit() throw(SqlException) {
  PGresult* res = PQexec(_conn, "COMMIT");
  ExecStatusType status = PQresultStatus(res);
  if(status == PGRES_FATAL_ERROR) {
    std::ostringstream msg;
    msg << "Unable to commit transaction: " << PQresultErrorMessage(res);
    PQclear(res);
    throw SqlException(msg.str());
  }
  PQclear(res);
}

void
PostgreSQLConnection::sendRollback() throw(SqlException) {
  PGresult* res = PQexec(_conn, "ROLLBACK");
  ExecStatusType status = PQresultStatus(res);
  if(status == PGRES_FATAL_ERROR) {
    std::ostringstream msg;
    msg << "Unable to rollback transaction: " << PQresultErrorMessage(res);
    PQclear(res);
    throw SqlException(msg.str());
  }
  PQclear(res);
}

void
PostgreSQLConnection::toggleAutoCommit(bool) throw(SqlException) {
  // Nothing to do .. for now => keep internal state ?
}

ResultSet*
PostgreSQLConnection::query(const std::string& query) throw(SqlException) {
  PGresult* res = PQexec(_conn, query.data());
  ExecStatusType status = PQresultStatus(res);
  if(status == PGRES_FATAL_ERROR) {
    std::ostringstream msg;
    msg << "Unable to execute query " << query << ": " << PQresultErrorMessage(res);
    PQclear(res);
    throw SqlException(msg.str());
  }
  int nbFields = PQnfields(res);
  PostgreSQLResultSet* resSet = NULL;
  if(nbFields > 0) {
    int nbRows = PQntuples(res);
    std::vector<std::string> fields;
    for(int i = 0 ; i < nbFields ; ++i) {
      fields.push_back(PQfname(res, i));
    }
    resSet = new PostgreSQLResultSet(res, fields, nbRows);
  } else {
    PQclear(res);
  }
  return resSet;
}
// Methods -

#endif //ANCH_SQL_POSTGRESQL
