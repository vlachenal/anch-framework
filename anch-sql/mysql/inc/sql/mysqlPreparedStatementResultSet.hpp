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
#ifndef _ANCH_SQL_MYSQL_PS_RESULT_SET_H_
#define _ANCH_SQL_MYSQL_PS_RESULT_SET_H_

#include "sql/resultSet.hpp"

#include <list>


typedef struct st_mysql_stmt MYSQL_STMT;
typedef struct st_mysql_bind MYSQL_BIND;

namespace anch {
  namespace sql {

    /*!
     * \brief MySQL result set implementation for prepared statement ... shitty API
     *
     * Implements \ref ResultSet for MySQL for prepared statement
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class MySQLPreparedStatementResultSet: public ResultSet {

      // Attributes +
    private:
      /*! MySQL statement */
      MYSQL_STMT* _stmt;

      /*! Columns length */
      long unsigned int* _lengths;

      /*! Columns null */
      char* _nulls;

      /*! MySQL result binding */
      MYSQL_BIND* _binds;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref MySQLPreparedStatementResultSet constructor
       *
       * \param stmt the MySQL statement
       *
       * \throw SqlException any error
       */
      MySQLPreparedStatementResultSet(MYSQL_STMT* stmt);
      // Constructors -

      // Destructor +
      /*!
       * \ref MySQLPreparedStatementResultSet destructor
       */
      virtual ~MySQLPreparedStatementResultSet() noexcept;
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
      virtual bool getValue(std::size_t idx, std::string& out) override;

#ifdef ANCH_STD_OTP
      /*!
       * Retrieve string value from result set according to SQL database engine.
       *
       * \param idx the index
       *
       * \return the result
       *
       * \throw SqlException any error
       */
      virtual std::optional<std::string> getValue(std::size_t idx) override;
#endif // ANCH_STD_OTP

      /*!
       * Retrieve SQL date formatter
       *
       * \return the SQL date formatter
       */
      virtual const anch::date::DateFormatter& getDateFormatter() override;

      /*!
       * Retrieve SQL time formatter
       *
       * \return the SQL time formatter
       */
      virtual const anch::date::DateFormatter& getTimeFormatter() override;

      /*!
       * Retrieve SQL timestamp formatter
       *
       * \return the SQL timestamp formatter
       */
      virtual const anch::date::DateFormatter& getTimestampFormatter() override;
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_MYSQL_PS_RESULT_SET_H_
#endif // ANCH_SQL_MYSQL
