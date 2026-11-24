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
#include "sql/builder/clauses.hpp"


namespace anch::sql {

  std::string
  notEquals(const std::string& column, std::span<const std::string> names) {
    return column + " <> :" + names[0];
  }

  std::string
  equals(const std::string& column, std::span<const std::string> names) {
    return column + " = :" + names[0];
  }

  std::string
  greater(const std::string& column, std::span<const std::string> names) {
    return column + " > :" + names[0];
  }

  std::string
  greaterEquals(const std::string& column, std::span<const std::string> names) {
    return column + " >= :" + names[0];
  }

  std::string
  lesser(const std::string& column, std::span<const std::string> names) {
    return column + " < :" + names[0];
  }

  std::string
  lesserEquals(const std::string& column, std::span<const std::string> names) {
    return column + " <= :" + names[0];
  }

  std::string
  like(const std::string& column, std::span<const std::string> names) {
    return column + " LIKE :" + names[0];
  }

  std::string
  notLike(const std::string& column, std::span<const std::string> names) {
    return column + " NOT LIKE :" + names[0];
  }

  std::string
  between(const std::string& column, std::span<const std::string> names) {
    return column + " BETWEEN :" + names[0] + " AND :" + names[1];
  }

  std::string
  notBetween(const std::string& column, std::span<const std::string> names) {
    return column + " NOT BETWEEN :" + names[0] + " AND :" + names[1];
  }

  std::string
  in(const std::string& column, std::span<const std::string> names) {
    return column + " IN (:" + names[0] + ")";
  }

  std::string
  notIn(const std::string& column, std::span<const std::string> names) {
    return column + " NOT IN (:" + names[0] + ")";
  }

}  // anch::sql
