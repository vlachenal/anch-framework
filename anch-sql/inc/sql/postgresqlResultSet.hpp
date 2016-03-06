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
#ifdef ANCH_SQL_POSTGRESQL
#ifndef _ANCH_SQL_PG_RESULT_SET_H_
#define _ANCH_SQL_PG_RESULT_SET_H_

#include "sql/resultSet.hpp"

#include "libpq-fe.h"


namespace anch {
  namespace sql {

    /*!
     * \brief PostgreSQL result set implementation
     *
     * Implements \ref ResultSet for PostgreSQL
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class PostgreSQLResultSet: public ResultSet {

      // Attributes +
    private:
      /*! PostgreSQL result */
      PGresult* _result;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref PostgreSQLResultSet constructor
       *
       * \param result the PostgreSQL result
       * \param fields the fields' name
       * \param nbRow the number of row in result set
       */
      PostgreSQLResultSet(PGresult* result, const std::vector<std::string>& fields, int nbRow);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref PostgreSQLResultSet destructor
       */
      virtual ~PostgreSQLResultSet();
      // Destructor -

      // Methods +
    protected:
      /*!
       * Retrieve string value from result set according to SQL database engine.
       *
       * \param idx the field index
       * \param out the result
       */
      virtual bool getValue(std::size_t idx, std::string& out);

      /*!
       * Fetch next row in SQL result set.
       *
       * \throw SqlException any error
       */
      virtual void fetchNextRow() throw(SqlException);
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_PG_RESULT_SET_H_
#endif // ANCH_SQL_POSTGRESQL
