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

#include <set>
#include <string>

#include "sql/sqlException.hpp"
#include "sql/resultSet.hpp"
#include "sql/types/date.hpp"
#include "sql/types/time.hpp"
#include "sql/types/timestamp.hpp"


namespace anch::sql {

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

    /*! Number of placeholders in SQL query */
    std::size_t _nbPlaceholders;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref PreparedStatement constructor
     */
    PreparedStatement() noexcept;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref PreparedStatement destructor
     */
    virtual ~PreparedStatement() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Execute prepared statement for SELECT statement
     *
     * \return the result set
     *
     * \throw SqlException any error
     */
    virtual ResultSet* executeQuery() = 0;

    /*!
     * Execute prepared statement for update database (INSERT, UPDATE, DELETE)
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error
     */
    virtual uint64_t executeUpdate() = 0;

    /*!
     * Bind 16 bits signed integer value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    void set(std::size_t idx, int16_t value);

    /*!
     * Bind 16 bits unsigned integer value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    void set(std::size_t idx, uint16_t value);

    /*!
     * Bind 32 bits signed integer value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    void set(std::size_t idx, int32_t value);

    /*!
     * Bind 32 bits unsigned integer value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    void set(std::size_t idx, uint32_t value);

    /*!
     * Bind 64 bits signed integer value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    void set(std::size_t idx, int64_t value);

    /*!
     * Bind 64 bits unsigned integer value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    void set(std::size_t idx, uint64_t value);

    /*!
     * Bind string value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    void set(std::size_t idx, const std::string& value);

    /*!
     * Bind character array value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    void set(std::size_t idx, const char* const value);

    /*!
     * Bind SQL date value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    virtual void set(std::size_t idx, const anch::sql::Date& value);

    /*!
     * Bind SQL time value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    virtual void set(std::size_t idx, const anch::sql::Time& value);

    /*!
     * Bind SQL timestamp value to prepared statement
     *
     * \param idx the prepared statement index parameter
     * \param value the value to bind
     *
     * \throw SqlException any error
     */
    virtual void set(std::size_t idx, const anch::sql::Timestamp& value);

  protected:
    /*!
     * Retrieve placeholders positions ('?') in SQL query
     *
     * \param query the SQL query
     *
     * \return the placeholders' position
     */
    std::set<std::size_t> getPlaceholders(const std::string& query) const;

    /*!
     * Check index value
     *
     * \param index the index to check
     *
     * \throw SqlException the index is upper than number of wildcards
     */
    inline void checkIndex(std::size_t index) const {
      if(index > _nbPlaceholders) {
	std::ostringstream oss;
	oss << "Index " << index << " is upper than number of wildcards " << _nbPlaceholders;
	throw SqlException(oss.str(), true);
      }
    }
    // Methods -

  };

}
