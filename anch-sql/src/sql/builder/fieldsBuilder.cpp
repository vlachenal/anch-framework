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
#include "sql/builder/fieldsBuilder.hpp"

#include "sql/builder/selectBuilder.hpp"

using anch::sql::FieldsBuilder;
using anch::sql::SelectBuilder;
using anch::sql::FromBuilder;


// Constructors +
FieldsBuilder::FieldsBuilder(SelectBuilder& select, const std::string& column): SelectBuilderPart(select) {
  select._buffer << column;
}
// Constructors -

// Destructor +
FieldsBuilder::~FieldsBuilder() {
  // Nothing to do
}
// Destructor -

// SQL commands +
FieldsBuilder&
FieldsBuilder::field(const std::string& column) {
  _select._buffer << ',' << column;
  return *this;
}

FieldsBuilder&
FieldsBuilder::as(const std::string& alias) {
  _select._buffer << " AS " << alias;
  return *this;
}

FieldsBuilder&
FieldsBuilder::asc() {
  _select._buffer << " ASC";
  return *this;
}

FieldsBuilder&
FieldsBuilder::desc() {
  _select._buffer << " DESC";
  return *this;
}

FromBuilder
FieldsBuilder::from(const std::string& table) {
  return FromBuilder(_select, table);
}
// SQL commands -
