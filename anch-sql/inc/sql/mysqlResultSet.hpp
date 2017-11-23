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

typedef struct st_mysql_res MYSQL_RES;


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
      char** _row;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref MySQLResultSet constructor
       *
       * \param result the MySQL result
       */
      MySQLResultSet(MYSQL_RES* result) noexcept;
      // Constructors -

      // Destructor +
      /*!
       * \ref MySQLResultSet destructor
       */
      virtual ~MySQLResultSet() noexcept;
      // Destructor -

      // Methods +
    public:
      /*!
       * Fetch next row in SQL result set.
       *
       * \return \c true if next row exists, \c false otherwise
       *
       * \throw SqlException any error
       */
      virtual bool next();

    protected:
      /*!
       * Retrieve string value from result set according to SQL database engine.
       *
       * \param idx the field index
       * \param out the result
       *
       * \throw SqlException any error
       */
      virtual bool getValue(std::size_t idx, std::string& out);

      /*!
       * Retrieve SQL date formatter
       *
       * \return the SQL date formatter
       */
      virtual const anch::date::DateFormatter& getDateFormatter();

      /*!
       * Retrieve SQL time formatter
       *
       * \return the SQL time formatter
       */
      virtual const anch::date::DateFormatter& getTimeFormatter();

      /*!
       * Retrieve SQL timestamp formatter
       *
       * \return the SQL timestamp formatter
       */
      virtual const anch::date::DateFormatter& getTimestampFormatter();
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_MYSQL_RESULT_SET_H_
#endif // ANCH_SQL_MYSQL
