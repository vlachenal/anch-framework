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
#include "sql/builder/clausesBuilder.hpp"

#include "sql/builder/selectBuilder.hpp"
#include "sql/builder/fromBuilder.hpp"


using anch::sql::ClausesBuilder;
using anch::sql::SelectBuilder;
using anch::sql::SQLQuery;
using anch::sql::FromBuilder;
using anch::sql::ClausesProvider;
using anch::sql::ValueChecker;


ClausesBuilder::ClausesBuilder(): _buffer(), _values(), _firstClause(true) {
  // Nothing to do
}

ClausesBuilder&
ClausesBuilder::field(const std::string& column) {
  _buffer << column;
  _firstClause = false;
  return *this;
}

ClausesBuilder&
ClausesBuilder::NOT(const std::string& clauses) {
  _buffer << "NOT(" << clauses << ')';
  _firstClause = false;
  return *this;
}

ClausesBuilder&
ClausesBuilder::isNull() {
  _buffer << " IS NULL";
  return *this;
}

ClausesBuilder&
ClausesBuilder::isNotNull() {
  _buffer << " IS NOT NULL";
  return *this;
}

ClausesBuilder&
ClausesBuilder::notEquals() {
  _buffer << " <> ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::equals() {
  _buffer << " = ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::notExists(const SelectBuilder& query) {
  _buffer << "NOT EXISTS(" << query << ')';
  return *this;
}

ClausesBuilder&
ClausesBuilder::exists(const SelectBuilder& query) {
  _buffer << "EXISTS(" << query << ')';
  return *this;
}

ClausesBuilder&
ClausesBuilder::in(const SQLQuery& query) {
  _buffer << " IN(" << query.getQuery() << ')';
  return *this;
}

ClausesBuilder&
ClausesBuilder::notIn(const SQLQuery& query) {
  _buffer << " NOT IN(" << query.getQuery() << ')';
  return *this;
}

ClausesBuilder&
ClausesBuilder::in(const SelectBuilder& query) {
  _buffer << " IN(" << query << ')';
  return *this;
}

ClausesBuilder&
ClausesBuilder::notIn(const SelectBuilder& query) {
  _buffer << " NOT IN(" << query << ')';
  return *this;
}

ClausesBuilder&
ClausesBuilder::greater() {
  _buffer << " > ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::greaterEquals() {
  _buffer << " >= ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::lesser() {
  _buffer << " < ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::lesserEquals() {
  _buffer << " <= ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::like() {
  _buffer << " LIKE ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::notLike() {
  _buffer << " NOT LIKE ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::between() {
  _buffer << " BETWEEN ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::notBetween() {
  _buffer << " NOT BETWEEN ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::AND() {
  _buffer << " AND ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::OR() {
  _buffer << " OR ";
  return *this;
}

ClausesBuilder&
ClausesBuilder::AND(ClausesProvider clauses) {
  return checkAndAddClauses("AND", clauses);
}

ClausesBuilder&
ClausesBuilder::OR(ClausesProvider clauses) {
  return checkAndAddClauses("OR", clauses);
}

ClausesBuilder&
ClausesBuilder::AND(const ClausesBuilder& other) {
  return checkAndAddClauses("AND", other);
}

ClausesBuilder&
ClausesBuilder::OR(const ClausesBuilder& other) {
  return checkAndAddClauses("OR", other);
}

ClausesBuilder&
ClausesBuilder::operator ||(const ClausesBuilder& other) {
  return OR(other);
}

ClausesBuilder&
ClausesBuilder::checkAndAddClauses(const std::string& boolAgg, const ClausesBuilder& other) {
  if(!other._firstClause) {
    addBooleanAggregator(boolAgg);
    _buffer << '(' << other._buffer.str() << ')';
    for(const std::string& val : other._values) {
      _values.push_back(val);
    }
    _firstClause = false;
  }
  return *this;
}

void
ClausesBuilder::addBooleanAggregator(const std::string& boolAgg) {
  if(!_firstClause) {
    _buffer << ' ' << boolAgg << ' ';
  }
}

ClausesBuilder&
ClausesBuilder::checkAndAddClauses(const std::string& boolAgg, ClausesProvider clauses) {
  return checkAndAddClauses(boolAgg, clauses());
}
