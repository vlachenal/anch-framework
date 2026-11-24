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
#include <cstddef>
#include <functional>
#include <list>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>

#include "sql/sqlException.hpp"
#include "sql/resultSet.hpp"
#include "sql/preparedStatement.hpp"


namespace anch::sql {

  /*!
   * Default maximum number of prepared statements cached by a connection (see \ref Connection::prepareStatement )
   *
   * \since 0.2
   */
  inline constexpr std::size_t DEFAULT_STATEMENT_CACHE_SIZE = 256;

  /*!
   * \brief SQL database connection configuration
   *
   * Parameters can not be used according to SQL database (especially for SQLite3).
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  struct ANCH_SQL_EXPORT SqlConnectionConfiguration {
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

    /*!
     * Maximum number of prepared statements cached by each connection (at least \c 1 ; key
     * \c statement-cache-size in configuration)
     */
    std::size_t statementCacheSize = DEFAULT_STATEMENT_CACHE_SIZE;
  };

  /*!
   * \brief SQL connection virtual class
   *
   * Abstract layer for SQL connections management.\n
   * \n
   * Errors: every \ref SqlException raised by the connection, its prepared statements and its result sets is
   * reported to the connection. A fatal error (\ref SqlException::isOK is \c false : connection lost, out of memory,
   * protocol error, ...) invalidates the connection: \ref isValid returns \c false and connection pools destroy it
   * instead of reusing it. Other errors (syntax, constraint, ...) keep the connection valid.\n
   * \n
   * Transactions: without \ref startTransaction , each query is committed by the database (auto-commit mode). A
   * transaction started with \ref startTransaction is committed only by \ref commit : a transaction which is not
   * ended when the connection is released (returned to its pool or destroyed) is rolled back, with or without error.\n
   * \n
   * Prepared statements: \ref prepareStatement keeps the prepared statements in a bounded cache (least recently used
   * statement first evicted, see \ref setStatementCacheSize ). A statement is never evicted while one of its result
   * sets is alive: the cache can then temporarily exceed its maximum size. An evicted statement is deallocated on the
   * server side. A statement is not executed again while one of its result sets is alive (nested queries with the
   * same SQL text): an other statement is prepared for the same query. A statement invalidated by the database
   * (PostgreSQL: result type changed by a schema change) is prepared again.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ANCH_SQL_EXPORT Connection {

    friend class PreparedStatement;
    friend class ResultSet;

    // Attributes +
  protected:
    /*! Is SQL connection valid */
    bool _valid;

    /*! Is in SQL transaction */
    bool _transaction;

    /*! Prepared statement cache entry */
    struct CachedStatement {
      /*! SQL query (cache key) */
      std::string query;
      /*! Prepared statement */
      PreparedStatement* statement;
    };

    /*! Prepared statements (the most recently used first) */
    std::list<CachedStatement> _stmts;

    /*! Prepared statements index (keys are views of \ref CachedStatement::query ) */
    std::unordered_map<std::string_view, std::list<CachedStatement>::iterator> _stmtsIndex;

    /*! Maximum number of cached prepared statements */
    std::size_t _stmtsMax;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Connection constructor
     *
     * \param statementCacheSize the maximum number of cached prepared statements (at least \c 1 )
     *
     * \throw SqlException invalid cache size
     */
    explicit Connection(std::size_t statementCacheSize = DEFAULT_STATEMENT_CACHE_SIZE);

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
     * If there is a transaction on this database connection, this method does nothing.\n
     * The transaction has to be ended with \ref commit or \ref rollback : it is rolled back by \ref release
     * otherwise.
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
     * Release connection: roll back the current transaction, if any (a transaction is committed only by
     * \ref commit ).\n
     * It is called when the connection is returned to its pool and when it is destroyed. If the transaction can be
     * neither committed nor rolled back, the connection is invalidated (its transaction state is unknown).
     */
    void release() noexcept;

    /*!
     * Prepare SQL statement if not already done.\n
     * The statement is cached by the connection (see \ref setStatementCacheSize ): the returned reference remains
     * valid until the statement is evicted, that is to say at least until \c statementCacheSize - 1 other queries
     * have been prepared on this connection (and as long as one of its result sets is alive).
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
     * \param query the SQL query to execute (with named parameters: \c :name )
     * \param parameters the parameters (every parameter of the query has to be given)
     *
     * \return the result set which has to be deleted after use
     *
     * \throw SqlException any error (missing or unknown parameter: non fatal)
     */
    ResultSet* query(const std::string& query, const anch::sql::Parameters& parameters);

    /*!
     * Execute query and treat each row of result set
     *
     * \param query the SQL query to execute (with named parameters: \c :name )
     * \param rowMapper the row mapper function
     * \param parameters the parameters (every parameter of the query has to be given)
     *
     * \throw SqlException any error
     */
    void queryMapRow(const std::string& query, std::function<void(ResultSet&)> rowMapper, const anch::sql::Parameters& parameters);

    /*!
     * Execute query and extract result set
     *
     * \param query the SQL query to execute (with named parameters: \c :name )
     * \param resExtractor the result set extractor function
     * \param parameters the parameters (every parameter of the query has to be given)
     *
     * \throw SqlException any error
     */
    void queryExtract(const std::string& query, std::function<void(ResultSet&)> resExtractor, const anch::sql::Parameters& parameters);

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
     * \param query the SQL query (with named parameters: \c :name )
     * \param parameters the parameters (every parameter of the query has to be given)
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error
     */
    uint64_t update(const std::string& query, const anch::sql::Parameters& parameters);

    /*!
     * Execute batch prepared statement
     *
     * \param query the SQL query (with named parameters: \c :name )
     * \param mapper the function which gives the parameters of a value (every parameter of the query has to be given
     *        for each value: parameters of the previous value are not reused)
     * \param values the values as structure or class
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error
     */
    template<typename T, typename Iterable>
    uint64_t batchUpdate(const std::string& query, std::function<void(anch::sql::Parameters&, const T&)> mapper, const Iterable& values);

    /*!
     * Execute batch prepared statement with described objects: the parameters of each object are its fields
     * ( \ref fieldsOf )
     *
     * \param query the SQL query (with named parameters: \c :name )
     * \param values the described objects
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error (a parameter which is not a field is not bound: non fatal)
     *
     * \since 0.3
     */
    template<std::ranges::input_range Iterable>
    requires anch::Described<std::ranges::range_value_t<Iterable>>
    uint64_t batchUpdate(const std::string& query, const Iterable& values);

    /*!
     * Execute SQL prepared statement query with the fields of an object as parameters ( \ref fieldsOf )
     *
     * \param query the SQL query to execute (with named parameters: \c :name )
     * \param fields the fields of the object
     *
     * \return the result set which has to be deleted after use
     *
     * \throw SqlException any error (a parameter which is not a field is not bound: non fatal)
     *
     * \since 0.3
     */
    template<anch::Described T>
    ResultSet* query(const std::string& query, const anch::sql::FieldsOf<T>& fields);

    /*!
     * Execute query with the fields of an object as parameters and treat each row of result set
     *
     * \param query the SQL query to execute (with named parameters: \c :name )
     * \param rowMapper the row mapper function
     * \param fields the fields of the object ( \ref fieldsOf )
     *
     * \throw SqlException any error
     *
     * \since 0.3
     */
    template<anch::Described T>
    void queryMapRow(const std::string& query, std::function<void(ResultSet&)> rowMapper, const anch::sql::FieldsOf<T>& fields);

    /*!
     * Execute query with the fields of an object as parameters and extract result set
     *
     * \param query the SQL query to execute (with named parameters: \c :name )
     * \param resExtractor the result set extractor function
     * \param fields the fields of the object ( \ref fieldsOf )
     *
     * \throw SqlException any error
     *
     * \since 0.3
     */
    template<anch::Described T>
    void queryExtract(const std::string& query, std::function<void(ResultSet&)> resExtractor, const anch::sql::FieldsOf<T>& fields);

    /*!
     * Execute update (INSERT, UPDATE or DELETE) SQL prepared statement with the fields of an object as parameters
     *
     * \param query the SQL query (with named parameters: \c :name )
     * \param fields the fields of the object ( \ref fieldsOf )
     *
     * \return the number of affected rows
     *
     * \throw SqlException any error
     *
     * \since 0.3
     */
    template<anch::Described T>
    uint64_t update(const std::string& query, const anch::sql::FieldsOf<T>& fields);

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
     * Report error: a fatal error invalidates the connection
     *
     * \param error the error
     *
     * \since 0.2
     */
    void onError(const SqlException& error) noexcept;

    /*!
     * Delete every cached prepared statement without deallocating it on the server side.\n
     * Drivers which have to release statements before closing their connection call it in their destructor.
     *
     * \since 0.2
     */
    void clearStatements() noexcept;

  private:
    /*!
     * Evict the least recently used prepared statements while the cache exceeds its maximum size (the most recently
     * used statement and the statements with alive result sets are kept)
     */
    void evictStatements() noexcept;

  protected:
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
     * Maximum number of cached prepared statements getter
     *
     * \return the maximum number of cached prepared statements
     *
     * \since 0.2
     */
    std::size_t getStatementCacheSize() const noexcept;

    /*!
     * Maximum number of cached prepared statements setter: least recently used statements are evicted when the cache
     * exceeds the new size
     *
     * \param size the maximum number of cached prepared statements (at least \c 1 )
     *
     * \throw SqlException invalid size
     *
     * \since 0.2
     */
    void setStatementCacheSize(std::size_t size);

    /*!
     * Number of cached prepared statements getter
     *
     * \return the number of cached prepared statements
     *
     * \since 0.2
     */
    std::size_t getCachedStatementCount() const noexcept;

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
