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

#include <string>
#include <functional>
#include "mysql.h"

#include "sql/connection.hpp"

//typedef struct st_mysql MYSQL;
//struct MYSQL;

namespace anch {
  namespace sql {

    /*!
     * \brief MySQL connection
     *
     * SQL connection implementation through MySQL native library
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class MySQLConnection: public Connection {

      // Attributes +
    private:
      /*! MySQL connection */
      MYSQL* _mysql;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref MySQLConnection constructor
       *
       * \param host the MySQL server host
       * \param user the connection user
       * \param password the connection password
       * \param database the database name
       * \param port the connection port
       * \param app the client application (default: AnCH) ; used to retrieve configuration per application if declared in my.cnf
       *
       * \throw SqlException any error
       */
      MySQLConnection(const std::string& host,
		      const std::string& user,
		      const std::string& password,
		      const std::string& database,
		      int port,
		      const std::string& app = "AnCH");

      /*!
       * \ref MySQLConnection constructor
       *
       * \param config the MySQL database configuration
       *
       * \throw SqlException any error
       */
      MySQLConnection(const SqlConnectionConfiguration& config);

      /*!
       * Prohibits \ref MySQLConnection copy constructor
       */
      MySQLConnection(const MySQLConnection&);
      // Constructors -

      // Destructor +
      /*!
       * \ref MySQLConnection destructor
       */
      virtual ~MySQLConnection() noexcept;
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

#endif // ANCH_SQL_MYSQL
