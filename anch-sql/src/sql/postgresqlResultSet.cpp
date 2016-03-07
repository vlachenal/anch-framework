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
PostgreSQLResultSet::PostgreSQLResultSet(PGresult* result,
					 const std::vector<std::string>& fields,
					 int nbRow):
  ResultSet(fields, nbRow),
  _result(result) {
  // Nothing to do
}
// Constructors -

// Destructor +
PostgreSQLResultSet::~PostgreSQLResultSet() {
  if(_result != NULL) {
    PQclear(_result);
  }
}
// Destructor -

// Methods +
bool
PostgreSQLResultSet::getValue(std::size_t idx, std::string& out) {
  bool null = true;
  if(!PQgetisnull(_result, _currentRow, idx)) {
    null = false;
    out = PQgetvalue(_result, _currentRow, idx);
  }
  return null;
}

/*!
 * Fetch next row in SQL result set.
 *
 * \throw SqlException any error
 */
void
PostgreSQLResultSet::fetchNextRow() throw(SqlException) {
  // Nothing to do
}
// Methods -

#endif // ANCH_SQL_POSTGRESQL
