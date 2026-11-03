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
#pragma once

#include "sql/connection.hpp"


typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;


namespace anch {
  namespace sql {

    namespace postgresql {

      /*!
       * Number of rows affected by a command
       *
       * \param res the command result
       *
       * \return the number of rows (\c 0 when command has no row count)
       *
       * \since 0.2
       */
      uint64_t affectedRows(PGresult* res) noexcept;

      /*!
       * Build SQL exception from PostgreSQL error: it is fatal when the connection is broken (\c PQstatus )
       *
       * \param message the error context
       * \param conn the PostgreSQL connection
       * \param res the PostgreSQL result (its error message is used when not \c NULL ; it is cleared)
       *
       * \return the exception
       *
       * \since 0.2
       */
      SqlException error(const std::string& message, PGconn* conn, PGresult* res = NULL);

    }

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
       * \param connStr the PostgreSQL connection string
       *
       * \throw SqlException fail to create connection
       */
      PostgreSQLConnection(const std::string& connStr);

      /*!
       * \ref PostgreSQLConnection constructor
       *
       * \param config the PostgreSQL database configuration
       *
       * \throw SqlException fail to create connection
       */
      PostgreSQLConnection(const SqlConnectionConfiguration& config);

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
      virtual ResultSet* executeQuery(const std::string& query) override;

      /*!
       * Execute SQL update query
       *
       * \param query the SQL query to execute
       *
       * \return the number of updated rows
       *
       * \throw SqlException any error
       */
      virtual uint64_t executeUpdate(const std::string& query) override;

      /*!
       * Check PostgreSQL connection status (\c PQstatus )
       *
       * \return \c true when connection is \c CONNECTION_OK , \c false otherwise
       */
      virtual bool checkConnection() const noexcept override;

      /*!
       * Send commit to database server
       *
       * \throw SqlException fail to commit transaction
       */
      virtual void sendCommit() override;

      /*!
       * Send rollback to database server
       *
       * \throw SqlException fail to rollback transaction
       */
      virtual void sendRollback() override;

      /*!
       * Send start transaction to database server
       *
       * \throw SqlException any error
       */
      virtual void sendStartTransaction() override;

      /*!
       * Send SQL query to prepare SQL statement
       *
       * \param query the SQL query
       *
       * \return the prepared statement
       *
       * \throw SqlException any error
       */
      virtual PreparedStatement* makePrepared(const std::string& query) override;
      // Methods -

    };

  }
}

#endif // ANCH_SQL_POSTGRESQL
