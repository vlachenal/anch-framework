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

#include "sql/sqlSharedLibraries.hpp"
#include "conf/configuration.hpp"
#include "convert.hpp"
#include "singleton.hpp"

#include <sstream>
#include <dlfcn.h>

using anch::sql::Connection;
using anch::sql::SqlException;
using anch::sql::SqlConnectionFactory;
using anch::sql::SqlConnectionConfiguration;
using anch::sql::SqlConnectionPool;
using anch::sql::SQLSharedLibraries;

using anch::conf::Configuration;
using anch::conf::Section;

const std::string SQL_SECTION("anch::sql");


std::shared_ptr<Connection>
anch::sql::make_shared_connection(const SqlConnectionConfiguration& config) {
  return SQLSharedLibraries::getInstance().makeSharedConnection(config);
}

// Constructors +
SqlConnectionFactory::SqlConnectionFactory(): _configs(), _pools() {
  const Section* conf = Configuration::inst().section(SQL_SECTION);
  for(auto iter = conf->getSections().begin() ; iter != conf->getSections().end() ; ++iter) {
    const Section& connection = iter->second;
    std::optional<std::string> optdriver = connection.getValue<std::string>("driver");
    if(!optdriver.has_value()) {
      continue;
    }
    // Try to register known database engine if found +
    SqlConnectionConfiguration conConf;
    conConf.driver = optdriver.value();
    if(conConf.driver == "PostgreSQL") {
      SQLSharedLibraries::registerPostgreSQL();
    } else if(conConf.driver == "MySQL") {
      SQLSharedLibraries::registerMySQL();
    } else if(conConf.driver == "SQLite3") {
      SQLSharedLibraries::registerSQLite();
    }
    // Try to register known database engine if found -
    auto db = connection.getValue<std::string>("database");
    if(db.has_value()) {
      conConf.database = db.value();
    }
    auto host = connection.getValue<std::string>("host");
    if(host.has_value()) {
      conConf.hostname = host.value();
    }
    auto user = connection.getValue<std::string>("user");
    if(user.has_value()) {
      conConf.user = user.value();
    }
    auto password = connection.getValue<std::string>("password");
    if(password.has_value()) {
      conConf.password = password.value();
    }
    auto application = connection.getValue<std::string>("application");
    if(application.has_value()) {
      conConf.application = application.value();
    }
    auto port = connection.getValue<int>("port");
    if(port.has_value()) {
      conConf.port = port.value();
    }
    _configs[iter->first] = conConf;
    // Connection configuration -

    // Pool configuration +
    const anch::conf::Section* pool = NULL;
    auto iterPool = connection.getSections().find("pool");
    if(iterPool == connection.getSections().end()) {
      continue;
    }
    pool = &iterPool->second;
    std::size_t maxSize = pool->getValue<std::size_t>("max-size", 0);
    std::size_t initSize = pool->getValue<std::size_t>("init-size", 0);
    std::size_t timeout = pool->getValue<std::size_t>("timeout", 100);
    _pools[iter->first] = new SqlConnectionPool(conConf, maxSize, initSize, std::chrono::milliseconds(timeout));
    // Pool configuration -
  }
}
// Constructors -

// Destructor +
SqlConnectionFactory::~SqlConnectionFactory() noexcept {
  _configs.clear();
  for(auto iter = _pools.begin() ; iter != _pools.end() ; ++iter) {
    delete iter->second;
  }
  _pools.clear();
}
// Destructor -

// Methods +
Connection*
SqlConnectionFactory::createConnection(const std::string& name) {
  auto iter = _configs.find(name);
  if(iter == _configs.end()) {
    std::ostringstream out;
    out << "Configuration " << name << " does not exist." << std::endl;
    throw SqlException(out.str());
  }
  return SQLSharedLibraries::getInstance().makeConnection(iter->second);
}

SqlConnectionPool&
SqlConnectionFactory::getPool(const std::string& name) {
  auto iter = _pools.find(name);
  if(iter == _pools.end()) {
  std::ostringstream out;
    out << "Pool " << name << " does not exist." << std::endl;
    throw SqlException(out.str());
  }
  return *(iter->second);
}
// Methods -
