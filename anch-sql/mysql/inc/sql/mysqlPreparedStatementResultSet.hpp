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
#ifdef ANCH_SQL_MYSQL
#pragma once

#include "anch-mysql-export.hpp"

#include "mysql.h"
#include "sql/resultSet.hpp"

#include <list>
#include <optional>
#include <string>
#include <vector>

#ifndef LIBMARIADB
typedef bool my_bool;
#endif

namespace anch::sql {

  /*!
   * \brief MySQL result set implementation for prepared statement ... shitty API
   *
   * Implements \ref ResultSet for MySQL for prepared statement
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ANCH_MYSQL_EXPORT MySQLPreparedStatementResultSet: public ResultSet {

    // Attributes +
  private:
    /*! MySQL statement */
    MYSQL_STMT* _stmt;

    /*! Columns buffers (bound with a bounded size: truncated values are read again with their real size) */
    std::vector<std::vector<char>> _buffers;

    /*! Columns values of current row (\c std::nullopt for \c NULL ) */
    std::vector<std::optional<std::string>> _values;

    /*! Columns length */
    std::vector<unsigned long> _lengths;

    /*! Columns null flags */
    std::vector<my_bool> _nulls;

    /*! Columns truncation flags */
    std::vector<my_bool> _errors;

    /*! MySQL result binding */
    std::vector<MYSQL_BIND> _binds;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref MySQLPreparedStatementResultSet constructor
     *
     * \param stmt the MySQL statement
     *
     * \throw SqlException any error
     */
    MySQLPreparedStatementResultSet(MYSQL_STMT* stmt);
    // Constructors -

    // Destructor +
    /*!
     * \ref MySQLPreparedStatementResultSet destructor
     */
    virtual ~MySQLPreparedStatementResultSet() noexcept;
    // Destructor -

    // Methods +
  protected:
    /*!
     * Fetch next row in SQL result set.
     *
     * \return \c true if next row exists, \c false otherwise
     *
     * \throw SqlException any error
     */
    virtual bool fetchNext() override;

  protected:
    /*!
     * Retrieve string value from result set according to SQL database engine.
     *
     * \param idx the field index
     * \param out the result
     *
     * \throw SqlException any error
     */
    virtual bool getValue(std::size_t idx, std::string& out) override;

    /*!
     * Retrieve string value from result set according to SQL database engine.
     *
     * \param idx the index
     *
     * \return the result
     *
     * \throw SqlException any error
     */
    virtual std::optional<std::string> getValue(std::size_t idx) override;

    /*!
     * Retrieve SQL date formatter
     *
     * \return the SQL date formatter
     */
    virtual const anch::date::DateFormatter& getDateFormatter() override;

    /*!
     * Retrieve SQL time formatter
     *
     * \return the SQL time formatter
     */
    virtual const anch::date::DateFormatter& getTimeFormatter() override;

    /*!
     * Retrieve SQL timestamp formatter
     *
     * \return the SQL timestamp formatter
     */
    virtual const anch::date::DateFormatter& getTimestampFormatter() override;
    // Methods -

  };

}  // anch::sql

#endif // ANCH_SQL_MYSQL
