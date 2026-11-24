/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "date/date.hpp"
#include "mapping.hpp"
#include "sql/connection.hpp"
#include "sql/builder/clauses.hpp"
#include "sql/builder/sqlFunctions.hpp"
#include "ut/assert.hpp"

/*!
 * Count rows of a query with parameters
 */
inline int64_t
countRows(anch::sql::Connection& conn, const std::string& query, const anch::sql::Parameters& parameters = {}) {
  int64_t count = -1;
  conn.queryMapRow(query, [&count](anch::sql::ResultSet& res) {
    count = res.get<int64_t>(0).value_or(-1);
  }, parameters);
  return count;
}

/*!
 * Prepared statement with builder operators (placeholders after '>', 'LIKE', 'BETWEEN' were not found)
 */
inline void
testStmtOperators(anch::sql::Connection& conn) {
  int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id > :min AND id <= :max AND last_name LIKE :name"
			    " AND id BETWEEN :low AND :max AND id <> :other",
			    {{"min", 0}, {"max", 4}, {"name", "%"}, {"low", 1}, {"other", 5}});
  anch::ut::assertThat(count == 4, "Found {} rows instead of 4", count);
}

/*!
 * NULL parameter
 */
inline void
testStmtNull(anch::sql::Connection& conn) {
  uint64_t nbRows = conn.update("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES(:first,:last,:birth,:email)",
				{{"first", "NullEmail"}, {"last", "Test"}, {"birth", "2016-03-22"}, {"email", std::optional<std::string>()}});
  anch::ut::assertThat(nbRows == 1, "{} inserted rows", nbRows);
  int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE first_name = :first AND email IS NULL", {{"first", "NullEmail"}});
  anch::ut::assertThat(count == 1, "Found {} rows with NULL email instead of 1", count);
}

/*!
 * Named parameters: a name used several times is bound once (PostgreSQL infers the type of a parameter from its first
 * use: 'column = :name' before ':name IS NULL') (MySQL: each '?' points to the same value), missing and
 * unknown parameters are reported with their names, positional parameters are refused
 *
 * \param conn the connection
 * \param positional a query with a positional parameter of the database ('?' is a jsonb operator in PostgreSQL: '$1')
 */
inline void
testStmtNamed(anch::sql::Connection& conn, const std::string& positional = "SELECT COUNT(*) FROM T_Test WHERE id = ?") {
  int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE (last_name = :name OR :name IS NULL) AND (id >= :id OR :name IS NULL)",
			    {{"id", 1}, {"name", std::optional<std::string>()}});
  anch::ut::assertThat(count >= 4, "Found {} rows with NULL name", count);
  count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE (id = :id OR id = :id + 1) AND (last_name <> :name OR :name IS NULL)",
		    {{"id", 1}, {"name", "Unknown"}});
  anch::ut::assertThat(count == 2, "Found {} rows instead of 2", count);
  auto expectMessage = [](std::function<void()> func, const std::string& expected) {
    try {
      func();
    } catch(const anch::sql::SqlException& e) {
      anch::ut::assertThat(std::string(e.what()).find(expected) != std::string::npos && e.isOK(), "Unexpected error '{}'", e.what());
      return;
    }
    anch::ut::fail("Error '" + expected + "' expected");
  };
  const std::string query = "SELECT COUNT(*) FROM T_Test WHERE id = :id AND last_name = :name AND email = :email";
  expectMessage([&conn, &query]() { countRows(conn, query, {{"id", 1}}); }, "parameters 'name', 'email' have not been bound");
  expectMessage([&conn, &query]() { countRows(conn, query, {{"id", 1}, {"mail", "x"}}); }, "no parameter 'mail' in query (parameters: id, name, email)");
  expectMessage([&conn, &positional]() { countRows(conn, positional); }, "positional parameters (");
  anch::ut::assertTrue(conn.isValid(), "Parameters errors should not invalidate connection");
}

/*!
 * Query built with named parameters (generated from columns) is executed
 */
inline void
testBuilderQuery(anch::sql::Connection& conn) {
  std::optional<std::string> email;
  anch::sql::SQLQuery query = anch::sql::select()
    .field("COUNT(*)")
    .from("T_Test")
    .where(anch::sql::clauses()
	   .AND("id", anch::sql::greater, 0)
	   .AND("id", anch::sql::between, 1, 4)
	   .AND("email", anch::sql::equals, email) // skipped
	   .AND(anch::sql::clauses().AND("last_name", anch::sql::like, std::string("%")).OR("id", anch::sql::lesser, 1)))
    .build();
  int64_t count = countRows(conn, query.getQuery(), query.getParameters());
  anch::ut::assertThat(count == 4, "Found {} rows instead of 4 with query '{}'", count, query.getQuery());
}

namespace sqltest {

  /*! Described type (anch::Mapping) bound with fieldsOf */
  struct Person {
    std::string firstName;
    std::string lastName;
    std::string birthDate;
    std::optional<std::string> email;
  };

}

