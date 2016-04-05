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
#include "sql/sqlConnectionFactory.hpp"

#include "resource/resource.hpp"
#include "convert.hpp"

#include <sstream>

using anch::sql::Connection;
using anch::sql::SqlException;
using anch::sql::SqlConnectionFactory;
using anch::sql::SqlConnectionConfiguration;
using anch::sql::SqlConnectionPool;

using anch::resource::Resource;
using anch::resource::Section;

#ifdef ANCH_SQL_MYSQL
using anch::sql::MySQLConnection;
#endif // ANCH_SQL_MYSQL

#ifdef ANCH_SQL_POSTGRESQL
using anch::sql::PostgreSQLConnection;
#endif // ANCH_SQL_POSTGRESQL

#ifdef ANCH_SQL_SQLITE3
using anch::sql::SQLite3Connection;
#endif // ANCH_SQL_SQLITE3

// Constructors +
SqlConnectionFactory::SqlConnectionFactory(): _configs(), _pools() {
  const Resource& resource = Resource::getResource("db_con.conf");
  auto conf = resource.getConfiguration();
  for(auto iter = conf.cbegin() ; iter != conf.cend() ; ++iter) {
    const std::string name = iter->first;
    const Section& conf = iter->second;
    // Connection configuration +
    SqlConnectionConfiguration conConf;
    conConf.driver = conf.getParameter("driver");
    if(conConf.driver.empty()) {
      continue;
    }
    conConf.database = conf.getParameter("database");
    conConf.hostname = conf.getParameter("host");
    conConf.user = conf.getParameter("user");
    conConf.password = conf.getParameter("password");
    conConf.application = conf.getParameter("application");
    std::string intStr = conf.getParameter("port");
    if(!intStr.empty()) {
      try {
	conConf.port = convert<int>(intStr);
      } catch(const std::bad_cast& e) {
	// continue ...
      }
    }
    _configs[name] = conConf;
    // Connection configuration -

    // Pool configuration +
    intStr = conf.getParameter("pool_maxsize");
    if(intStr.empty()) {
      continue;
    }
    std::size_t maxSize = 0;
    try {
      maxSize = convert<std::size_t>(intStr);
    } catch(const std::bad_cast& e) {
      continue;
    }
    std::size_t initSize = 0;
    intStr = conf.getParameter("pool_initsize");
    if(!intStr.empty()) {
      try {
	initSize = convert<std::size_t>(intStr);
      } catch(const std::bad_cast& e) {
	initSize = 0;
      }
    }
    std::size_t timeout = 100;
    intStr = conf.getParameter("pool_timeout");
    if(!intStr.empty()) {
      try {
	timeout = convert<std::size_t>(intStr);
      } catch(const std::bad_cast& e) {
	timeout = 100;
      }
    }
    _pools[name] = new SqlConnectionPool(conConf, maxSize, initSize, std::chrono::milliseconds(timeout));
    // Pool configuration -
  }
}
// Constructors -

// Destructor +
SqlConnectionFactory::~SqlConnectionFactory() {
  _configs.clear();
  for(auto iter = _pools.begin() ; iter != _pools.end() ; ++iter) {
    delete iter->second;
  }
  _pools.clear();
}
// Destructor -

// Methods +
Connection*
SqlConnectionFactory::createConnection(const std::string& name) throw(SqlException) {
  auto iter = _configs.find(name);
  if(iter == _configs.end()) {
    std::ostringstream out;
    out << "Configuration " << name << " does not exist." << std::endl;
    throw SqlException(out.str());
  }
  const SqlConnectionConfiguration& config = iter->second;
  Connection* conn = NULL;
#ifdef ANCH_SQL_MYSQL
  if(config.driver == "MySQL") {
    conn = new MySQLConnection(config);
  }
#endif // ANCH_SQL_MYSQL
#ifdef ANCH_SQL_POSTGRESQL
  if(config.driver == "PostgreSQL") {
    conn = new PostgreSQLConnection(config);
  }
#endif // ANCH_SQL_POSTGRESQL
#ifdef ANCH_SQL_SQLITE3
  if(config.driver == "SQLite3") {
    conn = new SQLite3Connection(config);
  }
#endif // ANCH_SQL_SQLITE3
  if(conn == NULL) {
    std::ostringstream out;
    out << "Can not create connection from configuration " << name << ": driver "
	<< config.driver << " is not taken into account." << std::endl;
    throw SqlException(out.str());
  }
  return conn;
}

SqlConnectionPool&
SqlConnectionFactory::getPool(const std::string& name) throw(SqlException) {
  auto iter = _pools.find(name);
  if(iter == _pools.end()) {
  std::ostringstream out;
    out << "Pool " << name << " does not exist." << std::endl;
    throw SqlException(out.str());
  }
  return *(iter->second);
}
// Methods -
