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
#ifndef _ANCH_SQL_CON_FACT_H_
#define _ANCH_SQL_CON_FACT_H_

#include "sql/connection.hpp"
#include "sql/sqlException.hpp"

#include "resourcePool.hpp"
#include "singleton.hpp"

#include <string>
#include <map>


namespace anch {
  namespace sql {

    /*!
     * Create a new shared pointer on SQL connection.
     *
     * \param config the SQL database configuration
     *
     * \return the connection
     *
     * \throw SqlException any error
     */
    extern std::shared_ptr<Connection> make_shared_connection(const SqlConnectionConfiguration& config);

    /*! SQL connections pool definition */
    using SqlConnectionPool = anch::ResourcePool<Connection, SqlConnectionConfiguration, make_shared_connection>;

    /*!
     * \brief SQL connection factory
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class SqlConnectionFactory: public anch::Singleton<SqlConnectionFactory> {
      friend anch::Singleton<SqlConnectionFactory>;

      // Attributes +
    private:
      /*! SQL database configurations */
      std::map<std::string, SqlConnectionConfiguration> _configs;

      /*! SQL database pools */
      std::map<std::string, SqlConnectionPool* > _pools;
      // Attributes -

      // Constructors +
    private:
      /*!
       * \ref SqlConnectionFactory private constructor.\n
       * Read databases configuration file and create database configurations.
       */
      SqlConnectionFactory();

      /*!
       * Prohibits \ref SqlConnectionFactory copy constructor
       *
       * \param other the \ref SqlConnectionFactory to copy
       */
      SqlConnectionFactory(const SqlConnectionFactory& other) = delete;
      // Constructors -

      // Destructor +
    private:
      /*!
       * \ref SqlConnectionFactory private destructor
       */
      virtual ~SqlConnectionFactory();
      // Destructor -

      // Methods +
    public:
      /*!
       * Create a new database connection.
       *
       * \param name the connection name
       *
       * \return the SQL database connection
       */
      Connection* createConnection(const std::string& name);

      /*!
       * Retrieve SQL connection pool
       *
       * \param name the pool name
       *
       * \return the pool
       */
      SqlConnectionPool& getPool(const std::string& name);
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_CON_FACT_H_
