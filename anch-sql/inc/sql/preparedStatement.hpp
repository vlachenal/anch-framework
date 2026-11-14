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

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "sql/sqlException.hpp"
#include "sql/resultSet.hpp"
#include "sql/types/date.hpp"
#include "sql/types/time.hpp"
#include "sql/types/timestamp.hpp"


namespace anch::sql {

  class Connection;

  /*!
   * \brief SQL prepared statement
   *
   * Prepared statements are created and cached by \ref Connection::prepareStatement . Parameters are numbered from
   * \c 1 ; every parameter has to be bound before execution (values are kept between executions).\n
   * The number of parameters is given by the database driver (MySQL and SQLite count them on the server side,
   * PostgreSQL \c '?' placeholders are found with \ref findPlaceholders and replaced by \c $n ).\n
   * Errors are reported to the connection: a fatal error invalidates it (see \ref Connection::isValid ).
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class PreparedStatement {

    friend class Connection;
    friend class ResultSet;

    // Attributes +
  protected:
    /*! Statement values (index \c 0 is parameter \c 1 ; \c std::nullopt for \c NULL ) */
    std::vector<std::optional<std::string>> _values;

    /*! Bound parameters */
    std::vector<bool> _bound;

    /*! Number of placeholders in SQL query */
    std::size_t _nbPlaceholders;

  private:
    /*! Connection which has prepared this statement (errors are reported to it) */
    anch::sql::Connection* _connection;

    /*! Number of alive result sets of this statement (it can not be evicted from cache while they are alive) */
    std::size_t _openResults;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref PreparedStatement constructor
     */
    PreparedStatement() noexcept;

    /*!
     * Prohibits \ref PreparedStatement copy constructor
     */
    PreparedStatement(const PreparedStatement&) = delete;
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
     * \return the result set (to delete)
     *
     * \throw SqlException unbound parameter or any database error
     */
    ResultSet* executeQuery();

    /*!
     * Execute prepared statement for update database (INSERT, UPDATE, DELETE)
     *
     * \return the number of affected rows
     *
     * \throw SqlException unbound parameter or any database error
     */
    uint64_t executeUpdate();

    /*!
     * Bind 16 bits signed integer value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    void set(std::size_t idx, int16_t value);

    /*!
     * Bind 16 bits unsigned integer value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    void set(std::size_t idx, uint16_t value);

    /*!
     * Bind 32 bits signed integer value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    void set(std::size_t idx, int32_t value);

    /*!
     * Bind 32 bits unsigned integer value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    void set(std::size_t idx, uint32_t value);

    /*!
     * Bind 64 bits signed integer value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    void set(std::size_t idx, int64_t value);

    /*!
     * Bind 64 bits unsigned integer value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    void set(std::size_t idx, uint64_t value);

    /*!
     * Bind string value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    void set(std::size_t idx, const std::string& value);

    /*!
     * Bind character array value to prepared statement (\c NULL pointer binds SQL \c NULL )
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    void set(std::size_t idx, const char* const value);

    /*!
     * Bind optional value to prepared statement (\c std::nullopt binds SQL \c NULL )
     *
     * \tparam T the value type
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     *
     * \since 0.2
     */
    template<typename T>
    void set(std::size_t idx, const std::optional<T>& value);

    /*!
     * Bind SQL \c NULL to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     *
     * \throw SqlException invalid index
     *
     * \since 0.2
     */
    void setNull(std::size_t idx);

    /*!
     * Bind SQL date value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    virtual void set(std::size_t idx, const anch::sql::Date& value);

    /*!
     * Bind SQL time value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    virtual void set(std::size_t idx, const anch::sql::Time& value);

    /*!
     * Bind SQL timestamp value to prepared statement
     *
     * \param idx the prepared statement index parameter (from \c 1 )
     * \param value the value to bind
     *
     * \throw SqlException invalid index
     */
    virtual void set(std::size_t idx, const anch::sql::Timestamp& value);

    /*!
     * Unbind every parameter
     *
     * \since 0.2
     */
    void clearParameters() noexcept;

    /*!
     * Number of parameters getter
     *
     * \return the number of parameters
     *
     * \since 0.2
     */
    std::size_t getParameterCount() const noexcept;

  protected:
    /*!
     * Set number of parameters (called by drivers once statement has been prepared)
     *
     * \param count the number of parameters
     */
    void setParameterCount(std::size_t count);

    /*!
     * Execute prepared statement for SELECT statement (driver implementation: parameters are bound)
     *
     * \return the result set
     *
     * \throw SqlException any error (with connection status)
     */
    virtual ResultSet* doExecuteQuery() = 0;

    /*!
     * Execute prepared statement for update (driver implementation: parameters are bound)
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error (with connection status)
     */
    virtual uint64_t doExecuteUpdate() = 0;

    /*!
     * Deallocate statement on the server side (called when the statement is evicted from the connection cache, while
     * the connection is open ; default: nothing to do).\n
     * Drivers which deallocate statements in their destructor (MySQL, SQLite3) do not have to implement it.
     *
     * \since 0.2
     */
    virtual void deallocate() noexcept;

  private:
    /*!
     * Bind value
     *
     * \param idx the parameter index (from \c 1 )
     * \param value the value (\c std::nullopt for \c NULL )
     *
     * \throw SqlException invalid index
     */
    void bind(std::size_t idx, std::optional<std::string> value);

    /*!
     * Check that every parameter has been bound
     *
     * \throw SqlException unbound parameter
     */
    void checkBindings() const;
    // Methods -

  };

  template<typename T>
  void
  PreparedStatement::set(std::size_t idx, const std::optional<T>& value) {
    if(value.has_value()) {
      set(idx, value.value());
    } else {
      setNull(idx);
    }
  }

}