template<>
struct anch::Mapping<sqltest::Person> {
  static void
  registerObject(anch::ObjectMapper<sqltest::Person> auto& mapper) {
    mapper
      .registerField("first_name", &sqltest::Person::firstName)
      .registerField("last_name", &sqltest::Person::lastName)
      .registerField("birth_date", &sqltest::Person::birthDate)
      .registerField("email", &sqltest::Person::email)
      ;
  }
};

/*!
 * Fields of described objects as parameters (fieldsOf, batchUpdate of objects)
 */
inline void
testStmtFields(anch::sql::Connection& conn) {
  const std::string insert = "INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES (:first_name,:last_name,:birth_date,:email)";
  sqltest::Person jane{"Jane", "Fields", "1971-01-01", std::nullopt};
  uint64_t nbRows = conn.update(insert, anch::sql::fieldsOf(jane));
  anch::ut::assertThat(nbRows == 1, "{} inserted rows", nbRows);
  std::vector<sqltest::Person> persons{{"Jack", "Fields", "1972-01-01", "jack@anch.org"}, {"Joe", "Fields", "1973-01-01", std::nullopt}};
  nbRows = conn.batchUpdate(insert, persons);
  anch::ut::assertThat(nbRows == 2, "{} batch inserted rows", nbRows);
  // Fields which are not parameters (first name, birth date, email) are ignored
  int64_t count = -1;
  conn.queryMapRow("SELECT COUNT(*) FROM T_Test WHERE last_name = :last_name", [&count](anch::sql::ResultSet& res) {
    count = res.get<int64_t>(0).value_or(-1);
  }, anch::sql::fieldsOf(jane));
  anch::ut::assertThat(count == 3, "Found {} persons instead of 3", count);
  count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE last_name = :last AND email IS NULL", {{"last", "Fields"}});
  anch::ut::assertThat(count == 2, "Found {} persons without email instead of 2", count);
}

/*!
 * Lists: PostgreSQL arrays (IN rewritten in = ANY / <> ALL), expanded IN lists for other databases ; the statement is
 * prepared again when the bound lists change the query
 */
inline void
testStmtLists(anch::sql::Connection& conn) {
  const int64_t total = countRows(conn, "SELECT COUNT(*) FROM T_Test");
  const std::string in = "SELECT COUNT(*) FROM T_Test WHERE id IN (:ids)";
  const std::string notIn = "SELECT COUNT(*) FROM T_Test WHERE id NOT IN (:ids)";
  int64_t count = countRows(conn, in, {{"ids", std::vector<int64_t>{1, 2, 3}}});
  anch::ut::assertThat(count == 3, "IN: found {} rows instead of 3", count);
  count = countRows(conn, in, {{"ids", std::vector<int64_t>{1, 2}}});
  anch::ut::assertThat(count == 2, "IN (other size): found {} rows instead of 2", count);
  count = countRows(conn, in, {{"ids", std::vector<int64_t>()}});
  anch::ut::assertThat(count == 0, "IN (empty list): found {} rows instead of 0", count);
  count = countRows(conn, in, {{"ids", 1}});
  anch::ut::assertThat(count == 1, "IN (value): found {} rows instead of 1", count);
  count = countRows(conn, notIn, {{"ids", std::vector<int64_t>{1, 2}}});
  anch::ut::assertThat(count == total - 2, "NOT IN: found {} rows instead of {}", count, total - 2);
  count = countRows(conn, notIn, {{"ids", std::vector<int64_t>()}});
  anch::ut::assertThat(count == total, "NOT IN (empty list): found {} rows instead of {}", count, total);
  // Strings (quoted in PostgreSQL arrays) and builder
  conn.update("INSERT INTO T_Test (first_name,last_name,birth_date) VALUES (:first,:last,'2016-03-22')",
	      {{"first", "List\"Quote"}, {"last", "Back\\slash"}});
  count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE first_name IN (:names) AND last_name IN (:last)",
		    {{"names", std::set<std::string>{"List\"Quote", "Nobody"}}, {"last", std::vector<std::string>{"Back\\slash"}}});
  anch::ut::assertThat(count == 1, "IN (strings): found {} rows instead of 1", count);
  anch::sql::SQLQuery query = anch::sql::select().field("COUNT(*)").from("T_Test")
    .where(anch::sql::clauses().AND("id", anch::sql::in, std::vector<int32_t>{1, 2, 3, 4})).build();
  count = countRows(conn, query.getQuery(), query.getParameters());
  anch::ut::assertThat(count == 4, "Builder IN: found {} rows instead of 4", count);
}

/*!
 * Dates: DATE columns read as anch::date::Date (calendar days, years 0 and 1 included), bound as parameters and lists
 *
 * \param conn the connection
 * \param christ the birth year of 'Christ' in the database fixture (0 with MySQL, 1 otherwise)
 */
