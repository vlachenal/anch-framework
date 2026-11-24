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

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "sql/sqlException.hpp"
#include "sql/resultSet.hpp"
#include "sql/parameters.hpp"
#include "sql/fields.hpp"
#include "sql/placeholders.hpp"


namespace anch::sql {

  class Connection;

  /*!
   * \brief SQL prepared statement
   *
   * Prepared statements are created and cached by \ref Connection::prepareStatement . Parameters are named
   * ( \c :name , see \ref NamedQuery ): a name used several times in the query is bound once. Every parameter has to
   * be bound before execution (values are kept between executions): the missing ones are reported together.
   * Positional parameters ( \c ? ) are refused.\n
   * Errors are reported to the connection: a fatal error invalidates it (see \ref Connection::isValid ).
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ANCH_SQL_EXPORT PreparedStatement {

    friend class Connection;
    friend class ResultSet;

    // Attributes +
  protected:
    /*! The query (with named parameters) */
    std::string _query;

    /*! Parameters names, in order of first use */
    std::vector<std::string> _names;

    /*! Value of each name ( \c std::nullopt for \c NULL ; PostgreSQL array literal of a list) */
    std::vector<anch::sql::ParameterValue> _values;

    /*! Values of each name bound to a list */
    std::vector<std::optional<anch::sql::ParameterList>> _lists;

    /*! Bound names */
    std::vector<bool> _bound;

    /*! Name of each parameter of the query prepared by the driver ( \c $1 , \c ?1 or each \c ? ) */
    std::vector<anch::sql::NamedQuery::Binding> _bindings;

    /*! The parsed query */
    std::optional<anch::sql::NamedQuery> _named;

    /*! Parameters style of the driver */
    anch::sql::ParameterStyle _style;

    /*! The query prepared by the driver */
    std::string _prepared;

    /*! List size of each name of the prepared query ( \c NamedQuery::SCALAR for a value which is not a list) */
    std::vector<std::size_t> _shape;

  private:
    /*! Connection which has prepared this statement (errors are reported to it) */
    anch::sql::Connection* _connection;

    /*! Number of alive result sets of this statement (it can not be evicted from cache while they are alive) */
    std::size_t _openResults;

    /*! Statement has been invalidated by the database: it is prepared again by \ref Connection::prepareStatement */
    bool _invalid;
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
     * Bind value to a parameter ( \c toParameter : integers, floating point numbers, strings, dates (\c anch::date::Date , \c anch::date::DateTime , \c anch::date::Time ),
     * \c std::optional of them ; \c std::nullopt and \c NULL character arrays bind SQL \c NULL ), or a list of
     * values ( \ref BindableList : PostgreSQL array, expanded \c IN (:name) list for other databases ; the query is
     * prepared again when the list sizes change its text)
     *
     * \tparam T the value type
     *
     * \param name the parameter name (without \c ':' )
     * \param value the value to bind
     *
     * \throw SqlException unknown parameter (non fatal, with the parameters of the query)
     *
     * \since 0.3
     */
    template<anch::sql::Bindable T>
    void set(std::string_view name, const T& value);

    /*!
     * Bind SQL \c NULL to a parameter
     *
     * \param name the parameter name (without \c ':' )
     *
     * \throw SqlException unknown parameter (non fatal)
     *
     * \since 0.3
     */
    void setNull(std::string_view name);

    /*!
     * Bind parameters (the parameters of the query which are not in \c parameters are unbound)
     *
     * \param parameters the parameters
     *
     * \throw SqlException unknown parameter (non fatal)
     *
     * \since 0.3
     */
    void set(const anch::sql::Parameters& parameters);

    /*!
     * Bind the fields of an object to the parameters of the same names (the other fields are ignored, the parameters
     * which are not fields are unbound)
     *
     * \tparam T the described type
     *
     * \param fields the fields of the object ( \ref fieldsOf )
     *
     * \since 0.3
     */
    template<anch::Described T>
    void set(const anch::sql::FieldsOf<T>& fields);

