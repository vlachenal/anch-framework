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
#include "sql/builder/fromBuilder.hpp"

#include "sql/builder/selectBuilder.hpp"
#include "sql/builder/clausesBuilder.hpp"
#include "sql/builder/sqlQuery.hpp"

using anch::sql::FromBuilder;
using anch::sql::SQLQuery;
using anch::sql::SelectBuilder;
using anch::sql::ClausesBuilder;


// Constructors +
FromBuilder::FromBuilder(anch::sql::SelectBuilder& select, const std::string& table): SelectBuilderPart(select) {
  select._buffer << " FROM " << table;
}

FromBuilder::FromBuilder(anch::sql::SelectBuilder& select, const anch::sql::SelectBuilder& subquery): SelectBuilderPart(select) {
  select._buffer << " FROM (" << subquery << ')';
  for(const std::string& value : subquery._values) {
    select._values.push_back(value);
  }
}

FromBuilder::FromBuilder(anch::sql::SelectBuilder& select, const anch::sql::SQLQuery& subquery): SelectBuilderPart(select) {
  select._buffer << " FROM (" << subquery.getQuery() << ')';
  for(const std::string& value : subquery.getValues()) {
    select._values.push_back(value);
  }
}

FromBuilder::FromBuilder(anch::sql::SelectBuilder& select, const anch::sql::SelectBuilder& subquery, const std::string& alias): SelectBuilderPart(select) {
  select._buffer << " FROM (" << subquery << ") " << alias;
  for(const std::string& value : subquery._values) {
    select._values.push_back(value);
  }
}

FromBuilder::FromBuilder(anch::sql::SelectBuilder& select, const anch::sql::SQLQuery& subquery, const std::string& alias): SelectBuilderPart(select) {
  select._buffer << " FROM (" << subquery.getQuery() << ") " << alias;
  for(const std::string& value : subquery.getValues()) {
    select._values.push_back(value);
  }
}
// Constructors -

// Destructor +
FromBuilder::~FromBuilder() {
  // Nothing to do
}
// Destructor -

// SQL commands +
FromBuilder&
FromBuilder::selfJoin(const std::string& table) {
  _select._buffer << ',' << table;
  return *this;
}

FromBuilder&
FromBuilder::selfJoin(const SQLQuery& subquery, const std::string& alias) {
  addJoin(",", subquery, alias);
  return *this;
}

FromBuilder&
FromBuilder::selfJoin(const SelectBuilder& subquery, const std::string& alias) {
  addJoin(",", subquery, alias);
  return *this;
}

FromBuilder&
FromBuilder::innerJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses) {
  addJoin(" INNER JOIN ", table, clauses);
  return *this;
}

FromBuilder&
FromBuilder::innerJoin(const SQLQuery& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  addJoin(" INNER JOIN ", subquery, alias, clauses);
  return *this;
}

FromBuilder&
FromBuilder::innerJoin(const SelectBuilder& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  addJoin(" INNER JOIN ", subquery, alias, clauses);
  return *this;
}

FromBuilder&
FromBuilder::leftOuterJoin(const std::string& table, const ClausesBuilder& clauses) {
  addJoin(" LEFT OUTER JOIN ", table, clauses);
  return *this;
}

FromBuilder&
FromBuilder::leftOuterJoin(const SQLQuery& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  addJoin(" LEFT OUTER JOIN ", subquery, alias, clauses);
  return *this;
}

FromBuilder&
FromBuilder::leftOuterJoin(const SelectBuilder& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  addJoin(" LEFT OUTER JOIN ", subquery, alias, clauses);
  return *this;
}

FromBuilder&
FromBuilder::rightOuterJoin(const std::string& table, const ClausesBuilder& clauses) {
  addJoin(" RIGHT OUTER JOIN ", table, clauses);
  return *this;
}

FromBuilder&
FromBuilder::rightOuterJoin(const SQLQuery& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  addJoin(" RIGHT OUTER JOIN ", subquery, alias, clauses);
  return *this;
}

