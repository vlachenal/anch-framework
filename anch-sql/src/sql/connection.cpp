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


// Constructors +
Connection::Connection(): _autoCommit(true), _valid(true) {
}
// Constructors -

// Destructor +
Connection::~Connection() {
  // Nothing to do ... for now
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
// Methods -
