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
#include "sql/connection.hpp"

using anch::sql::Connection;
using anch::sql::ResultSet;
using anch::sql::PreparedStatement;


// Constructors +
Connection::Connection() throw(SqlException): _autoCommit(true), _valid(true), _stmts() {
  // Nothing to do
}
// Constructors -

// Destructor +
Connection::~Connection() {
  // Delete prepared statements +
  for(auto iter = _stmts.begin() ; iter != _stmts.end() ; ++iter) {
    delete iter->second;
  }
  // Delete prepared statements -
}
// Destructor -

// Methods +
void
Connection::commit() throw(SqlException) {
  if(!_autoCommit) {
    sendCommit();
  }
}

void
Connection::rollback() throw(SqlException) {
  if(!_autoCommit) {
    sendRollback();
  }
}

void
Connection::setAutoCommit(bool autoCommit) throw(SqlException) {
  if(autoCommit != _autoCommit) {
    toggleAutoCommit(autoCommit);
    _autoCommit = autoCommit;
  }
}

ResultSet*
Connection::query(const std::string& query) throw(SqlException) {
  // \todo parse select query
  return executeQuery(query);
}

void
Connection::queryMapRow(const std::string& sqlQuery, std::function<void(ResultSet&)> rowMapper) throw(SqlException) {
  ResultSet* res = query(sqlQuery);
  try {
    while(res->next()) {
      rowMapper(*res);
    }
  } catch(...) {
    delete res;
    throw;
  }
  delete res;
}

void
Connection::queryExtract(const std::string& sqlQuery, std::function<void(ResultSet&)> resExtractor) throw(SqlException) {
  ResultSet* res = query(sqlQuery);
  try {
    resExtractor(*res);
  } catch(...) {
    delete res;
    throw;
  }
  delete res;
}

uint64_t
Connection::update(const std::string& query) throw(SqlException) {
  // \todo check update query ...
  return executeUpdate(query);
}

PreparedStatement&
Connection::prepareStatement(const std::string& query) throw(SqlException) {
  auto iter = _stmts.find(query);
  if(iter == _stmts.end()) { // Not found => create and return it
    PreparedStatement* stmt = makePrepared(query);
    _stmts[query] = stmt;
    return *stmt;
  } else {
    return *iter->second;
  }
}
// Methods -
