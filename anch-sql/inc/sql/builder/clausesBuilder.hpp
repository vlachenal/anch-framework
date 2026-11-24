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
#include <list>
#include <sstream>
#include <functional>
#include <vector>
#include <optional>
#include <span>
#include <string_view>


#include "sql/builder/sqlQuery.hpp"


namespace anch::sql {

  class SelectBuilder;
  class FromBuilder;
  class ClausesBuilder;
  extern ClausesBuilder clauses();

  /*!
   * Clause maker function ( \ref equals , \ref between , ...).\n
   * The first parameter is the column (and the first operand), the second one the names of the clause parameters
   * (generated from the column by \ref ClausesBuilder : one name for each value). It returns the clause.
   */
  using ClauseMaker = std::function<std::string(const std::string&, std::span<const std::string>)>;

  /*! Check if value is valid to add a clause */
  template<typename T>
  using ValueChecker = std::function<std::optional<std::string>(const T&)>;

  /*! Clauses provider */
  using ClausesProvider = std::function<const anch::sql::ClausesBuilder&()>;

  /*!
   * Default value checker: the clause is added when the value is set (an empty \c std::optional skips the clause)
   *
   * \param value the value to check
   *
   * \return the parameter value, \c std::nullopt to skip the clause
   */
  template<anch::sql::Bindable T>
  std::optional<std::string> isValidValue(const T& value);

  /*!
   * Default value checker of strings: an empty string skips the clause
   *
   * \param value the value to check
   *
   * \return the parameter value, \c std::nullopt to skip the clause
   */
  template<>
  ANCH_SQL_EXPORT std::optional<std::string> isValidValue<std::string>(const std::string& value);

  /*!
   * \brief SQL clauses builder
   *
   * This class should be instantiated through utility functions.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class ANCH_SQL_EXPORT ClausesBuilder {
    friend ClausesBuilder anch::sql::clauses();
    friend class anch::sql::SelectBuilder;
    friend class anch::sql::FromBuilder;

    // Attributes +
  private:
    /*! SQL request string buffer */
    std::ostringstream _buffer;

    /*! Clauses' values */
    anch::sql::Parameters _parameters;

    /*! At least one clause has been added to this builder */
    bool _firstClause;
    // Attributes -


    // Constructors +
  private:
    /*!
     * \ref ClausesBuilder default constructor
     */
    ClausesBuilder();

  public:
    /*!
     * \ref ClausesBuilder copy constructor
     *
     * \param other the \ref ClausesBuilder to copy
     */
    ClausesBuilder(const ClausesBuilder& other);

  private:
    /*!
     * \ref ClausesBuilder constructor.\n
     * This constructor will try to add a first clause if value is valid. Value will
     * be validated with \c SQL::isValidValue function.\n
     * This constructor can be used to add 'EXISTS' clause.
     *
     * \tparam T the value type
     *
     * \param clause the clause maker
     * \param value the value
     */
    template<typename T>
    ClausesBuilder(ClauseMaker clause, const T& value);

    // /*!
    //  * \ref ClausesBuilder constructor.\n
    //  * This constructor will try to add a first clause if value is valid. Value will
    //  * be validated with \c SQL::isValidValue function.\n
    //  * This constructor can be used to add 'EXISTS' clause.
    //  *
    //  * \tparam T the value type
    //  *
    //  * \param clause the clause maker
    //  * \param value the value
    //  * \param check the value checker to use
    //  */
    // template<typename T>
    // ClausesBuilder(ClauseMaker clause, const T& value, ValueChecker<T> checker);

    // /*!
    //  * \ref ClausesBuilder constructor.\n
    //  * This constructor will try to add a first clause if value is valid. Value will
    //  * be validated with \c SQL::isValidValue function.\n
    //  * This constructor can be used to add 'EXISTS' clause.
    //  *
    //  * \tparam T the value type
    //  *
    //  * \param clause the clause maker
    //  * \param value the value
    //  */
    // template<typename T>
    // ClausesBuilder(const std::string& column, ClauseMaker clause, const T& value);

    // /*!
    //  * \ref ClausesBuilder constructor.\n
    //  * This constructor will try to add a first clause if value is valid. Value will
    //  * be validated with \c SQL::isValidValue function.\n
    //  * This constructor can be used to add 'EXISTS' clause.
    //  *
    //  * \tparam T the value type
    //  *
    //  * \param clause the clause maker
    //  * \param value the value
    //  * \param check the value checker to use
    //  */
    // template<typename T>
    // ClausesBuilder(const std::string& column, ClauseMaker clause, const T& value, ValueChecker<T> checker);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ClausesBuilder destructor
     */
    virtual ~ClausesBuilder();
    // Destructor -


