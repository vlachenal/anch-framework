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
#ifndef _ANCH_SQL_SQLITE3CON_H_
#define _ANCH_SQL_SQLITE3CON_H_

#include "sql/connection.hpp"

#include "sqlite3.h"


namespace anch {
  namespace sql {

    /*!
     * \brief SQLite3 connection
     *
     * SQL connection implementation through SQLite3 native library
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class SQLite3Connection: public Connection {

      // Attributes +
    private:
      /*! SQLite3 database connection */
      sqlite3* _conn;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref SQLite3Connection constructor
       *
       * \param database the database path
       *
       * \throw SqlException fail to create connection
       */
      SQLite3Connection(const std::string& database) throw(SqlException);

      /*!
       * \ref SQLite3Connection constructor
       *
       * \param config the SQLite3 configuration
       *
       * \throw SqlException fail to create connection
       */
      SQLite3Connection(const SqlConnectionConfiguration& config) throw(SqlException);

      /*!
       * Prohibit \ref SQLite3Connection copy constructor
       */
      SQLite3Connection(const SQLite3Connection&) = delete;
      // Constructors -

      // Destructor +
      /*!
       * \ref SQLite3Connection destructor
       */
      virtual ~SQLite3Connection() noexcept;
      // Destructor -

      // Methods +
    protected:
      /*!
       * Execute SQL select query
       *
       * \param query the SQL query to execute
       *
       * \return the result set
       *
       * \throw SqlException any error
       */
      virtual ResultSet* executeQuery(const std::string& query) throw(SqlException) override;

      /*!
       * Execute SQL update query
       *
       * \param query the SQL query to execute
       *
       * \return the number of updated rows
       *
       * \throw SqlException any error
       */
      virtual uint64_t executeUpdate(const std::string& query) throw(SqlException) override;

      /*!
       * Send commit to database server
       *
       * \throw SqlException fail to commit transaction
       */
      virtual void sendCommit() throw(SqlException) override;

      /*!
       * Send rollback to database server
       *
       * \throw SqlException fail to rollback transaction
       */
      virtual void sendRollback() throw(SqlException) override;

      /*!
       * Send start transaction to database server
       *
       * \throw SqlException any error
       */
      virtual void sendStartTransaction() throw(SqlException) override;

      /*!
       * Send SQL query to prepare SQL statement
       *
       * \param query the SQL query
       *
       * \return the prepared statement
       *
       * \throw SqlException any error
       */
      virtual PreparedStatement* makePrepared(const std::string& query) throw(SqlException) override;
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_SQLITE3CON_H_
#endif // ANCH_SQL_SQLITE3
