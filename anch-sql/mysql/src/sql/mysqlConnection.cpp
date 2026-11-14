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
#ifdef ANCH_SQL_MYSQL

#include "errmsg.h"

#include "sql/mysqlConnection.hpp"
#include "sql/mysqlResultSet.hpp"
#include "sql/mysqlPreparedStatement.hpp"
#include "singleton.hpp"

#include <sstream>
#include <vector>

using anch::sql::Connection;
using anch::sql::MySQLConnection;
using anch::sql::SqlException;
using anch::sql::SqlConnectionConfiguration;
using anch::sql::ResultSet;
using anch::sql::MySQLResultSet;
using anch::sql::PreparedStatement;
using anch::sql::MySQLPreparedStatement;


/*!
 * \brief MySQL (un)initializer
 *
 * Use Meyer singleton to provide lazy (un)intialization.
 *
 * \author Vincent Lachenal
 *
 * \since 0.1
 */
class MySQLInitializer: public anch::Singleton<MySQLInitializer> {
  friend class anch::Singleton<MySQLInitializer>;

private:
  /*!
   * \ref MySQLInitializer private constructor.\n
   * Initialize MySQL library
   *
   * \throw SqlException fail to initialize MySQL library
   */
  MySQLInitializer() {
    if(mysql_library_init(0, NULL, NULL) != 0) {
      throw SqlException("Unable to initialize MySQL library", false);
    }
  }

  /*!
   * \ref MySQLInitializer destructor.\n
   * Uninitialize MySQL library
   */
  ~MySQLInitializer() noexcept {
    mysql_library_end();
  }

};

// Constructors +
MySQLConnection::MySQLConnection(const SqlConnectionConfiguration& config): Connection(config.statementCacheSize) {

  MySQLInitializer::getInstance(); // Initialize MySQL library if not already done

  _mysql = new MYSQL;
  mysql_init(_mysql);
  mysql_options(_mysql, MYSQL_READ_DEFAULT_GROUP, config.application.data());
  MYSQL* res = mysql_real_connect(_mysql,
				  config.hostname.data(),
				  config.user.data(),
				  config.password.data(),
				  config.database.data(),
				  static_cast<unsigned int>(config.port), NULL, 0);
  if(res == NULL) {
    _valid = false;
    std::ostringstream msg;
    msg << "Failed to connect to database. Error: " << mysql_error(_mysql);
    throw SqlException(msg.str(), _valid);
  }
}
// Constructors -

// Destructor +
MySQLConnection::~MySQLConnection() noexcept {
  release();
  mysql_close(_mysql);
  delete _mysql;
}
// Destructor -

// Methods +
bool
anch::sql::mysql::isFatal(unsigned int code) noexcept {
  switch(code) {
  case CR_CONNECTION_ERROR:
  case CR_CONN_HOST_ERROR:
  case CR_SERVER_GONE_ERROR:
  case CR_OUT_OF_MEMORY:
  case CR_SERVER_LOST:
  case CR_COMMANDS_OUT_OF_SYNC:
#ifdef CR_SERVER_LOST_EXTENDED
  case CR_SERVER_LOST_EXTENDED:
#endif
  case 1053: // ER_SERVER_SHUTDOWN
  case 1927: // ER_CONNECTION_KILLED (MariaDB)
  case 4031: // ER_CLIENT_INTERACTION_TIMEOUT (MySQL 8)
    return true;
  default:
    return false;
  }
}

SqlException
anch::sql::mysql::error(const std::string& message, unsigned int code, const char* text) {
  std::ostringstream msg;
  msg << message << ": " << text << " (" << code << ')';
  return SqlException(msg.str(), !isFatal(code));
}

/*!
 * Execute command which does not return rows
 *
 * \param conn the MySQL connection
 * \param command the command
 * \param context the error context
 *
 * \throw SqlException any error
 */
static void
execCommand(MYSQL* conn, const std::string& command, const std::string& context) {
  if(mysql_real_query(conn, command.data(), command.size()) != 0) {
    throw anch::sql::mysql::error(context, mysql_errno(conn), mysql_error(conn));
  }
}

void
MySQLConnection::sendCommit() {
  execCommand(_mysql, "COMMIT", "Unable to commit transaction");
}

void
MySQLConnection::sendRollback() {
  execCommand(_mysql, "ROLLBACK", "Unable to rollback transaction");
}

void
MySQLConnection::sendStartTransaction() {
  execCommand(_mysql, "START TRANSACTION", "Unable to start transaction");
}

ResultSet*
MySQLConnection::executeQuery(const std::string& query) {
  execCommand(_mysql, query, "Error while executing query " + query);
  MYSQL_RES* result = mysql_use_result(_mysql);
  if(result == NULL) {
    if(mysql_errno(_mysql) != 0) {
      throw anch::sql::mysql::error("Error while retrieving result of " + query, mysql_errno(_mysql), mysql_error(_mysql));
    }
    throw SqlException("Query " + query + " does not return a result set", true);
  }
  return new MySQLResultSet(_mysql, result);
}

uint64_t
MySQLConnection::executeUpdate(const std::string& query) {
  execCommand(_mysql, query, "Error while executing query " + query);
  my_ulonglong nbRow = mysql_affected_rows(_mysql);
  if(nbRow == static_cast<my_ulonglong>(-1)) {
    MYSQL_RES* result = mysql_store_result(_mysql); // pending rows have to be read before next command
    if(result != NULL) {
      mysql_free_result(result);
    }
    throw SqlException("Query " + query + " is not an update query", true);
  }
  return static_cast<uint64_t>(nbRow);
}

PreparedStatement*
MySQLConnection::makePrepared(const std::string& query) {
  return new MySQLPreparedStatement(_mysql, query);
}
// Methods -

// C shared library definition +
/*!
 * C shared library connection maker
 *
 * \return the MySQL connection
 */
extern "C"
std::shared_ptr<Connection>
create_shared_connection(const SqlConnectionConfiguration& config) {
  return std::make_shared<MySQLConnection>(config);
}

/*!
 * C shared library connection maker
 *
 * \return the MySQL connection
 */
extern "C"
Connection*
create_connection(const SqlConnectionConfiguration& config) {
  return new MySQLConnection(config);
}

/*!
 * C shared library MySQL connection free
 *
 * \param conn the connection to free
 */
extern "C"
void
delete_connection(MySQLConnection* conn) {
  delete conn;
}
// C shared library definition -

#endif // ANCH_SQL_MYSQL