FromBuilder&
FromBuilder::rightOuterJoin(const SelectBuilder& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  addJoin(" RIGHT OUTER JOIN ", subquery, alias, clauses);
  return *this;
}

FromBuilder&
FromBuilder::fullOuterJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses) {
  addJoin(" FULL OUTER JOIN ", table, clauses);
  return *this;
}

FromBuilder&
FromBuilder::fullOuterJoin(const SQLQuery& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  addJoin(" FULL OUTER JOIN ", subquery, alias, clauses);
  return *this;
}

FromBuilder&
FromBuilder::fullOuterJoin(const SelectBuilder& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  addJoin(" FULL OUTER JOIN ", subquery, alias, clauses);
  return *this;
}

FromBuilder&
FromBuilder::naturalJoin(const std::string& table) {
  _select._buffer << " NATURAL JOIN " << table;
  return *this;
}

FromBuilder&
FromBuilder::naturalJoin(const SQLQuery& subquery, const std::string& alias) {
  addJoin(" NATURAL JOIN ", subquery, alias);
  return *this;
}

FromBuilder&
FromBuilder::naturalJoin(const SelectBuilder& subquery, const std::string& alias) {
  addJoin(" NATURAL JOIN ", subquery, alias);
  return *this;
}

FromBuilder&
FromBuilder::crossJoin(const std::string& table) {
  _select._buffer << " CROSS JOIN " << table;
  return *this;
}

FromBuilder&
FromBuilder::crossJoin(const SQLQuery& subquery, const std::string& alias) {
  addJoin(" CROSS JOIN ", subquery, alias);
  return *this;
}

FromBuilder&
FromBuilder::crossJoin(const SelectBuilder& subquery, const std::string& alias) {
  addJoin(" CROSS JOIN ", subquery, alias);
  return *this;
}

SelectBuilder&
FromBuilder::where(const ClausesBuilder& clauses) {
  if(!clauses._firstClause) { // which means that at least one clause has been added
    _select._buffer << " WHERE " << clauses._buffer.str();
    for(const std::string& value : clauses._values) {
      _select._values.push_back(value);
    }
  }
  return _select;
}
// SQL commands -


// Methods +
void
FromBuilder::addJoin(const std::string& join, const SelectBuilder& subquery, const std::string& alias) {
  _select._buffer << join << '(' << subquery << ") " << alias;
  for(const std::string& value : subquery._values) {
    _select._values.push_back(value);
  }
}

void
FromBuilder::addJoin(const std::string& join, const SQLQuery& subquery, const std::string& alias) {
  _select._buffer << join << '(' << subquery.getQuery() << ") " << alias;
  for(const std::string& value : subquery.getValues()) {
    _select._values.push_back(value);
  }
}

void
FromBuilder::addJoin(const std::string& join, const std::string& table, const anch::sql::ClausesBuilder& clauses) {
  if(!clauses._firstClause) {
    _select._buffer << join << table << " ON " << clauses._buffer.str();
    for(const std::string& value : clauses._values) {
      _select._values.push_back(value);
    }
  }
}

void
FromBuilder::addJoin(const std::string& join, const SelectBuilder& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  if(!clauses._firstClause) {
    _select._buffer << join << '(' << subquery << ") " << alias << " ON " << clauses._buffer.str();
    for(const std::string& value : subquery._values) {
      _select._values.push_back(value);
    }
    for(const std::string& value : clauses._values) {
      _select._values.push_back(value);
    }
  }
}

void
FromBuilder::addJoin(const std::string& join, const SQLQuery& subquery, const std::string& alias, const ClausesBuilder& clauses) {
  if(!clauses._firstClause) {
    _select._buffer << join << '(' << subquery.getQuery() << ") " << alias << " ON " << clauses._buffer.str();
    for(const std::string& value : subquery.getValues()) {
      _select._values.push_back(value);
    }
    for(const std::string& value : clauses._values) {
      _select._values.push_back(value);
    }
  }
}
// Methods -
