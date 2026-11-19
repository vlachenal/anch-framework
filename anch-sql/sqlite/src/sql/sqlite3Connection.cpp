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

SQLite3Connection::SQLite3Connection(const SqlConnectionConfiguration& config): Connection(config.statementCacheSize), _conn() {
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
SQLite3Connection::~SQLite3Connection() noexcept {
  release();
  clearStatements(); // statements have to be finalized before closing connection
  sqlite3_close(_conn);
}
// Destructor -

// Methods +
bool
anch::sql::sqlite::isFatal(int code) noexcept {
  switch(code & 0xff) { // primary result code
  case SQLITE_IOERR:
  case SQLITE_CORRUPT:
  case SQLITE_NOTADB:
  case SQLITE_CANTOPEN:
  case SQLITE_NOMEM:
    return true;
  default:
    return false;
  }
}

SqlException
anch::sql::sqlite::error(const std::string& message, sqlite3* conn, int code) {
  std::ostringstream msg;
  msg << message << ": " << (conn != NULL ? sqlite3_errmsg(conn) : sqlite3_errstr(code)) << " (" << code << ')';
  return SqlException(msg.str(), !isFatal(code));
}

ResultSet*
SQLite3Connection::executeQuery(const std::string& query) {
  sqlite3_stmt* stmt = NULL;
  int res = sqlite3_prepare_v2(_conn, query.data(), static_cast<int>(query.size()), &stmt, NULL);
  if(res != SQLITE_OK) {
    throw sqlite::error("Error while preparing statement " + query, _conn, res);
  }
  return new SQLite3ResultSet(stmt);
}

/*!
 * Execute SQL instruction
 *
 * \param conn the SQLite connection
 * \param sql the SQL instruction
 * \param context the error context
 *
 * \throw SqlException any error
 */
static void
exec(sqlite3* conn, const std::string& sql, const std::string& context) {
  char* errMsg = NULL;
  int res = sqlite3_exec(conn, sql.data(), NULL, NULL, &errMsg);
  if(res != SQLITE_OK) {
    std::string msg = context + ": " + (errMsg != NULL ? errMsg : sqlite3_errstr(res)) + " (" + std::to_string(res) + ')';
    sqlite3_free(errMsg);
    throw SqlException(msg, !anch::sql::sqlite::isFatal(res));
  }
}

uint64_t
SQLite3Connection::executeUpdate(const std::string& query) {
  exec(_conn, query, "Error while executing update " + query);
  return static_cast<uint64_t>(sqlite3_changes64(_conn));
}

void
SQLite3Connection::sendCommit() {
  exec(_conn, "COMMIT", "Error while commiting transaction");
}

void
SQLite3Connection::sendRollback() {
  exec(_conn, "ROLLBACK", "Error while rolling back transaction");
}

void
SQLite3Connection::sendStartTransaction() {
  exec(_conn, "BEGIN", "Error while starting transaction");
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
// C linkage only gives an unmangled name: the function is loaded with dlsym by the C++ connection factory
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif
extern "C" ANCH_SQLITE_EXPORT
std::shared_ptr<Connection>
create_shared_connection(const SqlConnectionConfiguration& config) {
  return std::make_shared<SQLite3Connection>(config);
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

/*!
 * C shared library connection maker
 *
 * \return the SQLite3 connection
 */
extern "C" ANCH_SQLITE_EXPORT
Connection*
create_connection(const SqlConnectionConfiguration& config) {
  return new SQLite3Connection(config);
}

/*!
 * C shared library SQLite3 connection free
 *
 * \param conn the connection to free
 */
extern "C" ANCH_SQLITE_EXPORT
void
delete_connection(SQLite3Connection* conn) {
  delete conn;
}
// C shared library definition -

#endif // ANCH_SQL_SQLITE3
