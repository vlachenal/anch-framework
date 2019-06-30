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
#include "sql/builder/clauses.hpp"


namespace anch {
  namespace sql {

    std::string notEquals(const std::string& column) {
      return column + " <> ?";
    }

    std::string equals(const std::string& column) {
      return column + " = ?";
    }

    std::string greater(const std::string& column) {
      return column + " > ?";
    }

    std::string greaterEquals(const std::string& column) {
      return column + " >= ?";
    }

    std::string lesser(const std::string& column) {
      return column + " < ?";
    }

    std::string lesserEquals(const std::string& column) {
      return column + " <= ?";
    }

    std::string like(const std::string& column) {
      return column + " LIKE ?";
    }

    std::string notLike(const std::string& column) {
      return column + " NOT LIKE ?";
    }

    std::string between(const std::string& column) {
      return column + " BETWEEN ? AND ?";
    }

    std::string notBetween(const std::string& column) {
      return column + " NOT BETWEEN ? AND ?";
    }

    std::string in(const std::string& column) {
      return column + " IN ";
    }

    std::string notIn(const std::string& column) {
      return column + " NOT IN ";
    }

    std::string exists(const std::string&) {
      return "EXISTS ";
    }

    std::string notExists(const std::string&) {
      return "NOT EXISTS ";
    }

  }  // sql
}  // anch
