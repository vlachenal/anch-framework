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
#include "sql/sqlite3PreparedStatement.hpp"

#include <sstream>

using anch::sql::Connection;
using anch::sql::SQLite3Connection;
using anch::sql::ResultSet;
using anch::sql::SQLite3ResultSet;
using anch::sql::SQLite3PreparedStatement;
using anch::sql::SqlException;
using anch::sql::PreparedStatement;
using anch::sql::SqlConnectionConfiguration;


// Constructors +
SQLite3Connection::SQLite3Connection(const std::string& database): Connection(), _conn() {
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

SQLite3Connection::SQLite3Connection(const SqlConnectionConfiguration& config): Connection(), _conn() {
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
  release();
  for(auto iter = _stmts.begin() ; iter != _stmts.end() ; ++iter) {
    delete iter->second;
  }
  _stmts.clear();
  sqlite3_close(_conn);
}
// Destructor -

// Methods +
ResultSet*
SQLite3Connection::executeQuery(const std::string& query) {
  sqlite3_stmt* stmt = NULL;
  int res = sqlite3_prepare_v2(_conn, query.data(), -1, &stmt, NULL);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Error while preparing statement " << query << ": " << sqlite3_errmsg(_conn);
    throw SqlException(msg.str());
  }
  return new SQLite3ResultSet(stmt);
}

static int emptyCB(void*, int, char**, char**) noexcept {
  // Nothing to do
  return 0;
}

uint64_t
SQLite3Connection::executeUpdate(const std::string& query) {
  char* errMsg = NULL;
  int res = sqlite3_exec(_conn, query.data(), emptyCB, 0, &errMsg);
  if(res != SQLITE_OK) {
    _errors = true;
    std::ostringstream msg;
    msg << "Error while executing update: " << errMsg;
    sqlite3_free(errMsg);
    throw SqlException(msg.str());
  }
  return static_cast<uint64_t>(sqlite3_changes(_conn));
}

void
SQLite3Connection::sendCommit() {
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
SQLite3Connection::sendRollback() {
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
SQLite3Connection::sendStartTransaction() {
  char* errMsg = 0;
  int res = sqlite3_exec(_conn, "BEGIN", emptyCB, 0, &errMsg);
  if(res != SQLITE_OK) {
    std::ostringstream msg;
    msg << "Error while starting transaction: " << errMsg;
    sqlite3_free(errMsg);
    throw SqlException(msg.str());
  }
}

PreparedStatement*
SQLite3Connection::makePrepared(const std::string& query) {
  return new SQLite3PreparedStatement(_conn, query);
}
// Methods -

// C shared library definition +
/*!
 * C shared library connection maker
 *
 * \return the SQLite3 connection
 */
extern "C"
std::shared_ptr<Connection>
create_shared_connection(const SqlConnectionConfiguration& config) {
  return std::make_shared<SQLite3Connection>(config);
}

/*!
 * C shared library connection maker
 *
 * \return the SQLite3 connection
 */
extern "C"
Connection*
create_connection(const SqlConnectionConfiguration& config) {
  return new SQLite3Connection(config);
}

/*!
 * C shared library SQLite3 connection free
 *
 * \param conn the connection to free
 */
extern "C"
void
delete_connection(SQLite3Connection* conn) {
  delete conn;
}
// C shared library definition -

#endif // ANCH_SQL_SQLITE3
