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
#include <string>

#include "sql/connection.hpp"
#include "ut/assert.hpp"

/*!
 * Count rows of a query with parameters
 */
template<typename... T>
int64_t
countRows(anch::sql::Connection& conn, const std::string& query, const T&... values) {
  int64_t count = -1;
  conn.queryMapRow(query, [&count](anch::sql::ResultSet& res) {
    count = res.get<int64_t>(0).value_or(-1);
  }, values...);
  return count;
}

/*!
 * Prepared statement with builder operators (placeholders after '>', 'LIKE', 'BETWEEN' were not found)
 */
inline void
testStmtOperators(anch::sql::Connection& conn) {
  int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id > ? AND id <= ? AND last_name LIKE ? AND id BETWEEN ? AND ? AND id <> ?",
			    0, 4, "%", 1, 4, 5);
  anch::ut::assertThat(count == 4, "Found {} rows instead of 4", count);
}

/*!
 * NULL parameter
 */
inline void
testStmtNull(anch::sql::Connection& conn) {
  uint64_t nbRows = conn.update("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES(?,?,?,?)",
				"NullEmail", "Test", "2016-03-22", std::optional<std::string>());
  anch::ut::assertThat(nbRows == 1, "{} inserted rows", nbRows);
  int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE first_name = ? AND email IS NULL", "NullEmail");
  anch::ut::assertThat(count == 1, "Found {} rows with NULL email instead of 1", count);
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
    conn.prepareStatement("SELECT * FROM T_Unknown WHERE a = ?");
  } catch(const anch::sql::SqlException& e) {
    raised = true;
    anch::ut::assertThat(e.isOK(), "Invalid prepared statement should not be a fatal error: {}", e.what());
  }
  anch::ut::assertTrue(raised, "Invalid prepared statement should raise SqlException");
  anch::ut::assertTrue(conn.isValid(), "SQL errors should not invalidate connection");
  anch::ut::assertThat(countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id <= ?", 4) == 4, "Connection should still work");
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
    countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id = ?", 1);
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
      int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id <= ? AND " + std::to_string(i) + " = " + std::to_string(i), i);
      anch::ut::assertThat(count == i, "Found {} rows instead of {}", count, i);
    }
    anch::ut::assertThat(conn.getCachedStatementCount() == 2, "{} cached statements instead of 2", conn.getCachedStatementCount());
    int64_t count = countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id <= ? AND 1 = 1", 3); // evicted: prepared again
    anch::ut::assertThat(count == 3, "Found {} rows instead of 3", count);
    if(nested) {
      int64_t rows = 0;
      int64_t sum = 0;
      conn.queryMapRow("SELECT id FROM T_Test WHERE id <= ? ORDER BY id", [&conn, &rows, &sum](anch::sql::ResultSet& row) {
	int64_t id = row.get<int64_t>(0).value_or(0);
	++rows;
	sum += countRows(conn, "SELECT COUNT(*) FROM T_Test WHERE id = ? AND " + std::to_string(id) + " = " + std::to_string(id), id);
      }, 4);
      anch::ut::assertThat(rows == 4 && sum == 4, "Nested queries: {} rows and {} sub-query rows instead of 4 and 4", rows, sum);
    }
  } catch(...) {
    conn.setStatementCacheSize(size);
    throw;
  }
  conn.setStatementCacheSize(size);
}
