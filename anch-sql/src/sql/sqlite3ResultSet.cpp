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

using anch::sql::SQLite3ResultSet;
using anch::sql::SqlException;

// Constructors +
SQLite3ResultSet::SQLite3ResultSet(sqlite3_stmt* stmt, const std::vector<std::string>& fields, int nbRow):
  ResultSet(fields, nbRow),
  _stmt(stmt) {
}
// Constructors -

// Destructor +
SQLite3ResultSet::~SQLite3ResultSet() {
}
// Destructor -

// Methods +
bool
SQLite3ResultSet::getValue(std::size_t /*idx*/, std::string& /*out*/) {
  return false;
}

void
SQLite3ResultSet::fetchNextRow() throw(SqlException) {

}
// Methods -

#endif // ANCH_SQL_SQLITE3
