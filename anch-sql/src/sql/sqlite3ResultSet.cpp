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
#ifdef ANCH_SQL_SQLITE3

#include "sql/sqlite3ResultSet.hpp"

#include "sqlite3.h"

using anch::sql::SQLite3ResultSet;
using anch::sql::SqlException;


// Constructors +
SQLite3ResultSet::SQLite3ResultSet(sqlite3_stmt* stmt, bool prepared):
  ResultSet(),
  _stmt(stmt),
  _prepared(prepared),
  _nbFields(-1) {
  // Nothing to do
}
// Constructors -

// Destructor +
SQLite3ResultSet::~SQLite3ResultSet() {
  if(_stmt != NULL && !_prepared) { // If prepared keep statement
    sqlite3_finalize(_stmt); // Do not check result because we do not care about it
  }
}
// Destructor -

// Methods +
bool
SQLite3ResultSet::getValue(std::size_t idx, std::string& out) throw(SqlException) {
  if(static_cast<int>(idx) >= _nbFields) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_nbFields - 1) << "): " << idx;
    throw SqlException(msg.str());
  }
  bool null = true;
  const unsigned char* data = sqlite3_column_text(_stmt, static_cast<int>(idx));
  if(data != NULL) {
    out = std::string((char*)data);
    null = false;
  }
  return null;
}

bool
SQLite3ResultSet::next() throw(SqlException) {
  int res = sqlite3_step(_stmt);
  bool hasMore = (res == SQLITE_ROW);
  if(hasMore && _fields.empty()) {
    _nbFields = sqlite3_column_count(_stmt);
    for(std::size_t i = 0 ; i < static_cast<std::size_t>(_nbFields) ; ++i) {
      _fields[sqlite3_column_name(_stmt, static_cast<int>(i))] = i;
    }
  }
  return hasMore;
}
// Methods -

#endif // ANCH_SQL_SQLITE3