    /*!
     * Unbind every parameter
     *
     * \since 0.2
     */
    void clearParameters() noexcept;

    /*!
     * Number of parameters getter
     *
     * \return the number of parameters names
     *
     * \since 0.2
     */
    std::size_t getParameterCount() const noexcept;

    /*!
     * Parameters names getter
     *
     * \return the parameters names, in order of first use
     *
     * \since 0.3
     */
    const std::vector<std::string>& getParameterNames() const noexcept;

    /*!
     * Query getter
     *
     * \return the query (with named parameters)
     *
     * \since 0.3
     */
    const std::string& getQuery() const noexcept;

  protected:
    /*!
     * Parse query and render it for the driver (called by drivers before preparing the statement)
     *
     * \param query the query (with named parameters)
     * \param syntax the SQL dialect lexical rules
     * \param style the parameters style of the driver
     *
     * \return the query to prepare
     *
     * \throw SqlException positional parameter in query (non fatal)
     */
    std::string parse(const std::string& query, const anch::sql::SqlSyntax& syntax, anch::sql::ParameterStyle style);

    /*!
     * Prepare the query rendered for the driver (driver implementation: called by its constructor with the query given
     * by \ref parse , and before execution when bound lists change the rendered query: the new query replaces the
     * previous one)
     *
     * \param query the query to prepare
     *
     * \throw SqlException any error (with connection status)
     */
    virtual void doPrepare(const std::string& query) = 0;

  public:
    /*!
     * Number of parameters of the query prepared by the driver ( \c $1 , \c ?1 or each \c ? )
     *
     * \return the number of parameters
     */
    std::size_t getBindingCount() const noexcept;

    /*!
     * Value of a parameter of the query prepared by the driver
     *
     * \param idx the parameter index (from \c 0 : \c $1 , \c ?1 or the first \c ? ; a name used several times by
     *        MySQL gives the same value)
     *
     * \return the value ( \c std::nullopt for \c NULL )
     */
    const anch::sql::ParameterValue& getBindingValue(std::size_t idx) const;

  protected:
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

    /*!
     * Invalidate statement: the database can not execute it anymore (PostgreSQL: result type changed by a schema
     * change, statement deallocated by \c DISCARD \c ALL , ...). \ref Connection::prepareStatement prepares it
     * again.
     *
     * \since 0.3
     */
    void invalidate() noexcept;

  private:
    /*!
     * Bind value
     *
     * \param name the parameter name
     * \param value the value (\c std::nullopt for \c NULL )
     *
     * \throw SqlException unknown parameter
     */
    void bind(std::string_view name, anch::sql::BoundValue value);

    /*!
     * Render and prepare the query again when the bound lists change it (see \ref doPrepare )
     *
     * \throw SqlException list which is not the only item of an \c IN list (non fatal), driver error
     */
    void reshape();

    /*!
     * Check that every parameter has been bound
     *
     * \throw SqlException unbound parameter
     */
    void checkBindings() const;
    // Methods -

  };

  template<anch::sql::Bindable T>
  void
  PreparedStatement::set(std::string_view name, const T& value) {
    bind(name, anch::sql::toBound(value));
  }

  template<anch::Described T>
  void
  PreparedStatement::set(const anch::sql::FieldsOf<T>& fields) {
    clearParameters();
    const anch::sql::FieldsMapper<T>& mapper = anch::sql::FieldsMapper<T>::instance();
    for(std::size_t slot = 0 ; slot < _names.size() ; ++slot) {
      const typename anch::sql::FieldsMapper<T>::Field* field = mapper.find(_names[slot]);
      if(field != nullptr) {
	anch::sql::BoundValue bound = field->read(fields.object());
	_values[slot] = std::move(bound.value);
	_lists[slot] = std::move(bound.list);
	_bound[slot] = true;
      }
    }
  }

}
