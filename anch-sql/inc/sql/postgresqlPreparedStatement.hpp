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
#ifndef _ANCH_SQL_PG_PREPARED_STATEMENT_H_
#define _ANCH_SQL_PG_PREPARED_STATEMENT_H_

#include "sql/preparedStatement.hpp"
#include "sql/postgresqlConnection.hpp"

#include <atomic>

typedef struct pg_conn PGconn;


namespace anch {
  namespace sql {

    class PostgreSQLConnection;

    /*!
     * \brief PostgreSQL prepared statement implementation
     *
     * Compile prepared statement in constructor, manage bind and statement execution.
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class PostgreSQLPreparedStatement: public PreparedStatement {

      // Attributes +
    private:
      /*! Statement counter */
      static std::atomic<std::uint64_t> _counter;

      /*! PostgreSQL database connection */
      PGconn* _conn;

      /** PostgreSQL statement identifier */
      std::string _stmtId;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref PostgreSQLPreparedStatement constructor
       *
       * \param dbCon the database connection
       * \param query the SQL query
       *
       * \throw SqlException any error
       */
      PostgreSQLPreparedStatement(PGconn* dbCon, const std::string& query) throw(SqlException);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref PostgreSQLPreparedStatement destructor
       */
      virtual ~PostgreSQLPreparedStatement();
      // Destructor -

      // Methods +
    public:
      /*!
       * Execute prepared statement for SELECT statement
       *
       * \return the result set
       *
       * \throw SqlException any error
       */
      virtual ResultSet* executeQuery() throw(SqlException) override;

      /*!
       * Execute prepared statement for update database (INSERT, UPDATE, DELETE)
       *
       * \return the number of affected rows
       *
       * \throw SqlException any error
       */
      virtual uint64_t executeUpdate() throw(SqlException) override;
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_PG_PREPARED_STATEMENT_H_
#endif // ANCH_SQL_POSTGRESQL