    // SQL commands +
  public:
    // No check no prepared statement values +
    /*!
     * Add field to clause
     *
     * \param column the column
     *
     * \return \c this
     */
    ClausesBuilder& field(const std::string& column);

    /*!
     * Add \c NOT command
     *
     * \param clauses the clauses to negate
     *
     * \return \c this
     */
    ClausesBuilder& NOT(const std::string& clauses);

    /*!
     * Add <tt>IS NULL</tt> clause
     *
     * \return \c this
     */
    ClausesBuilder& isNull();

    /*!
     * Add <tt>IS NOT NULL</tt> clause
     *
     * \return \c this
     */
    ClausesBuilder& isNotNull();

    /*!
     * Add not equals clause with \c <> operator
     *
     * \return \c this
     */
    ClausesBuilder& notEquals();

    /*!
     * Add equals clause with \c = operator
     *
     * \return \c this
     */
    ClausesBuilder& equals();

    /*!
     * Add <tt>NOT EXISTS</tt> in subquery clause
     *
     * \param query the select query
     *
     * \return \c this
     */
    ClausesBuilder& notExists(const anch::sql::SelectBuilder& query);

    /*!
     * Add \c EXISTS in subquery clause
     *
     * \param query the select query
     *
     * \return \c this
     */
    ClausesBuilder& exists(const anch::sql::SelectBuilder& query);

    /*!
     * Add \c IN clause.\n
     * Due to many database engine limitation about the maximum number of prepared
     * statement per connection, \c IN operator will never be treated with place holder
     * prepared statement values.\n
     * You can format text values with \c SQL utility methods.
     *
     * \tparam T the values' type
     *
     * \param values the values
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& in(const std::vector<T>& values); // \todo list vector ?

    /*!
     * Add <tt>NOT IN</tt> clause.\n
     * Due to many database engine limitation about the maximum number of prepared
     * statement per connection, <tt>NOT IN</tt> operator will never be treated with place holder
     * prepared statement values.\n
     * You can format text values with \c SQL utility methods.
     *
     * \tparam T the values' type
     * \param values the values
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& notIn(const std::vector<T>& values); // \todo list vector ?

    /*!
     * Add \c IN subquery clause.
     *
     * \param query the subquery
     *
     * \return \c this
     */
    ClausesBuilder& in(const anch::sql::SQLQuery& query);

    /*!
     * Add <tt>NOT IN</tt> subquery clause.
     *
     * \param query the subquery
     *
     * \return \c this
     */
    ClausesBuilder& notIn(const anch::sql::SQLQuery& query);

    /*!
     * Add \c IN subquery clause.
     *
     * \param query the subquery
     *
     * \return \c this
     */
    ClausesBuilder& in(const anch::sql::SelectBuilder& query);

    /*!
     * Add <tt>NOT IN</tt> subquery clause.
     *
     * \param query the subquery
     *
     * \return \c this
     */
    ClausesBuilder& notIn(const anch::sql::SelectBuilder& query);

    /*!
     * Add greater clause with \c > operator
     *
     * \return \c this
     */
    ClausesBuilder& greater();

    /*!
     * Add greater or equals clause with \c >= operator
     *
     * \return \c this
     */
    ClausesBuilder& greaterEquals();

    /*!
     * Add lesser clause with \c < operator
     *
     * \return \c this
     */
    ClausesBuilder& lesser();

    /*!
     * Add lesser or equals clause with \c <= operator
     *
     * \return \c this
     */
    ClausesBuilder& lesserEquals();

    /*!
     * Add \c LIKE clause
     *
     * \return \c this
     */
    ClausesBuilder& like();

    /*!
     * Add <tt>NOT LIKE</tt> clause
     *
     * \return \c this
     */
    ClausesBuilder& notLike();

    /*!
     * Add \c BETWEEN clause
     *
     * \return \c this
     */
    ClausesBuilder& between();

    /*!
     * Add <tt>NOT BETWEEN</tt> clause
     *
     * \return \c this
     */
    ClausesBuilder& notBetween();

    /*!
     * Add \c AND to SQL query
     *
     * \return \c this
     */
    ClausesBuilder& AND(); // \todo operator ...

    /*!
     * Add \c OR to SQL query
     *
     * \return \c this
     */
    ClausesBuilder& OR(); // \todo operator ...
    // No check no prepared statement values -

    // Check and add value to prepared statement +
    /*!
     * Add \c AND other clauses if clauses are not \c null or empty.
     *
     * \param clauses the clauses to add
     *
     * \return \c this
     */
    ClausesBuilder& AND(anch::sql::ClausesProvider clauses);

