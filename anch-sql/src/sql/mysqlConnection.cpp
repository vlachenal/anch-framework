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
#ifdef ANCH_SQL_MYSQL

#include "mysql.h"
#include "errmsg.h"

#include "sql/mysqlConnection.hpp"
#include "sql/mysqlResultSet.hpp"
#include "sql/mysqlPreparedStatement.hpp"
#include "singleton.hpp"

#include <sstream>
#include <vector>

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
      throw SqlException("Unable to initialize MySQL library");
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
MySQLConnection::MySQLConnection(const std::string& host,
				 const std::string& user,
				 const std::string& password,
				 const std::string& database,
				 int port,
				 const std::string& app) {

  MySQLInitializer::getInstance(); // Initialize MySQL library if not already done

  _mysql = new MYSQL;
  mysql_init(_mysql);
  mysql_options(_mysql, MYSQL_READ_DEFAULT_GROUP, app.data());
  MYSQL* res = mysql_real_connect(_mysql, host.data(), user.data(), password.data(), database.data(), static_cast<unsigned int>(port), NULL, 0);
  if(res == NULL) {
    /*
      \todo manage error with mysql_errno(_mysql):
      - CR_CONN_HOST_ERROR: fail to connect server
      - CR_CONNECTION_ERROR: fail to connect localhost server
      - CR_IPSOCK_ERROR: fail to create IP socket
      - CR_OUT_OF_MEMORY: out of memory
      - CR_SOCKET_CREATE_ERROR: fail to create UNIX socket
      - CR_UNKNOWN_HOST: unknown host
      - CR_VERSION_ERROR: protocol versions mismatch between client and server
      - CR_NAMEDPIPEOPEN_ERROR: fail to create named pipe on Windows
      - CR_NAMEDPIPEWAIT_ERROR: fail to wait for a named pipe on Windows
      - CR_NAMEDPIPESETSTATE_ERROR: fail to get a pipe handler on Windows
      - CR_SERVER_LOST: connection timeout if set or server died
      - CR_ALREADY_CONNECTED: already connected ... can not happened
    */
    std::ostringstream msg;
    msg << "Failed to connect to database. Error: " << mysql_error(_mysql);
    throw SqlException(msg.str());
  }
}

MySQLConnection::MySQLConnection(const SqlConnectionConfiguration& config) {

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
    /*
      \todo manage error with mysql_errno(_mysql):
      - CR_CONN_HOST_ERROR: fail to connect server
      - CR_CONNECTION_ERROR: fail to connect localhost server
      - CR_IPSOCK_ERROR: fail to create IP socket
      - CR_OUT_OF_MEMORY: out of memory
      - CR_SOCKET_CREATE_ERROR: fail to create UNIX socket
      - CR_UNKNOWN_HOST: unknown host
      - CR_VERSION_ERROR: protocol versions mismatch between client and server
      - CR_NAMEDPIPEOPEN_ERROR: fail to create named pipe on Windows
      - CR_NAMEDPIPEWAIT_ERROR: fail to wait for a named pipe on Windows
      - CR_NAMEDPIPESETSTATE_ERROR: fail to get a pipe handler on Windows
      - CR_SERVER_LOST: connection timeout if set or server died
      - CR_ALREADY_CONNECTED: already connected ... can not happened
    */
    std::ostringstream msg;
    msg << "Failed to connect to database. Error: " << mysql_error(_mysql);
    throw SqlException(msg.str());
  }
}
// Constructors -

// Destructor +
MySQLConnection::~MySQLConnection() noexcept {
  mysql_close(_mysql);
  delete _mysql;
}
// Destructor -

// Methods +
void
MySQLConnection::sendCommit() {
  int res = mysql_query(_mysql, "COMMIT");
  if(res != 0) {
    std::ostringstream out;
    out << "Unable to commit transaction ; message=" << mysql_error(_mysql);
    throw SqlException(out.str());
  }
}

void
MySQLConnection::sendRollback() {
  int res = mysql_query(_mysql, "ROLLBACK");
  if(res != 0) {
    std::ostringstream out;
    out << "Unable to rollback transaction ; message=" << mysql_error(_mysql);
    throw SqlException(out.str());
  }
}

void
MySQLConnection::sendStartTransaction() {
  int res = mysql_query(_mysql, "START TRANSACTION");
  if(res != 0) {
    std::ostringstream out;
    out << "Unable to start transaction ; message=" << mysql_error(_mysql);
    throw SqlException(out.str());
  }
}

ResultSet*
MySQLConnection::executeQuery(const std::string& query) {
  ResultSet* resSet = NULL;
  int res = mysql_query(_mysql, query.data());
  if(res != 0) {
    std::ostringstream out;
    out << "Error while executing query " << query << " ; message="
	<< mysql_error(_mysql);
    throw SqlException(out.str());
  }
  MYSQL_RES* result = mysql_use_result(_mysql);
  if(result == NULL) {
    std::ostringstream out;
    out << "Error while retrieving result " << query << " ; message="
	<< mysql_error(_mysql);
    throw SqlException(out.str());

  } else {
    resSet = new MySQLResultSet(result);
  }
  return resSet;
}

uint64_t
MySQLConnection::executeUpdate(const std::string& query) {
  int res = mysql_query(_mysql, query.data());
  if(res != 0) {
    std::ostringstream out;
    out << "Error while executing query " << query << " ; message="
	<< mysql_error(_mysql);
    throw SqlException(out.str());
  }
  my_ulonglong nbRow = mysql_affected_rows(_mysql);
  if(nbRow == static_cast<my_ulonglong>(-1)) {
    std::ostringstream out;
    out << "Query " << query << " is not an update query " << query
	<< " ; message=" << mysql_error(_mysql);
    throw SqlException(out.str());
  }
  return static_cast<uint64_t>(nbRow);
}

PreparedStatement*
MySQLConnection::makePrepared(const std::string& query) {
  return new MySQLPreparedStatement(_mysql, query);
}
// Methods -

#endif // ANCH_SQL_MYSQL
