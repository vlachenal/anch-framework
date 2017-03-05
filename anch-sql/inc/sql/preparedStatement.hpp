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
#ifndef _ANCH_SQL_PREPARED_STATEMENT_H_
#define _ANCH_SQL_PREPARED_STATEMENT_H_

#include <set>
#include <string>

#include "sql/sqlException.hpp"
#include "sql/resultSet.hpp"


namespace anch {
  namespace sql {

    /*!
     * \brief SQL prepared statement
     *
     * Virtual class for SQL prepared statement management.
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    class PreparedStatement {

      // Attributes +
    protected:
      /*! Statement values */
      std::map<std::size_t,std::string> _values;

      /*! Number of wildcards in SQL query */
      std::size_t _nbWildcards;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref PreparedStatement constructor
       *
       * \param query the SQL query
       */
      PreparedStatement();
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref PreparedStatement destructor
       */
      virtual ~PreparedStatement();
      // Destructor -

      // Methods +
    public:
      /*!
       * Execute prepared statement
       *
       * \return the result set
       *
       * \throw anch::sql::SqlException any error
       */
      virtual anch::sql::ResultSet* execute() throw(anch::sql::SqlException) = 0;

      /*!
       * Bind 16 bits signed integer value to prepared statement
       *
       * \param idx the prepared statement index parameter
       * \param value the value to bind
       *
       * \throw anch::sql::SqlException any error
       */
      virtual void set(std::size_t idx, int16_t value) throw(anch::sql::SqlException) = 0;

      /*!
       * Bind 16 bits unsigned integer value to prepared statement
       *
       * \param idx the prepared statement index parameter
       * \param value the value to bind
       *
       * \throw anch::sql::SqlException any error
       */
      virtual void set(std::size_t idx, uint16_t value) throw(anch::sql::SqlException) = 0;

      /*!
       * Bind 32 bits signed integer value to prepared statement
       *
       * \param idx the prepared statement index parameter
       * \param value the value to bind
       *
       * \throw anch::sql::SqlException any error
       */
      virtual void set(std::size_t idx, int32_t value) throw(anch::sql::SqlException) = 0;

      /*!
       * Bind 32 bits unsigned integer value to prepared statement
       *
       * \param idx the prepared statement index parameter
       * \param value the value to bind
       *
       * \throw anch::sql::SqlException any error
       */
      virtual void set(std::size_t idx, uint32_t value) throw(anch::sql::SqlException) = 0;

      /*!
       * Bind 64 bits signed integer value to prepared statement
       *
       * \param idx the prepared statement index parameter
       * \param value the value to bind
       *
       * \throw anch::sql::SqlException any error
       */
      virtual void set(std::size_t idx, int64_t value) throw(anch::sql::SqlException) = 0;

      /*!
       * Bind 64 bits unsigned integer value to prepared statement
       *
       * \param idx the prepared statement index parameter
       * \param value the value to bind
       *
       * \throw anch::sql::SqlException any error
       */
      virtual void set(std::size_t idx, uint64_t value) throw(anch::sql::SqlException) = 0;

      /*!
       * Bind string value to prepared statement
       *
       * \param idx the prepared statement index parameter
       * \param value the value to bind
       *
       * \throw anch::sql::SqlException any error
       */
      virtual void set(std::size_t idx, const std::string& value) throw(anch::sql::SqlException) = 0;

    protected:
      /*!
       * Retrieve wildcards positions ('?') in SQL query
       *
       * \param query the SQL query
       *
       * \return the wildcards' position
       */
      std::set<std::size_t> getWildCards(const std::string& query) const;
      // Methods -

    };

  }
}

#endif // _ANCH_SQL_PREPARED_STATEMENT_H_
