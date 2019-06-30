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

#include <sstream>
#include <list>
#include <ostream>
#include <string>

#include "sql/builder/fieldsBuilder.hpp"
#include "sql/builder/clausesBuilder.hpp"
#include "sql/builder/sqlQuery.hpp"


namespace anch {
  namespace sql {

    extern SelectBuilder select();
    class FromBuilder;

    /*!
     * \brief Dynamic SQL select request builder
     *
     * Dynamic SQL select request builder
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class SelectBuilder {
      friend SelectBuilder anch::sql::select();
      friend anch::sql::FieldsBuilder;
      friend anch::sql::FromBuilder;

      /*!
       * \ref SelectBuilder stream insertion operator
       *
       * \param stream the output stream
       * \param builder the \ref SelectBuilder
       *
       * \return the output stream
       */
      friend std::ostream& operator<<(std::ostream& stream, const SelectBuilder& builder);

      // Attributes +
    private:
      /*! SQL request string buffer */
      std::ostringstream _buffer;

      /*! Clauses' values */
      std::list<std::string> _values;
      // Attributes -

      // Constructors +
    private:
      /*!
       * \ref SelectBuilder private constructor.\n
       * Only select function will initialize this object.
       */
      SelectBuilder();
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref SelectBuilder destructor
       */
      virtual ~SelectBuilder();
      // Destructor -

      // SQL commands +
    public:
      /*!
       * Add \c DISTINCT command
       *
       * \return \c this
       */
      SelectBuilder& distinct();

      /*!
       * Intialize a new fields builder
       *
       * \return the fields builder
       */
      anch::sql::FieldsBuilder field(const std::string& column);

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
       * \return \c this
       */
      SelectBuilder& having(anch::sql::ClausesBuilder& clauses);

      /*!
       * Add \c UNION to other SQL request
       *
       * \param other the other SQL request
       *
       * \return \c this
       */
      SelectBuilder& unionOnce(SelectBuilder& other);

      /*!
       * Add \c UNION to other SQL request
       *
       * \param other the other SQL request
       *
       * \return \c this
       */
      SelectBuilder& unionAll(SelectBuilder& other);

      /*!
       * Add <tt>OFFSET n ROWS</tt>
       *
       * \param offset the offset value
       *
       * \return \c this
       */
      SelectBuilder& offset(int64_t offset);

      /*!
       * Add <tt>FETCH FIRST n ROWS ONLY</tt>
       *
       * \param limit the limit value
       *
       * \return \c this
       */
      SelectBuilder& fetch(uint64_t limit);

      /*!
       * Modify current query to add apply \c row_number window function as define in SQL:2003 standard.
       *
       * \param alias the current query table alias
       * \param rowColumn the row column alias
       * \param order the \c ORDER instruction
       * \param min the minimum row number value
       * \param max the maximum row number value
       *
       * \return \c this
       */
      SelectBuilder& windowByRowNumber(const std::string& alias, const std::string& rowColumn, const std::string& order, uint32_t min, uint32_t max);

      /*!
       * Modify current query to add apply \c rank window function as define in SQL:2003 standard.
       *
       * \param alias the current query table alias
       * \param rowColumn the row column alias
       * \param order the \c ORDER instruction
       * \param min the minimum row number value
       * \param max the maximum row number value
       *
       * \return \c this
       */
      SelectBuilder& windowByRank(const std::string& alias, const std::string& rowColumn, const std::string& order, uint32_t min, uint32_t max);
      // SQL commands -

      // Methods +
      /*!
       * Build SQL query
       *
       * \return the query and its prepared statement values
       */
      anch::sql::SQLQuery build();

    private:
      /**
       * Modify current query to add window function as define in SQL:2003 standard.
       *
       * \param function the window function to apply
       * \param alias the current query table alias
       * \param rowColumn the row column alias
       * \param order thise \c ORDER instruction
       * \param min the minimum row number value
       * \param max the maximum row number value
       */
      void window(const std::string& function, const std::string& alias, const std::string& rowColumn, const std::string& order, uint32_t min, uint32_t max);
      // Methods -

    };

  } // sql
} // anch
