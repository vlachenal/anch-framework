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
#ifdef ANCH_SQL_SQLITE3
#ifndef _ANCH_SQL_SQLITE3_RESULT_SET_
#define _ANCH_SQL_SQLITE3_RESULT_SET_

#include "sql/resultSet.hpp"

#include "sqlite3.h"


namespace anch {
  namespace sql {

    /*!
     * \brief SQLite3 result set implementation
     *
     * Implements \ref ResultSet for SQLite3
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class SQLite3ResultSet: public ResultSet {

      // Attributes +
    private:
      /*! PostgreSQL result */
      sqlite3_stmt* _stmt;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref SQLite3ResultSet constructor
       *
       * \param stmt the SQLite3 statement
       * \param fields the fields' name
       * \param nbRow the number of row in result set
       */
      SQLite3ResultSet(sqlite3_stmt* stmt, const std::vector<std::string>& fields, int nbRow);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref SQLite3ResultSet destructor
       */
      virtual ~SQLite3ResultSet();
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

#endif // _ANCH_SQL_SQLITE3_RESULT_SET_
#endif // ANCH_SQL_SQLITE3