inline void
testStmtDates(anch::sql::Connection& conn, int32_t christ = 1) {
  std::optional<anch::date::Date> birth;
  conn.queryMapRow("SELECT birth_date FROM T_Test WHERE last_name = :last", [&birth](anch::sql::ResultSet& res) {
    birth = res.get<anch::date::Date>(0);
  }, {{"last", "Christ"}});
  anch::ut::assertThat(birth == anch::date::Date(christ, 12, 24), "Unexpected birth date {}", birth ? birth->format() : std::string("NULL"));
  int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE birth_date = :birth", {{"birth", anch::date::Date(1993, 1, 1)}});
  anch::ut::assertThat(count == 1, "Found {} rows born on 1993-01-01 instead of 1", count);
  count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE birth_date IN (:days)",
		    {{"days", std::vector<anch::date::Date>{anch::date::Date(1970, 1, 1), anch::date::Date(1993, 1, 1)}}});
  anch::ut::assertThat(count == 2, "Found {} rows born on 1970-01-01 or 1993-01-01 instead of 2", count);
}

/*!
 * SQL error (not fatal) keeps the connection valid
 */
inline void
testSqlErrorKeepsConnection(anch::sql::Connection& conn) {
  bool raised = false;
  try {
    conn.update("INSERT INTO T_Unknown (a) VALUES (1)");
  } catch(const anch::sql::SqlException& e) {
    raised = true;
    anch::ut::assertThat(e.isOK(), "Unknown table should not be a fatal error: {}", e.what());
  }
  anch::ut::assertTrue(raised, "Unknown table should raise SqlException");
  raised = false;
  try {
    conn.prepareStatement("SELECT * FROM T_Unknown WHERE a = :a");
  } catch(const anch::sql::SqlException& e) {
    raised = true;
    anch::ut::assertThat(e.isOK(), "Invalid prepared statement should not be a fatal error: {}", e.what());
  }
  anch::ut::assertTrue(raised, "Invalid prepared statement should raise SqlException");
  anch::ut::assertTrue(conn.isValid(), "SQL errors should not invalidate connection");
  anch::ut::assertThat(countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id <= :id", {{"id", 4}}) == 4, "Connection should still work");
}

/*!
 * Kill the connection on the server side: it has to be invalidated
 *
 * \param conn a connection which is not used by other tests
 * \param kill the function which kills the connection
 */
inline void
testFatalError(anch::sql::Connection& conn, std::function<void(anch::sql::Connection&)> kill) {
  anch::ut::assertTrue(conn.isValid(), "Connection should be valid");
  try {
    kill(conn);
  } catch(const anch::sql::SqlException& e) {
    std::cout << "Kill: " << e.what() << std::endl;
  }
  try {
    countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id = :id", {{"id", 1}});
  } catch(const anch::sql::SqlException& e) {
    std::cout << "Query after kill: " << e.what() << std::endl;
  }
  anch::ut::assertFalse(conn.isValid(), "Killed connection should be invalid");
}

/*!
 * Bounded prepared statement cache: evicted statements are prepared again
 *
 * \param conn the connection (its cache size is restored)
 * \param nested check that the outer statement of nested queries is not evicted while its rows are read (only for
 *        drivers which can run a query while reading another one's rows)
 */
inline void
testStatementCache(anch::sql::Connection& conn, bool nested) {
  std::size_t size = conn.getStatementCacheSize();
  conn.setStatementCacheSize(2);
  try {
    for(int i = 1 ; i <= 4 ; ++i) { // distinct queries: each one evicts the least recently used statement
      int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id <= :id AND " + std::to_string(i) + " = " + std::to_string(i), {{"id", i}});
      anch::ut::assertThat(count == i, "Found {} rows instead of {}", count, i);
    }
    anch::ut::assertThat(conn.getCachedStatementCount() == 2, "{} cached statements instead of 2", conn.getCachedStatementCount());
    int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id <= :id AND 1 = 1", {{"id", 3}}); // evicted: prepared again
    anch::ut::assertThat(count == 3, "Found {} rows instead of 3", count);
    if(nested) {
      int64_t rows = 0;
      int64_t sum = 0;
      conn.queryMapRow("SELECT id FROM T_Test WHERE id <= :id ORDER BY id", [&conn, &rows, &sum](anch::sql::ResultSet& row) {
	int64_t id = row.get<int64_t>(0).value_or(0);
	++rows;
	sum += countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id = :id AND " + std::to_string(id) + " = " + std::to_string(id), {{"id", id}});
      }, {{"id", 4}});
      anch::ut::assertThat(rows == 4 && sum == 4, "Nested queries: {} rows and {} sub-query rows instead of 4 and 4", rows, sum);
      // Same SQL text in nested query: the outer statement is not executed again (its cursor would be reset)
      rows = 0;
      sum = 0;
      conn.queryMapRow("SELECT id FROM T_Test WHERE id <= :id ORDER BY id", [&conn, &rows, &sum](anch::sql::ResultSet& row) {
	++rows;
	int64_t id = row.get<int64_t>(0).value_or(0);
	conn.queryMapRow("SELECT id FROM T_Test WHERE id <= :id ORDER BY id", [&sum](anch::sql::ResultSet&) { ++sum; }, {{"id", id}});
      }, {{"id", 4}});
      anch::ut::assertThat(rows == 4 && sum == 10, "Nested same query: {} rows and {} sub-query rows instead of 4 and 10", rows, sum);
    }
  } catch(...) {
    conn.setStatementCacheSize(size);
    throw;
  }
  conn.setStatementCacheSize(size);
}
