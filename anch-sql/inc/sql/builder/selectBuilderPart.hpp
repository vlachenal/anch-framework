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

#include <string>
#include <cstdint>

#include "sql/builder/sqlQuery.hpp"


namespace anch {
  namespace sql {

    class SelectBuilder;
    class FieldsBuilder;
    class ClausesBuilder;

    /*!
     * \brief SQL select query part builder
     *
     * Acts like a decorator to call some initial \ref SelectBuilder methods.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    class SelectBuilderPart {

      // Attributes +
    protected:
      /*! SQL select query builder */
      anch::sql::SelectBuilder& _select;
      // Attributes -

      // Constructors +
    protected:
      /*!
       * Prohibits \ref SelectBuilderPart default constructor
       */
      SelectBuilderPart() = delete;

      /**
       * \ref SelectBuilderPart constructor
       *
       * \param select the select query builder
       */
      SelectBuilderPart(anch::sql::SelectBuilder& select);
      // Constructors -

      // Destructor +
      /*!
       * \ref SelectBuilderPart destructor
       */
      virtual ~SelectBuilderPart();
      // Destructor -

      // SQL commands +
    public:
      /*!
       * Add SQL <tt>GROUP BY</tt> command
       *
       * \param column the first column to group by
       *
       * \return a new \ref FieldsBuilder to add other columns
       */
      anch::sql::FieldsBuilder groupBy(const std::string& column);

      /*!
       * Add SQL <tt>ORDER BY</tt> command
       *
       * \param column the first column to group by
       *
       * \return a new \ref FieldsBuilder to add other columns
       */
      anch::sql::FieldsBuilder orderBy(const std::string& column);

      /*!
       * Add SQL \c HAVING command and its clauses if not empty
       *
       * \param clauses the clauses
       *
       * \return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& having(anch::sql::ClausesBuilder& clauses);

      /*!
       * Add \c UNION to other SQL request
       *
       * \param other the other SQL request
       *
       * \return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& unionOnce(anch::sql::SelectBuilder& other);

      /*!
       * Add \c UNION to other SQL request
       *
       * \param other the other SQL request
       *
       * \return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& unionAll(anch::sql::SelectBuilder& other);

      /*!
       * Add <tt>OFFSET n ROWS</tt>
       *
       * \param offset the offset value
       *
       * \return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& offset(int64_t offset);

      /*!
       * Add <tt>FETCH FIRST n ROWS ONLY</tt>
       *
       * \param limit the limit value
       *
       * \return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& fetch(uint64_t limit);

      /*!
       * Modify current query to add apply \c row_number window function as define in SQL:2003 standard.
       *
       * \param alias the current query table alias
       * \param rowColumn the row column alias
       * \param order the \c ORDER instruction
       * \param min the minimum row number value
       * \param max the maximum row number value
       *
       * \return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& windowByRowNumber(const std::string& alias, const std::string& rowColumn, const std::string& order, uint32_t min, uint32_t max);

      /*!
       * Modify current query to add apply \c rank window function as define in SQL:2003 standard.
       *
       * \param alias the current query table alias
       * \param rowColumn the row column alias
       * \param order the \c ORDER instruction
       * \param min the minimum row number value
       * \param max the maximum row number value
       *
       * \return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& windowByRank(const std::string& alias, const std::string& rowColumn, const std::string& order, uint32_t min, uint32_t max);
      // SQL commands -

      // Methods +
    public:
      /**
       * Return the original \ref SelectBuilder
       *
       * @return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& done();

      /*!
       * Build SQL query
       *
       * \return the query and its prepared statement values
       */
      anch::sql::SQLQuery build();
      // Methods -

    };

  }  // sql
}  // anch
