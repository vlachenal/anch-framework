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
#ifdef ANCH_SQL_MYSQL
#ifndef _ANCH_SQL_MYSQL_RESULT_SET_H_
#define _ANCH_SQL_MYSQL_RESULT_SET_H_

#include "sql/resultSet.hpp"

#include <list>

#include "mysql.h"


namespace anch {
  namespace sql {

    /*!
     * \brief MySQL result set implementation
     *
     * Implements \ref ResultSet for MySQL
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class MySQLResultSet: public ResultSet {

      // Attributes +
    private:
      /*! MySQL result */
      MYSQL_RES* _result;

      /*! MySQL current row */
      MYSQL_ROW _row;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref MySQLResultSet constructor
       *
       * \param result the MySQL result
       * \param fields the fields' name
       * \param nbRow the number of row in result set
       */
      MySQLResultSet(MYSQL_RES* result, const std::vector<std::string>& fields, std::size_t nbRow);
      // Constructors -

      // Destructor +
      /*!
       * \ref MySQLResultSet destructor
       */
      virtual ~MySQLResultSet();
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

#endif // _ANCH_SQL_MYSQL_RESULT_SET_H_
#endif // ANCH_SQL_MYSQL
