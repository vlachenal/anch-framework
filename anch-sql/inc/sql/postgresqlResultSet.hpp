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


typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;


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
      /*! PostgreSQL database connection */
      PGconn* _conn;

      /*! PostgreSQL result */
      PGresult* _result;

      /*! Current row in current result */
      int _currentRow;

      /*! Number of rows in current result */
      int _nbRows;

      /*! Number of fields in result */
      int _nbFields;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref PostgreSQLResultSet constructor
       *
       * \param conn the PostgreSQL database connection
       */
      PostgreSQLResultSet(PGconn* conn) noexcept;
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref PostgreSQLResultSet destructor
       */
      virtual ~PostgreSQLResultSet() noexcept;
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

#endif // _ANCH_SQL_PG_RESULT_SET_H_
#endif // ANCH_SQL_POSTGRESQL
