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
#pragma once

#include "sql/builder/selectBuilder.hpp"
#include "sql/builder/clausesBuilder.hpp"


namespace anch {
  namespace sql {

    // Functions +
    /*!
     * Initialize a new \ref SelectBuilder
     *
     * \return the new \ref SelectBuilder
     */
    anch::sql::SelectBuilder select();

    /*!
     * Initialize a new \ref ClausesBuilder
     *
     * \return the new \ref ClausesBuilder
     */
    anch::sql::ClausesBuilder clauses();

    /*!
     * Initialize a new \ref ClausesBuilder
     *
     * \return the new \ref ClausesBuilder
     */
    template<typename T>
    anch::sql::ClausesBuilder clauses(anch::sql::ClauseMaker clause, T& value, anch::sql::ValueChecker<T> checker = anch::sql::isValidValue<T>);

    /*!
     * Initialize a new \ref ClausesBuilder
     *
     * \return the new \ref ClausesBuilder
     */
    template<typename T>
    anch::sql::ClausesBuilder clauses(const std::string& column, anch::sql::ClauseMaker clause, T& value, anch::sql::ValueChecker<T> checker = anch::sql::isValidValue<T>);
    // Functions -

    // Implementations +
    template<typename T>
    anch::sql::ClausesBuilder clauses(anch::sql::ClauseMaker clause, T& value, anch::sql::ValueChecker<T> checker) {
      return std::move(clauses().AND(clause, value, checker));
    }

    template<typename T>
    anch::sql::ClausesBuilder
    clauses(const std::string& column, anch::sql::ClauseMaker clause, T& value, anch::sql::ValueChecker<T> checker) {
      return std::move(clauses().AND(column, clause, value, checker));
    }
    // Implementations -

  } // sql
} // anch
