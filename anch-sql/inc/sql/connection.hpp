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
   * Abstract layer for SQL connections management.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Connection {

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
     * Release connection (used for connections' pool)
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
    // Methods -

    // Accessors +
  public:
    /*!
     * Valid status getter
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
