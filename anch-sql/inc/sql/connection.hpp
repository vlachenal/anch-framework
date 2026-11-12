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
#include <functional>

#include "sql/sqlException.hpp"
#include "sql/resultSet.hpp"
#include "sql/preparedStatement.hpp"


namespace anch::sql {

  /*!
   * \brief SQL database connection configuration
   *
   * Parameters can not be used according to SQL database (especially for SQLite3).
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct SqlConnectionConfiguration {
    /*! SQL connection database type */
    std::string driver = "";

    /*! SQL database name */
    std::string database;

    /*! SQL database hostname */
    std::string hostname;

    /*! SQL database port */
    int port;

    /*! SQL database connection user */
    std::string user;

    /*! SQL database user password */
    std::string password;

    /*! Client application */
    std::string application = "AnCH";
  };

  /*!
   * \brief SQL connection virtual class
   *
   * Abstract layer for SQL connections management.\n
   * \n
   * Errors: every \ref SqlException raised by the connection, its prepared statements and its result sets is
   * reported to the connection. A fatal error (\ref SqlException::isOK is \c false : connection lost, out of memory,
   * protocol error, ...) invalidates the connection: \ref isValid returns \c false and connection pools destroy it
   * instead of reusing it. Other errors (syntax, constraint, ...) keep the connection valid, but the current
   * transaction will be rolled back by \ref release .
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Connection {

    friend class PreparedStatement;
    friend class ResultSet;

    // Attributes +
  protected:
    /*! Is SQL connection valid */
    bool _valid;

    /*! Is in SQL transaction */
    bool _transaction;

    /*! Prepared statements */
    std::map<std::string, PreparedStatement*> _stmts;

    /*! Error on current calls */
    bool _errors;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Connection default constructor
     *
     * \throw SqlException any error
     */
    Connection();

    /*!
     * Prohibit \ref Connection copy constructor
     */
    Connection(const Connection&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Connection destrcutor
     */
    virtual ~Connection() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Start SQL transaction.\n
     * If there is a transaction on this database connection, this method does nothing.
     *
     * \throw SqlException any error
     */
    void startTransaction();

    /*!
     * Alias for startTransaction
     *
     * \throw SqlException any error
     */
    void begin();

    /*!
     * Commit current SQL transaction.\n
     * If there is not transaction, this method does nothing.
     *
     * \throw SqlException any error
     */
    void commit();

    /*!
     * Rollback current SQL transaction.\n
     * If there is not transaction, this method does nothing.
     *
     * \throw SqlException any error
     */
    void rollback();

    /*!
     * Release connection: end the current transaction (rollback when an error has been reported since the last
     * release, commit otherwise).\n
     * It is called when the connection is returned to its pool and when it is destroyed. If the transaction can be
     * neither committed nor rolled back, the connection is invalidated (its transaction state is unknown).
     */
    void release() noexcept;

    /*!
     * Prepare SQL statement if not already done.
     *
     * \param query the SQL query
     *
     * \return the prepared statement
     *
     * \throw SqlException any error
     */
    PreparedStatement& prepareStatement(const std::string& query);

    /*!
     * Execute SQL query
     *
     * \param query the SQL query to execute
     *
     * \return the result set which has to be deleted after use
     *
     * \throw SqlException any error
     */
    ResultSet* query(const std::string& query);

    /*!
     * Execute query and treat each row of result set
     *
     * \param sqlQuery the SQL query to execute
     * \param rowMapper the row mapper function
     *
     * \throw SqlException any error
     */
    void queryMapRow(const std::string& sqlQuery, std::function<void(ResultSet&)> rowMapper);

    /*!
     * Execute query and extract result set
     *
     * \param sqlQuery the SQL query to execute
     * \param resExtractor the result set extractor function
     *
     * \throw SqlException any error
     */
    void queryExtract(const std::string& sqlQuery, std::function<void(ResultSet&)> resExtractor);

    /*!
     * Execute SQL prepared statement query
     *
     * \param query the SQL query to execute
     * \param value the first value to bind
     * \param values the other values
     *
     * \return the result set which has to be deleted after use
     *
     * \throw SqlException any error
     */
    template<typename T, typename... Q>
    ResultSet* query(const std::string& query, const T& value, const Q&... values);

    /*!
     * Execute query and treat each row of result set
     *
     * \param query the SQL query to execute
     * \param rowMapper the row mapper function
     * \param value the first value to bind
     * \param values the other values
     *
     * \throw SqlException any error
     */
    template<typename T, typename... Q>
    void queryMapRow(const std::string& query, std::function<void(ResultSet&)> rowMapper, const T& value, const Q&... values);

    /*!
     * Execute query and extract result set
     *
     * \param query the SQL query to execute
     * \param resExtractor the result set extractor function
     * \param value the first value to bind
     * \param values the other values
     *
     * \throw SqlException any error
     */
    template<typename T, typename... Q>
    void queryExtract(const std::string& query, std::function<void(ResultSet&)> resExtractor, const T& value, const Q&... values);

    /*!
     * Execute update (INSERT, UPDATE or DELETE) SQL query.
     *
     * \param query the SQL query
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error
     */
    uint64_t update(const std::string& query);

    /*!
     * Execute update (INSERT, UPDATE or DELETE) SQL prepared statement.
     *
     * \param query the SQL query
     * \param value the first value to bind
     * \param values the other values
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error
     */
    template<typename T, typename... Q>
    uint64_t update(const std::string& query, const T& value, const Q&... values);

    /*!
     * Execute batch prepared statement
     *
     * \param query the SQL query
     * \param mapper the single value prepared statement mapper
     * \param values the values as structure or class
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error
     */
    template<typename T, typename Iterable>
    uint64_t batchUpdate(const std::string& query, std::function<void(PreparedStatement&, const T&)> mapper, const Iterable& values);

  protected:
    /*!
     * Execute SQL select query
     *
     * \param query the SQL query to execute
     *
     * \return the result set which has to be deleted after use
     *
     * \throw SqlException any error
     */
    virtual ResultSet* executeQuery(const std::string& query) = 0;

    /*!
     * Execute SQL update query
     *
     * \param query the SQL query to execute
     *
     * \return the result set which has to be deleted after use
     *
     * \throw SqlException any error
     */
    virtual uint64_t executeUpdate(const std::string& query) = 0;

    /*!
     * Send start transaction to database server
     *
     * \throw SqlException any error
     */
    virtual void sendStartTransaction() = 0;

    /*!
     * Send commit to database server
     *
     * \throw SqlException any error
     */
    virtual void sendCommit() = 0;

    /*!
     * Send rollback to database server
     *
     * \throw SqlException any error
     */
    virtual void sendRollback() = 0;

    /*!
     * Send SQL query to prepare SQL statement
     *
     * \param query the SQL query
     *
     * \return the prepared statement
     *
     * \throw SqlException any error
     */
    virtual PreparedStatement* makePrepared(const std::string& query) = 0;

  private:
    /*!
     * Extract result set and delete it
     *
     * \param res the result set
     * \param resExtractor the result extraction function
     *
     * \throw SqlException any error
     */
    static void extract(ResultSet* res, std::function<void(ResultSet&)> resExtractor);

    /*!
     * Map row from result set and delete it
     *
     * \param res the result set
     * \param rowMapper the result extraction function
     *
     * \throw SqlException any error
     */
    static void mapRow(ResultSet* res, std::function<void(ResultSet&)> rowMapper);

    /*!
     * Bind parameters to prepared statement
     *
     * \param stmt the prepared statement
     * \param idx the current index
     * \param value the value to bind for index
     * \param values the other values to bind
     *
     * \throw SqlException any error
     */
    template<typename T, typename... Q>
    static void bindParameters(PreparedStatement& stmt, std::size_t& idx, const T& value, const Q&... values);

    /*!
     * Bind parameters to prepared statement
     *
     * \param stmt the prepared statement
     * \param idx the current index
     * \param value the value to bind for index
     *
     * \throw SqlException any error
     */
    template<typename T>
    static void bindParameters(PreparedStatement& stmt, std::size_t& idx, const T& value);

  protected:
    /*!
     * Call function and report its \ref SqlException to the connection (see \ref onError )
     *
     * \tparam F the function type
     *
     * \param func the function to call
     *
     * \return the function result
     *
     * \throw SqlException the function error (rethrown)
     *
     * \since 0.2
     */
    template<typename F>
    auto guard(F&& func) -> decltype(func());

    /*!
     * Report error: the current transaction has to be rolled back ; a fatal error invalidates the connection
     *
     * \param error the error
     *
     * \since 0.2
     */
    void onError(const SqlException& error) noexcept;

    /*!
     * Check driver connection status (called by \ref isValid ; default to \c true )
     *
     * \return \c true when the driver connection is usable, \c false otherwise
     *
     * \since 0.2
     */
    virtual bool checkConnection() const noexcept;
    // Methods -

    // Accessors +
  public:
    /*!
     * Valid status getter: \c false once a fatal error has occurred, or when the driver reports a broken
     * connection. Connection pools destroy invalid connections instead of reusing them.
     *
     * \return the status
     */
    bool isValid() const noexcept;

  protected:
    /*!
     * Valid status setter
     *
     * \param valid the status to set
     */
    void setValid(bool valid) noexcept;
    // Accessors -

  };

}

#include "sql/impl/connection.hpp"
