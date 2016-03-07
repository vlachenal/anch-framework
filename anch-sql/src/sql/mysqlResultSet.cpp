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
#include "sql/mysqlResultSet.hpp"

using anch::sql::MySQLResultSet;
using anch::sql::ResultSet;


MySQLResultSet::MySQLResultSet(MYSQL_RES* result, const std::vector<std::string>& fields, int nbRow):
  ResultSet(fields, nbRow),
  _result(result),
  _row() {
  // Nothing to do
}

MySQLResultSet::~MySQLResultSet() {
  if(_result != NULL) {
    mysql_free_result(_result);
  }
}

bool
MySQLResultSet::getValue(std::size_t idx, std::string& out) {
  bool null = true;
  if(_row[idx] != NULL) {
    null = false;
    out = _row[idx];
  }
  return null;
}

void
MySQLResultSet::fetchNextRow() throw(SqlException) {
  _row = mysql_fetch_row(_result);
  if(_row == NULL) {
    throw SqlException("Can not fetch next row");
  }
}