    /*!
     * Add \c AND clause if value is valid
     *
     * \tparam T the value type
     *
     * \param clause the clause maker
     * \param value the value
     * \param checker the value checker to use. Default: \c anch::sql::isValidValue<T>
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& AND(anch::sql::ClauseMaker clause, const T& value, anch::sql::ValueChecker<T> checker = anch::sql::isValidValue<T>);

    /*!
     * Add \c AND clause if value is valid
     *
     * \tparam T the value type
     *
     * \param column the column
     * \param clause the clause maker
     * \param value the value
     * \param checker the value checker to use. Default: \c anch::sql::isValidValue<T>
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& AND(const std::string& column, anch::sql::ClauseMaker clause, const T& value, ValueChecker<T> checker = anch::sql::isValidValue<T>);

    /*!
     * Add \c AND clause if value is valid
     *
     * \tparam T the value type
     *
     * \param column the column
     * \param clause the clause maker
     * \param value1 the first value
     * \param value2 the second value
     * \param checker the value checker to use. Default: \c anch::sql::isValidValue<T>
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& AND(const std::string& column, ClauseMaker clause, const T& value1, const T& value2, ValueChecker<T> checker = anch::sql::isValidValue<T>);

    /*!
     * Add \c OR clause if value is valid
     *
     * \tparam T the value type
     *
     * \param clause the clause maker
     * \param value the value
     * \param checker the value checker to use. Default: \c anch::sql::isValidValue<T>
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& OR(ClauseMaker clause, const T& value, ValueChecker<T> checker = anch::sql::isValidValue<T>);

    /*!
     * Add \c OR clause if value is valid
     *
     * \tparam T the value type
     *
     * \param column the column
     * \param clause the clause maker
     * \param value the value
     * \param checker the value checker to use. Default: \c anch::sql::isValidValue<T>
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& OR(const std::string& column, ClauseMaker clause, const T& value, ValueChecker<T> checker = anch::sql::isValidValue<T>);

    /*!
     * Add \c OR clause if value is valid
     *
     * \tparam T the value type
     *
     * \param column the column
     * \param clause the clause maker
     * \param value1 the first value
     * \param value2 the second value
     * \param checker the value checker to use. Default: \c anch::sql::isValidValue<T>
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& OR(const std::string& column, ClauseMaker clause, const T& value1, const T& value2, ValueChecker<T> checker = anch::sql::isValidValue<T>);

    /*!
     * Add \c AND} other clauses if clauses are not \c NULL or empty.
     *
     * \param clauses the clauses to add
     *
     * \return \c this
     */
    ClausesBuilder& OR(anch::sql::ClausesProvider clauses);
    // Check and add value to prepared statement -

    // Parentheses +
  public:
    /*!
     * Add clauses into parenthesis with AND
     *
     * \param other the clauses to add
     *
     * \return \c this
     */
    ClausesBuilder& AND(const ClausesBuilder& other);

    /*!
     * Add clauses into parenthesis with OR
     *
     * \param other the clauses to add
     *
     * \return \c this
     */
    ClausesBuilder& OR(const ClausesBuilder& other);

    /*!
     * Add clauses into parenthesis with OR
     *
     * \param other the clauses to add
     *
     * \return \c this
     */
    ClausesBuilder& operator ||(const ClausesBuilder& other);
    // Parentheses -
    // SQL commands -


    // Methods +
  private:
    /*!
     * Check and add clauses
     *
     * \param boolAgg the boolean aggregator to use
     * \param other the other clauses to add
     *
     * \return \c this
     */
    ClausesBuilder& checkAndAddClauses(const std::string& boolAgg, const ClausesBuilder& other);

    /*!
     * Add parameter of a clause, named from its column (see \ref ClauseMaker )
     *
     * \param column the column
     * \param suffix the name suffix ( \c _min , \c _max for 2 values clauses)
     * \param value the parameter value
     *
     * \return the parameter name
     */
    std::string addParameter(const std::string& column, std::string_view suffix, anch::sql::ParameterValue value);

    /*!
     * Add boolean clauses aggregator
     *
     * \param boolAgg the aggregator to use
     */
    void addBooleanAggregator(const std::string& boolAgg);

    /*!
     * Check and add clauses
     *
     * \param boolAgg the boolean aggregator to use
     * \param clauses the clauses to add
     *
     * \return \c this
     */
    ClausesBuilder& checkAndAddClauses(const std::string& boolAgg, anch::sql::ClausesProvider clauses);

