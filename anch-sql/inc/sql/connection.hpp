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
#ifndef _ANCH_SQL_CON_H_
#define _ANCH_SQL_CON_H_

#include <functional>

#include "sql/sqlException.hpp"
#include "sql/resultSet.hpp"
#include "sql/preparedStatement.hpp"


namespace anch {
  namespace sql {

    /*!
     * \brief SQL database connection configuration
     *
     * Parameters can not be used according to SQL database (especially for SQLite3).
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    struct SqlConnectionConfiguration {
      /*! SQL connection database type */
      std::string driver;

      /*! SQL database name */
      std::string database;

      /*! SQL database hostname */
      std::string hostname;

      /*! SQL database port */
      int port;

      /*! SQL database connection user */
      std::string user;

      /*! SQL database user password */
      std::string password;

      /*! Client application */
      std::string application;
    };

    /*!
     * \brief SQL connection virtual class
     *
     * Abstract layer for SQL connections management.
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class Connection {

      // Attributes +
    protected:
      /*! Auto commit */
      bool _autoCommit;

      /*! Is SQL connection valid */
      bool _valid;

      /*! Prepared statements */
      std::map<std::string, PreparedStatement*> _stmts;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref Connection default constructor
       *
       * \throw SqlException any error
       */
      Connection() throw(SqlException);

      /*!
       * Prohibit \ref Connection copy constructor
       */
      Connection(const Connection&) = delete;
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref Connection destrcutor
       */
      virtual ~Connection();
      // Destructor -

      // Methods +
    public:
      /*!
       * Commit current SQL transaction.\n
       * If auto commit is set, this method does nothing.
       *
       * \throw SqlException any error
       */
      void commit() throw(SqlException);

      /*!
       * Rollback current SQL transaction.\n
       * If auto commit is set, this method does nothing.
       *
       * \throw SqlException any error
       */
      void rollback() throw(SqlException);

      /*!
       * Auto commit status setter.\n
       * Change auto commit status in database connection too.\n
       * This method does nothing if auto commit is same as before.
       *
       * \param autoCommit the status
       *
       * \throw SqlException any error
       */
      void setAutoCommit(bool autoCommit) throw(SqlException);

      /*!
       * Prepare SQL statement if not already done.
       *
       * \param query the SQL query
       *
       * \return the prepared statement
       *
       * \throw SqlException any error
       */
      PreparedStatement& prepareStatement(const std::string& query) throw(SqlException);

      /*!
       * Execute SQL query
       *
       * \param query the SQL query to execute
       *
       * \return the result set which has to be deleted after use
       *
       * \throw SqlException any error
       */
      ResultSet* query(const std::string& query) throw(SqlException);

      /*!
       * Execute query and treat each row of result set
       *
       * \param sqlQuery the SQL query to execute
       * \param rowMapper the row mapper function
       *
       * \throw SqlException any error
       */
      void queryMapRow(const std::string& sqlQuery, std::function<void(ResultSet&)> rowMapper) throw(SqlException);

      /*!
       * Execute query and extract result set
       *
       * \param sqlQuery the SQL query to execute
       * \param resExtractor the result set extractor function
       *
       * \throw SqlException any error
       */
      void queryExtract(const std::string& sqlQuery, std::function<void(ResultSet&)> resExtractor) throw(SqlException);

      /*!
       * Execute update (INSERT, UPDATE or DELETE) SQL query.
       *
       * \param query the SQL query
       *
       * \throw SqlException any error
       */
      uint64_t update(const std::string& query) throw(SqlException);

    protected:
      /*!
       * Execute SQL select query
       *
       * \param query the SQL query to execute
       *
       * \return the result set which has to be deleted after use
       *
       * \throw SqlException any error
       */
      virtual ResultSet* executeQuery(const std::string& query) throw(SqlException) = 0;

      /*!
       * Execute SQL update query
       *
       * \param query the SQL query to execute
       *
       * \return the result set which has to be deleted after use
       *
       * \throw SqlException any error
       */
      virtual uint64_t executeUpdate(const std::string& query) throw(SqlException) = 0;

      /*!
       * Send commit to database server
       *
       * \throw SqlException any error
       */
      virtual void sendCommit() throw(SqlException) = 0;

      /*!
       * Send rollback to database server
       *
       * \throw SqlException any error
       */
      virtual void sendRollback() throw(SqlException) = 0;

      /*!
       * Send auto commit status modification to server
       *
       * \param autoCommit the status to send
       *
       * \throw SqlException any error
       */
      virtual void toggleAutoCommit(bool autoCommit) throw(SqlException) = 0;

      /*!
       * Send SQL query to prepare SQL statement
       *
       * \param query the SQL query
       *
       * \return the prepared statement
       *
       * \throw SqlException any error
       */
      virtual PreparedStatement* makePrepared(const std::string& query) throw(SqlException) = 0;
      // Methods -

      // Accessors +
    public:
      /*!
       * Auto commit status getter
       *
       * \return the status
       */
      inline bool isAutoCommit() const {
	return _autoCommit;
      }

      /*!
       * Valid status getter
       *
       * \return the status
       */
      inline bool isValid() const {
	return _valid;
      }

    protected:
      /*!
       * Valid status setter
       *
       * \param valid the status to set
       */
      inline void setValid(bool valid) {
	_valid = valid;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_SQL_CON_H_
