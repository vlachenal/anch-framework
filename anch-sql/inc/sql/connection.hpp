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


namespace anch {
  namespace sql {

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
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref Connection default constructor
       */
      Connection();
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
       * Execute SQL query
       *
       * \param query the SQL query to execute
       *
       * \return the result set which has to be deleted after use
       *
       * \throw SqlException any error
       */
      virtual ResultSet* query(const std::string& query) throw(SqlException) = 0;

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

    protected:
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
      // Methods -

      // Accessors +
      /*!
       * Auto commit status getter
       *
       * \return the status
       */
      inline bool isAutoCommit() const {
	return _autoCommit;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_SQL_CON_H_
