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
#ifndef _ANCH_SQL_PGCON_H_
#define _ANCH_SQL_PGCON_H_

#include "sql/connection.hpp"


typedef struct pg_conn PGconn;


namespace anch {
  namespace sql {

    /*!
     * \brief PostgreSQL connection
     *
     * SQL connection implementation through PostgreSQL native library
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class PostgreSQLConnection: public Connection {

      // Attributes +
    private:
      /*! PostgreSQL database connection */
      PGconn* _conn;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref PostgreSQLConnection constructor
       *
       * \param host the PostgreSQL server hostname
       * \param user the datase user login
       * \param password the database login password
       * \param database the database name
       * \param port the database access port
       * \param app the application name
       *
       * \throw SqlException fail to create connection
       */
      PostgreSQLConnection(const std::string& host,
			   const std::string& user,
			   const std::string& password,
			   const std::string& database,
			   int port,
			   const std::string& app = "")
	throw(SqlException);

      /*!
       * \ref PostgreSQLConnection constructor
       *
       * \param connStr the PostgreSQL connection string
       *
       * \throw SqlException fail to create connection
       */
      PostgreSQLConnection(const std::string& connStr) throw(SqlException);

      /*!
       * \ref PostgreSQLConnection constructor
       *
       * \param config the PostgreSQL database configuration
       *
       * \throw SqlException fail to create connection
       */
      PostgreSQLConnection(const SqlConnectionConfiguration& config) throw(SqlException);

      /*!
       * Prohibit \ref PostgreSQLConnection copy constructor
       */
      PostgreSQLConnection(const PostgreSQLConnection&) = delete;
      // Constructors -

      // Destructor +
      /*!
       * \ref PostgreSQLConnection destructor
       */
      virtual ~PostgreSQLConnection() noexcept;
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

#endif // _ANCH_SQL_PGCON_H_
#endif // ANCH_SQL_POSTGRESQL