    /*!
     * Check and add clause
     *
     * \tparam T the value type
     *
     * \param boolAgg the boolean aggregator to use
     * \param column the column (first operand)
     * \param clause the clause type (operator)
     * \param value the value (second operand)
     * \param checker the value checker
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& checkAndAddClause(const std::string& boolAgg, const std::string& column, ClauseMaker clause, const T& value, ValueChecker<T> checker);

    /*!
     * Check and add clause
     *
     * \tparam T the value type
     *
     * \param boolAgg the boolean aggregator to use
     * \param column the column (first operand)
     * \param clause the clause type (operator)
     * \param value1 the first value (second operand)
     * \param value2 the first value (third operand)
     * \param checker the value checker
     *
     * \return \c this
     */
    template<typename T>
    ClausesBuilder& checkAndAddClause(const std::string& boolAgg, const std::string& column, ClauseMaker clause, const T& value1, const T& value2, ValueChecker<T> checker);
    // Methods -

  };

  // Implementations +
  template<anch::sql::Bindable T>
  std::optional<std::string>
  isValidValue(const T& value) {
    return anch::sql::toParameter(value);
  }

  template<typename T>
  ClausesBuilder&
  ClausesBuilder::AND(anch::sql::ClauseMaker clause, const T& value, const anch::sql::ValueChecker<T> checker) {
    return checkAndAddClause("AND", "", clause, value, checker);
  }

  template<typename T>
  ClausesBuilder&
  ClausesBuilder::AND(const std::string& column, anch::sql::ClauseMaker clause, const T& value, ValueChecker<T> checker) {
    return checkAndAddClause("AND", column, clause, value, checker);
  }

  template<typename T>
  ClausesBuilder&
  ClausesBuilder::AND(const std::string& column, ClauseMaker clause, const T& value1, const T& value2, ValueChecker<T> checker) {
    return checkAndAddClause("AND", column, clause, value1, value2, checker);
  }

  template<typename T>
  ClausesBuilder&
  ClausesBuilder::OR(ClauseMaker clause, const T& value, ValueChecker<T> checker) {
    return checkAndAddClause("OR", "", clause, value, checker);
  }

  template<typename T>
  ClausesBuilder&
  ClausesBuilder::OR(const std::string& column, ClauseMaker clause, const T& value, ValueChecker<T> checker) {
    return checkAndAddClause("OR", column, clause, value, checker);
  }

  template<typename T>
  ClausesBuilder&
  ClausesBuilder::OR(const std::string& column, ClauseMaker clause, const T& value1, const T& value2, ValueChecker<T> checker) {
    return checkAndAddClause("OR", column, clause, value1, value2, checker);
  }

  template<typename T>
  ClausesBuilder&
  ClausesBuilder::checkAndAddClause(const std::string& boolAgg, const std::string& column, ClauseMaker clause, const T& value, ValueChecker<T> checker) {
    auto optVal = checker(value);
    if(optVal) {
      addBooleanAggregator(boolAgg);
      const std::string names[] = {addParameter(column, "", std::move(optVal))};
      _buffer << clause(column, names);
      // if(value instanceof Optional<?>) {
      //   // Optional value has already been check at his point
      //   values.add(((Optional<?>)value).get());
      // } else if(value instanceof Collection) { // For (NOT) IN operators
      //   buffer.append(SQL.toSQLList((Collection<?>)value));
      // } else if(value instanceof SelectBuilder) { // For (NOT) EXISTS operators
      //   buffer.append('(').append(value).append(')');
      //   values.addAll(((SelectBuilder)value).values);
      // } else if(value instanceof SQLQuery) { // For (NOT) EXISTS operators
      //   final SQLQuery query = (SQLQuery)value;
      //   buffer.append('(').append(query.getQuery()).append(')');
      //   values.addAll(query.getValues());
      // } else {
      //   values.add(value);
      // }
      _firstClause = false;
    }
    return *this;
  }

  template<typename T>
  ClausesBuilder&
  ClausesBuilder::checkAndAddClause(const std::string& boolAgg, const std::string& column, ClauseMaker clause, const T& value1, const T& value2, ValueChecker<T> checker) {
    auto optVal1 = checker(value1);
    auto optVal2 = checker(value2);
    if(optVal1 && optVal2) {
      addBooleanAggregator(boolAgg);
      const std::string names[] = {addParameter(column, "_min", std::move(optVal1)), addParameter(column, "_max", std::move(optVal2))};
      _buffer << clause(column, names);
      // if(value1 instanceof Optional<?>) {
      //   // Optional value has already been check at his point
      //   values.add(((Optional<?>)value1).get());
      // } else {
      //   values.add(value1);
      // }
      // if(value2 instanceof Optional<?>) {
      //   // Optional value has already been check at his point
      //   values.add(((Optional<?>)value2).get());
      // } else {
      //   values.add(value2);
      // }
      _firstClause = false;
    }
    return *this;
  }
  // Implementations -

}  // anch::sql
