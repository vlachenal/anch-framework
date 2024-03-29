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
#pragma once

#include "sql/preparedStatement.hpp"
#include "sql/mysqlConnection.hpp"

#include <atomic>
#include "mysql.h"

//typedef struct st_mysql_stmt MYSQL_STMT;
//struct MYSQL_STMT;

namespace anch {
  namespace sql {

    class MySQLConnection;

    /*!
     * \brief MySQL prepared statement implementation
     *
     * Compile prepared statement in constructor, manage bind and statement execution.
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class MySQLPreparedStatement: public PreparedStatement {

      // Attributes +
    private:
      /*! Statement counter */
      static std::atomic<std::uint64_t> _counter;

      /*! MySQL connection */
      MYSQL* _conn;

      /*! MySQL statement */
      MYSQL_STMT* _stmt;

      /*! MySQL statement identifier */
      std::string _stmtId;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref MySQLPreparedStatement constructor
       *
       * \param dbCon the database connection
       * \param query the SQL query
       *
       * \throw SqlException any error
       */
      MySQLPreparedStatement(MYSQL* dbCon, const std::string& query);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref MySQLPreparedStatement destructor
       */
      virtual ~MySQLPreparedStatement() noexcept;
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
      virtual ResultSet* executeQuery() override;

      /*!
       * Execute prepared statement for update database (INSERT, UPDATE, DELETE)
       *
       * \return the number of affected rows
       *
       * \throw SqlException any error
       */
      virtual uint64_t executeUpdate() override;
      // Methods -

    };

  }
}

#endif // ANCH_SQL_MYSQL
