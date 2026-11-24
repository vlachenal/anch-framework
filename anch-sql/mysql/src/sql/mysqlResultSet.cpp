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
#ifdef ANCH_SQL_MYSQL

#include "sql/mysqlResultSet.hpp"
#include "sql/mysqlConnection.hpp"

using anch::sql::MySQLResultSet;
using anch::sql::ResultSet;


MySQLResultSet::MySQLResultSet(MYSQL* conn, MYSQL_RES* result) noexcept:
  ResultSet(),
  _conn(conn),
  _result(result),
  _row() {
  MYSQL_FIELD* field;
  std::vector<std::string> fields;
  std::size_t i = 0;
  while((field = mysql_fetch_field(_result))) {
    _fields[field->name] = i;
    ++i;
  }
}

MySQLResultSet::~MySQLResultSet() noexcept {
  if(_result != NULL) {
    mysql_free_result(_result);
  }
}

bool
MySQLResultSet::getValue(std::size_t idx, std::string& out) {
  if(idx >= _fields.size()) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_fields.size() - 1) << "): " << idx;
    throw SqlException(msg.str(), true);
  }
  bool null = true;
  if(_row[idx] != NULL) {
    null = false;
    out.assign(_row[idx], mysql_fetch_lengths(_result)[idx]); // binary values can contain '\0'
  }
  return null;
}

std::optional<std::string>
MySQLResultSet::getValue(std::size_t idx) {
  if(idx >= _fields.size()) {
    std::ostringstream msg;
    msg << "Index out of range (0.." << (_fields.size() - 1) << "): " << idx;
    throw SqlException(msg.str(), true);
  }
  std::optional<std::string> res;
  if(_row[idx] != NULL) {
    res = std::string(_row[idx], mysql_fetch_lengths(_result)[idx]);
  }
  return res;
}

bool
MySQLResultSet::fetchNext() {
  _row = mysql_fetch_row(_result);
  if(_row == NULL && mysql_errno(_conn) != 0) { // end of rows or error
    throw anch::sql::mysql::error("Error while fetching MySQL row", mysql_errno(_conn), mysql_error(_conn));
  }
  return (_row != NULL);
}

const anch::date::DateFormatter&
MySQLResultSet::getTimeFormatter() {
  return getDefaultTimeFormatter();
}

const anch::date::DateFormatter&
MySQLResultSet::getTimestampFormatter() {
  return getDefaultTimestampFormatter();
}

#endif // ANCH_SQL_MYSQL
