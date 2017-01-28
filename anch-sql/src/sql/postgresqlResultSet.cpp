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
#ifdef ANCH_SQL_POSTGRESQL

#include "sql/postgresqlResultSet.hpp"

using anch::sql::PostgreSQLResultSet;
using anch::sql::ResultSet;


// Constructors +
PostgreSQLResultSet::PostgreSQLResultSet(PGconn* conn):
  ResultSet(),
  _conn(conn),
  _result(NULL),
  _currentRow(-1),
  _nbRows(-1),
  _nbFields(-1) {
  // Nothing to do
}
// Constructors -

// Destructor +
PostgreSQLResultSet::~PostgreSQLResultSet() {
  if(_result != NULL) {
    PQclear(_result);
  }
  while((_result = PQgetResult(_conn)) != NULL) {
    PQclear(_result);
  }
}
// Destructor -

// Methods +
bool
PostgreSQLResultSet::getValue(std::size_t idx, std::string& out) throw(SqlException) {
  if(static_cast<int>(idx) >= _nbFields) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_nbFields - 1) << "): " << idx;
    throw SqlException(msg.str());
  }
  bool null = true;
  if(!PQgetisnull(_result, _currentRow, static_cast<int>(idx))) {
    null = false;
    out = PQgetvalue(_result, _currentRow, static_cast<int>(idx));
  }
  return null;
}

bool
PostgreSQLResultSet::next() throw(SqlException) {
  bool hasMore = false;
  if(_currentRow + 1 < _nbRows) {
    _currentRow++;
    hasMore = true;

  } else {
    if(_result != NULL) {
      PQclear(_result);
    }
    _result = PQgetResult(_conn);
    hasMore = (_result != NULL);
    if(hasMore) {
      _currentRow = 0;
      _nbRows = PQntuples(_result);
      if(_fields.empty()) {
	_nbFields = PQnfields(_result);
	for(std::size_t i = 0 ; i < static_cast<std::size_t>(_nbFields) ; ++i) {
	  _fields[PQfname(_result, static_cast<int>(i))] = i;
	}
      }
    }
  }
  return hasMore;
}
// Methods -

#endif // ANCH_SQL_POSTGRESQL
