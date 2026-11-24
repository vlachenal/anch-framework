/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "anch-sql-export.hpp"

#include <string>

#include "sql/parameters.hpp"


namespace anch::sql {

  /**
   * \brief SQL query
   *
   * This is the result of the builders. It provides:
   * <ul>
   * <li>query to send to database</li>
   * <li>named parameters of the prepared statement ( \c :name , see \ref Connection::query )</li>
   * </ul>
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class ANCH_SQL_EXPORT SQLQuery {

    // Attributes +
  private:
    /*! SQL query */
    std::string _query;

    /*! Prepared statement parameters (named parameters of the query) */
    anch::sql::Parameters _parameters;
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
     * \param parameters the prepared statement parameters (named parameters of the query)
     */
    SQLQuery(const std::string& query, const anch::sql::Parameters& parameters);
    // Constructors -

    // Operators +
  public:
    /*!
     * \ref SQLQuery copy assignment
     *
     * \param other the query to copy
     *
     * \return \c this
     */
    SQLQuery& operator=(const SQLQuery& other) = default;
    // Operators -


    // Accessors +
  public:
    /*!
     * SQL query getter
     *
     * \return the SQL query
     */
    inline const std::string& getQuery() const;

    /*!
     * Prepared statement parameters getter
     *
     * \return the parameters
     */
    inline const anch::sql::Parameters& getParameters() const;
    // Accessors -

  };

  const std::string&
  SQLQuery::getQuery() const {
    return _query;
  }

  const anch::sql::Parameters&
  SQLQuery::getParameters() const {
    return _parameters;
  }

}  // anch::sql
