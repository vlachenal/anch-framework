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
#pragma once

#include <string>
#include <list>


namespace anch {
  namespace sql {

    /**
     * \brief SQL query
     *
     * This is the result of the builders. It provides:
     * <ul>
     * <li>query to send to database</li>
     * <li>values to add to prepared statement</li>
     * </ul>
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    class SQLQuery {

      // Attributes +
    private:
      /*! SQL query */
      std::string _query;

      /*! Prepared statement values */
      std::list<std::string> _values;
      // Attributes -


      // Constructors +
    public:
      /*!
       * Prohibits \ref SQLQuery default constructor
       */
      SQLQuery() = delete;

      /*!
       * \ref SQLQuery copy constructor
       */
      SQLQuery(const SQLQuery& other);

      /*!
       * \ref SQLQuery constructor
       *
       * \param query the SQL query
       * \param values the prepared statement values
       */
      SQLQuery(const std::string& query, const std::list<std::string>& values);
      // Constructors -


      // Accessors +
    public:
      /*!
       * SQL query getter
       *
       * \return the SQL query
       */
      inline const std::string& getQuery() const;

      /*!
       * Prepared statement values getter
       *
       * \return the values
       */
      inline const std::list<std::string>& getValues() const;
      // Accessors -

    };

    const std::string&
    SQLQuery::getQuery() const {
      return _query;
    }

    const std::list<std::string>&
    SQLQuery::getValues() const {
      return _values;
    }

  }  // sql
}  // anch
