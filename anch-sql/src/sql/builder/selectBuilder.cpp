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
#include "sql/builder/selectBuilder.hpp"

using anch::sql::SelectBuilder;
using anch::sql::FieldsBuilder;
using anch::sql::ClausesBuilder;
using anch::sql::SQLQuery;


// Constructors +
SelectBuilder::SelectBuilder(): _buffer(), _values() {
  _buffer << "SELECT ";
}
// Constructors -

// Destructors +
SelectBuilder::~SelectBuilder() {
  // Nothing to do
}
// Destructors -

// SQL commands +
SelectBuilder&
SelectBuilder::distinct() {
  _buffer << "DISTINCT ";
  return *this;
}

FieldsBuilder
SelectBuilder::field(const std::string& column) {
  return FieldsBuilder(*this, column);
}

FieldsBuilder
SelectBuilder::groupBy(const std::string& column) {
  _buffer << " GROUP BY ";
  return FieldsBuilder(*this, column);
}

FieldsBuilder
SelectBuilder::orderBy(const std::string& column) {
  _buffer << " ORDER BY ";
  return FieldsBuilder(*this, column);
}

SelectBuilder&
SelectBuilder::having(ClausesBuilder& clauses) {
  if(!clauses._firstClause) {
    _buffer << " HAVING " << clauses._buffer.str();
    for(const std::string& value : clauses._values) {
      _values.push_back(value);
    }
  }
  return *this;
}

SelectBuilder&
SelectBuilder::unionOnce(SelectBuilder& other) {
  _buffer << " UNION " << other;
  return *this;
}

SelectBuilder&
SelectBuilder::unionAll(SelectBuilder& other) {
  _buffer << " UNION ALL " << other;
  return *this;
}

SelectBuilder&
SelectBuilder::offset(int64_t offset) {
  _buffer << " OFFSET " << offset << " ROWS";
  return *this;
}

SelectBuilder&
SelectBuilder::fetch(uint64_t limit) {
  _buffer << " FETCH FIRST " << limit << " ROWS ONLY";
  return *this;
}

SelectBuilder&
SelectBuilder::windowByRowNumber(const std::string& alias,
				 const std::string& rowColumn,
				 const std::string& order,
				 uint32_t min,
				 uint32_t max) {
  window("row_number()", alias, rowColumn, order, min, max);
  return *this;
}

SelectBuilder&
SelectBuilder::windowByRank(const std::string& alias,
			    const std::string& rowColumn,
			    const std::string& order,
			    uint32_t min,
			    uint32_t max) {
  window("rank()", alias, rowColumn, order, min, max);
  return *this;
}
// SQL commands -

// Methods +
SQLQuery
SelectBuilder::build() {
  return SQLQuery(_buffer.str(), _values);
}

void
SelectBuilder::window(const std::string& function,
		      const std::string& alias,
		      const std::string& rowColumn,
		      const std::string& order,
		      uint32_t min,
		      uint32_t max) {
  std::ostringstream colBuffer;
  colBuffer << ',' << function << " OVER(ORDER BY " << order << ") AS " << rowColumn;
  std::string query = _buffer.str();
  std::size_t idx = query.find_first_of(" FROM");
  query.insert(idx, colBuffer.str());
  query.insert(0, "SELECT * FROM (");
  _buffer.str(query);
  _buffer << ") AS " << alias << " WHERE";
  if(min > 0) {
    _buffer << ' ' << rowColumn << " >= ?";
    _values.push_back(std::to_string(min));
  }
  if(max > 1) {
    if(min > 0) {
      _buffer << " AND ";
    } else {
      _buffer << ' ';
    }
    _buffer << rowColumn << " < ?";
    _values.push_back(std::to_string(max));
  }
}
// Methods -


namespace anch {
  namespace sql {

    std::ostream&
    operator<<(std::ostream& stream, const SelectBuilder& builder) {
      stream << builder._buffer.str();
      return stream;
    }

  }  // sql
}  // anch
