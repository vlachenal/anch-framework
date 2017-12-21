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
#ifndef _ANCH_SQL_SHARED_LIB_H_
#define _ANCH_SQL_SHARED_LIB_H_

#include <string>
#include <map>
#include <memory>

#include "connection.hpp"
#include "singleton.hpp"


namespace anch {
  namespace sql {

    /*!
     * \brief SQL shared library data
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    struct SQLSharedLibrary {
      /*! Layer library name */
      std::string layer;

      /*! Layer shared library object */
      void* _layer;

      /*! Make shared pointer SQL connection */
      std::shared_ptr<Connection> (*make_shared_connection)(const SqlConnectionConfiguration&);

      /*! Make SQL connection */
      Connection* (*create)(const SqlConnectionConfiguration&);
    };

    /*!
     * \brief SQL shared libraries manager
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class SQLSharedLibraries: public anch::Singleton<SQLSharedLibraries> {
      friend anch::Singleton<SQLSharedLibraries>;

      // Attributes +
    private:
      /*! SQL libraries registration */
      std::map<std::string,anch::sql::SQLSharedLibrary> _libraries;
      // Attributes -

      // Constructors +
    private:
      /*!
       * \ref SQLSharedLibraries private constructor
       */
      SQLSharedLibraries() noexcept;

      /*!
       * Prohibits \ref SQLSharedLibraries copy constructor
       *
       * \param other the \ref SQLSharedLibraries to copy
       */
      SQLSharedLibraries(const SQLSharedLibraries& other) = delete;
      // Constructors -

      // Destructor +
    private:
      /*!
       * \ref SQLSharedLibraies private destructor
       */
      virtual ~SQLSharedLibraries();
      // Destructor -

      // Methods +
    public:
      /*!
       * Register SQL library.\n
       * This method will not open the libraries.
       *
       * \param db the database client/server name which will be used in configuration file
       * \param library the library name
       */
      void registerSQLLibrary(const std::string& db, const std::string& library);

      /*!
       * Make shared SQL connection
       *
       * \param config the database configuration
       *
       * \return the SQL connection
       *
       * \throw SqlException when database has not been registered
       */
      std::shared_ptr<Connection> makeSharedConnection(const SqlConnectionConfiguration& config);

      /*!
       * Make new SQL connection
       *
       * \param config the database configuration
       *
       * \return the SQL connection
       *
       * \throw SqlException when database has not been registered
       */
      Connection* makeConnection(const SqlConnectionConfiguration& config);

      /*!
       * Register MySQL driver utlility method
       */
      inline static void registerMySQL() {
        getInstance().registerSQLLibrary("MySQL", "libanch-mysql.so");
      }

      /*!
       * Register PostgreSQL driver utlility method
       */
      inline static void registerPostgreSQL() {
	getInstance().registerSQLLibrary("PostgreSQL", "libanch-postgresql.so");
      }

      /*!
       * Register SQLite3 driver utlility method
       */
      inline static void registerSQLite() {
	getInstance().registerSQLLibrary("SQLite3", "libanch-sqlite.so");
      }
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_SHARED_LIB_H_
