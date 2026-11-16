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
#include "sql/connection.hpp"

using anch::sql::Connection;
using anch::sql::ResultSet;
using anch::sql::PreparedStatement;


// Constructors +
Connection::Connection(std::size_t statementCacheSize):
  _valid(true),
  _transaction(false),
  _stmts(),
  _stmtsIndex(),
  _stmtsMax(statementCacheSize) {
  if(statementCacheSize == 0) {
    throw SqlException("Prepared statement cache size has to be at least 1");
  }
}
// Constructors -

// Destructor +
Connection::~Connection() noexcept {
  clearStatements(); // connection can already be closed by driver: statements are not deallocated
}
// Destructor -

// Methods +
void
Connection::commit() {
  if(_transaction) {
    guard([this]() { sendCommit(); });
    _transaction = false;
  }
}

void
Connection::rollback() {
  if(_transaction) {
    guard([this]() { sendRollback(); });
    _transaction = false;
  }
}

void
Connection::startTransaction() {
  if(!_transaction) {
    guard([this]() { sendStartTransaction(); });
    _transaction = true;
  }
}

void
Connection::release() noexcept {
  if(!isValid()) {
    return;
  }
  if(_transaction) { // not ended with commit(): rolled back, even without error (application exception, ...)
    try {
      rollback();
    } catch(...) {
      // Checked below
    }
    if(_transaction) { // unknown transaction state: connection must not be reused
      _valid = false;
    }
  }
}

void
Connection::onError(const SqlException& error) noexcept {
  if(!error.isOK()) {
    _valid = false;
  }
}

bool
Connection::checkConnection() const noexcept {
  return true;
}

ResultSet*
Connection::query(const std::string& query) {
  return guard([this, &query]() {
    ResultSet* res = executeQuery(query);
    res->_connection = this;
    return res;
  });
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
  return guard([this, &query]() { return executeUpdate(query); });
}

PreparedStatement&
Connection::prepareStatement(const std::string& query) {
  auto found = _stmtsIndex.find(std::string_view(query));
  if(found != _stmtsIndex.end()) { // Cached => becomes the most recently used
    _stmts.splice(_stmts.begin(), _stmts, found->second);
    return *found->second->statement;
  }
  // Not found => create, cache and return it +
  PreparedStatement* stmt = guard([this, &query]() { return makePrepared(query); });
  stmt->_connection = this;
  try {
    _stmts.push_front(CachedStatement{query, stmt});
  } catch(...) {
    stmt->deallocate();
    delete stmt;
    throw;
  }
  try {
    _stmtsIndex.emplace(std::string_view(_stmts.front().query), _stmts.begin());
  } catch(...) {
    _stmts.pop_front();
    stmt->deallocate();
    delete stmt;
    throw;
  }
  evictStatements();
  return *stmt;
  // Not found => create, cache and return it -
}

void
Connection::evictStatements() noexcept {
  auto iter = _stmts.end();
  while(_stmts.size() > _stmtsMax && iter != _stmts.begin()) {
    --iter;
    if(iter == _stmts.begin()) { // the most recently used statement is kept
      break;
    }
    if(iter->statement->_openResults > 0) { // statement is used by a result set
      continue;
    }
    PreparedStatement* stmt = iter->statement;
    _stmtsIndex.erase(std::string_view(iter->query)); // key is a view of the entry query: erased first
    iter = _stmts.erase(iter);
    stmt->deallocate();
    delete stmt;
  }
}

void
Connection::clearStatements() noexcept {
  _stmtsIndex.clear();
  for(CachedStatement& entry : _stmts) {
    delete entry.statement;
  }
  _stmts.clear();
}
// Methods -

// Accessors +
std::size_t
Connection::getStatementCacheSize() const noexcept {
  return _stmtsMax;
}

void
Connection::setStatementCacheSize(std::size_t size) {
  if(size == 0) {
    throw SqlException("Prepared statement cache size has to be at least 1");
  }
  _stmtsMax = size;
  evictStatements();
}

std::size_t
Connection::getCachedStatementCount() const noexcept {
  return _stmts.size();
}
// Accessors -
