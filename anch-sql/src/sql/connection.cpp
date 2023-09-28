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
Connection::Connection(): _valid(true), _transaction(false), _stmts(), _errors(false) {
  // Nothing to do
}
// Constructors -

// Destructor +
Connection::~Connection() noexcept {
  // Delete prepared statements +
  for(auto iter = _stmts.begin() ; iter != _stmts.end() ; ++iter) {
    delete iter->second;
  }
  // Delete prepared statements -
}
// Destructor -

// Methods +
void
Connection::commit() {
  if(_transaction) {
    sendCommit();
    _transaction = false;
  }
}

void
Connection::rollback() {
  if(_transaction) {
    sendRollback();
    _transaction = false;
  }
}

void
Connection::startTransaction() {
  if(!_transaction) {
    sendStartTransaction();
    _transaction = true;
  }
}

void
Connection::release() noexcept {
  if(!_valid) {
    return;
  }
  try {
    if(_errors) {
      rollback();
    } else {
      commit();
    }
  } catch(...) {
    // nothing to do
  }
  _errors = false;
}

ResultSet*
Connection::query(const std::string& query) {
  return executeQuery(query);
}

void
Connection::mapRow(ResultSet* res, std::function<void(ResultSet&)> rowMapper) {
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
Connection::queryMapRow(const std::string& sqlQuery, std::function<void(ResultSet&)> rowMapper) {
  mapRow(query(sqlQuery), rowMapper);
}

void
Connection::extract(ResultSet* res, std::function<void(ResultSet&)> resExtractor) {
  try {
    resExtractor(*res);
  } catch(...) {
    delete res;
    throw;
  }
  delete res;
}

void
Connection::queryExtract(const std::string& sqlQuery, std::function<void(ResultSet&)> resExtractor) {
  extract(query(sqlQuery), resExtractor);
}

uint64_t
Connection::update(const std::string& query) {
  return executeUpdate(query);
}

PreparedStatement&
Connection::prepareStatement(const std::string& query) {
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
