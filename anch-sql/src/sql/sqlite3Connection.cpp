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

#include "sql/sqlite3Connection.hpp"
#include "sql/sqlite3ResultSet.hpp"

#include <sstream>

using anch::sql::Connection;
using anch::sql::SQLite3Connection;
using anch::sql::ResultSet;
using anch::sql::SQLite3ResultSet;
using anch::sql::SqlException;


// Constructors +
SQLite3Connection::SQLite3Connection(const std::string& database)
  throw(SqlException): Connection(), _conn() {
  std::string connStr = "file:";
  connStr += database;
  int res = sqlite3_open_v2(connStr.data(), &_conn, SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE, NULL);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Fail to connect SQLite3 database " << database << ": " << sqlite3_errmsg(_conn);
    sqlite3_close(_conn);
    throw SqlException(msg.str());
  }
}

SQLite3Connection::SQLite3Connection(const SqlConnectionConfiguration& config)
  throw(SqlException): Connection(), _conn() {
  std::string connStr = "file:";
  connStr += config.database;
  int res = sqlite3_open_v2(connStr.data(), &_conn, SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE, NULL);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Fail to connect SQLite3 database " << config.database << ": " << sqlite3_errmsg(_conn);
    sqlite3_close(_conn);
    throw SqlException(msg.str());
  }
}
// Constructors -

// Destructor +
SQLite3Connection::~SQLite3Connection() {
  sqlite3_close(_conn);
}
// Destructor -

// Methods +
ResultSet*
SQLite3Connection::executeQuery(const std::string& query) throw(SqlException) {
  sqlite3_stmt* stmt;
  int res = sqlite3_prepare_v2(_conn, query.data(), -1, &stmt, NULL);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Error while preparing statement " << query << ": " << sqlite3_errmsg(_conn);
    throw SqlException(msg.str());
  }
  ResultSet* resSet = new SQLite3ResultSet(stmt);
  return resSet;
}

static int emptyCB(void*, int, char**, char**) {
  // Nothing to do
  return 0;
}

uint64_t
SQLite3Connection::executeUpdate(const std::string& query) throw(SqlException) {
  char* errMsg = 0;
  int res = sqlite3_exec(_conn, query.data(), emptyCB, 0, &errMsg);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Error while executing update: " << errMsg;
    sqlite3_free(errMsg);
    throw SqlException(msg.str());
  }
  return static_cast<uint64_t>(sqlite3_changes(_conn));;
}

void
SQLite3Connection::sendCommit() throw(SqlException) {
  char* errMsg = 0;
  int res = sqlite3_exec(_conn, "COMMIT", emptyCB, 0, &errMsg);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Error while commiting transaction: " << errMsg;
    sqlite3_free(errMsg);
    throw SqlException(msg.str());
  }
}

void
SQLite3Connection::sendRollback() throw(SqlException) {
  char* errMsg = 0;
  int res = sqlite3_exec(_conn, "ROLLBACK", emptyCB, 0, &errMsg);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Error while rolling back transaction: " << errMsg;
    sqlite3_free(errMsg);
    throw SqlException(msg.str());
  }
}

void
SQLite3Connection::toggleAutoCommit(bool /*autoCommit*/) throw(SqlException) {
  // Nothing to do .. for now => keep internal state ?
}
// Methods -

#endif // ANCH_SQL_SQLITE3
