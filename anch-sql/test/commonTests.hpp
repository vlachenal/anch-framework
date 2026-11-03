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
