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
#include "sql/builder/selectBuilderPart.hpp"

#include "sql/builder/selectBuilder.hpp"
#include "sql/builder/fieldsBuilder.hpp"

using anch::sql::SelectBuilderPart;
using anch::sql::SelectBuilder;
using anch::sql::FieldsBuilder;
using anch::sql::ClausesBuilder;
using anch::sql::SQLQuery;


// Constructors +
SelectBuilderPart::SelectBuilderPart(SelectBuilder& select): _select(select) {
  // Nothing to do
}
// Constructors -

// Destructor +
SelectBuilderPart::~SelectBuilderPart() {
  // Nothing to do
}
// Destructor -

// SQL commands +
FieldsBuilder
SelectBuilderPart::groupBy(const std::string& column) {
  return _select.groupBy(column);
}

FieldsBuilder
SelectBuilderPart::orderBy(const std::string& column) {
  return _select.orderBy(column);
}

SelectBuilder&
SelectBuilderPart::having(ClausesBuilder& clauses) {
  return _select.having(clauses);
}

SelectBuilder&
SelectBuilderPart::unionOnce(SelectBuilder& other) {
  return _select.unionOnce(other);
}

SelectBuilder&
SelectBuilderPart::unionAll(anch::sql::SelectBuilder& other) {
  return _select.unionAll(other);
}

SelectBuilder&
SelectBuilderPart::offset(int64_t offset) {
  return _select.offset(offset);
}

SelectBuilder&
SelectBuilderPart::fetch(uint64_t limit) {
  return _select.fetch(limit);
}

SelectBuilder&
SelectBuilderPart::windowByRowNumber(const std::string& alias, const std::string& rowColumn, const std::string& order, uint32_t min, uint32_t max) {
  return _select.windowByRowNumber(alias, rowColumn, order, min, max);
}

SelectBuilder&
SelectBuilderPart::windowByRank(const std::string& alias, const std::string& rowColumn, const std::string& order, uint32_t min, uint32_t max) {
  return _select.windowByRank(alias, rowColumn, order, min, max);
}
// SQL commands -

// Methods +
SelectBuilder&
SelectBuilderPart::done() {
  return _select;
}

SQLQuery
SelectBuilderPart::build() {
  return _select.build();
}
// Methods -
