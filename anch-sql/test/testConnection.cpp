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
#include "sql/connection.hpp"
#include "sql/placeholders.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "resourcePool.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::Connection;
using anch::sql::PreparedStatement;
using anch::sql::ResultSet;
using anch::sql::SqlException;

/*! Failure to raise by mock operations */
enum class Failure {
  NONE,
  SQL,   // non fatal error (syntax, constraint, ...)
  FATAL  // connection lost
};

/*! Calls recorded by mock connections */
struct Calls {
  int created = 0;
  int commits = 0;
  int rollbacks = 0;
  int prepared = 0;
  int deallocated = 0;
  int deleted = 0;
};
Calls calls;

/*!
 * Raise failure
 */
void
raise(Failure failure) {
  if(failure != Failure::NONE) {
    throw SqlException(failure == Failure::FATAL ? "Connection lost" : "Syntax error", failure != Failure::FATAL);
  }
}

class MockResultSet: public ResultSet {
public:
  Failure _failure;
  int _rows;
  explicit MockResultSet(Failure failure): _failure(failure), _rows(2) {}
protected:
  virtual bool fetchNext() override {
    raise(_failure);
    return _rows-- > 0;
  }
  virtual bool getValue(std::size_t, std::string& out) override {
    out = "1";
    return false;
  }
  virtual std::optional<std::string> getValue(std::size_t) override {
    return "1";
  }
  virtual const anch::date::DateFormatter& getDateFormatter() override {
    return getDefaultDateFormatter();
  }
  virtual const anch::date::DateFormatter& getTimeFormatter() override {
    return getDefaultTimeFormatter();
  }
  virtual const anch::date::DateFormatter& getTimestampFormatter() override {
    return getDefaultTimestampFormatter();
  }
};

class MockStatement: public PreparedStatement {
public:
  Failure& _failure;
  MockStatement(const std::string& query, Failure& failure): PreparedStatement(), _failure(failure) {
    setParameterCount(anch::sql::findPlaceholders(query, anch::sql::SqlSyntax::STANDARD).size());
    ++calls.prepared;
  }
  virtual ~MockStatement() noexcept {
    ++calls.deleted;
  }
  const std::vector<std::optional<std::string>>& values() const {
    return _values;
  }
protected:
  virtual ResultSet* doExecuteQuery() override {
    raise(_failure);
    return new MockResultSet(Failure::NONE);
  }
  virtual uint64_t doExecuteUpdate() override {
    raise(_failure);
    return 1;
  }
  virtual void deallocate() noexcept override {
    ++calls.deallocated;
  }
};

class MockConnection: public Connection {
public:
  /*! Failure of next operations */
  Failure _failure = Failure::NONE;
  /*! Failure of commit */
  Failure _commitFailure = Failure::NONE;
  /*! Failure of rollback */
  Failure _rollbackFailure = Failure::NONE;
  /*! Result set failure */
  Failure _fetchFailure = Failure::NONE;

  explicit MockConnection(std::size_t statementCacheSize = anch::sql::DEFAULT_STATEMENT_CACHE_SIZE): Connection(statementCacheSize) {
    ++calls.created;
  }

  virtual ~MockConnection() noexcept {
    release();
  }

protected:
  virtual ResultSet* executeQuery(const std::string&) override {
    raise(_failure);
    return new MockResultSet(_fetchFailure);
  }
  virtual uint64_t executeUpdate(const std::string&) override {
    raise(_failure);
    return 1;
  }
  virtual void sendStartTransaction() override {
    raise(_failure);
  }
  virtual void sendCommit() override {
    raise(_commitFailure);
    ++calls.commits;
  }
  virtual void sendRollback() override {
    raise(_rollbackFailure);
    ++calls.rollbacks;
  }
  virtual PreparedStatement* makePrepared(const std::string& query) override {
    raise(_failure);
    return new MockStatement(query, _failure);
  }
};

/*! Pool of mock connections */
std::shared_ptr<Connection>
makeMock(const int&) {
  return std::make_shared<MockConnection>();
}
using MockPool = anch::ResourcePool<Connection, int, makeMock>;

/*!
 * Check that function raises SqlException
 */
template<typename F>
void
expectError(F func, const std::string& msg) {
  try {
    func();
  } catch(const SqlException&) {
    return;
  }
  anch::ut::fail(msg);
}

void
resetCalls() {
  calls = Calls();
}

void
testFatalInvalidates() {
  MockConnection conn;
  conn._failure = Failure::FATAL;
  expectError([&conn]() { conn.update("UPDATE t SET a = 1"); }, "Update should fail");
  anch::ut::assertFalse(conn.isValid(), "Fatal error should invalidate connection");
}

void
testSqlErrorKeepsValid() {
  MockConnection conn;
  conn.begin();
  conn._failure = Failure::SQL;
  expectError([&conn]() { conn.update("UPDATE t SET a = 1"); }, "Update should fail");
  anch::ut::assertTrue(conn.isValid(), "SQL error should not invalidate connection");
  conn.release();
  anch::ut::assertThat(calls.rollbacks == 1 && calls.commits == 0, "Transaction should be rolled back: {} rollbacks, {} commits", calls.rollbacks, calls.commits);
  anch::ut::assertTrue(conn.isValid(), "Connection should still be valid");
}

void
testReleaseRollsBack() {
  MockConnection conn;
  // Unfinished transaction without error (application exception between two queries, ...): rolled back
  conn.begin();
  conn.update("UPDATE t SET a = 1");
  conn.release();
  anch::ut::assertThat(calls.rollbacks == 1 && calls.commits == 0, "Unfinished transaction should be rolled back: {} rollbacks, {} commits", calls.rollbacks, calls.commits);
  anch::ut::assertTrue(conn.isValid(), "Connection should still be valid");
  // Committed transaction: nothing to do
  conn.begin();
  conn.update("UPDATE t SET a = 1");
  conn.commit();
  conn.release();
  anch::ut::assertThat(calls.commits == 1 && calls.rollbacks == 1, "Committed transaction should not be rolled back: {} commits, {} rollbacks", calls.commits, calls.rollbacks);
  // No transaction (auto-commit mode): nothing to do
  conn.update("UPDATE t SET a = 1");
  conn.release();
  anch::ut::assertThat(calls.commits == 1 && calls.rollbacks == 1, "No transaction: {} commits, {} rollbacks", calls.commits, calls.rollbacks);
}

void
testReleaseUnknownState() {
  MockConnection conn;
  conn.begin();
  conn._rollbackFailure = Failure::SQL;
  conn.release();
  anch::ut::assertFalse(conn.isValid(), "Transaction which can not be rolled back should invalidate connection");
}

void
testPreparedStatementErrors() {
  MockConnection conn;
  PreparedStatement& stmt = conn.prepareStatement("UPDATE t SET a = ? WHERE b = ?");
  anch::ut::assertThat(stmt.getParameterCount() == 2, "{} parameters", stmt.getParameterCount());
  expectError([&stmt]() { stmt.set(0, 1); }, "Index 0 should be refused");
  expectError([&stmt]() { stmt.set(3, 1); }, "Index 3 should be refused");
  stmt.set(1, 1);
  expectError([&stmt]() { stmt.executeUpdate(); }, "Unbound parameter 2 should be refused");
  anch::ut::assertTrue(conn.isValid(), "Binding errors should not invalidate connection");
  stmt.setNull(2);
  const auto& values = static_cast<MockStatement&>(stmt).values();
  anch::ut::assertTrue(values[0] == "1" && !values[1].has_value(), "Values should be '1' and NULL");
  anch::ut::assertThat(stmt.executeUpdate() == 1, "Update should succeed");
  conn._failure = Failure::FATAL;
  expectError([&stmt]() { stmt.executeUpdate(); }, "Execution should fail");
  anch::ut::assertFalse(conn.isValid(), "Fatal error of prepared statement should invalidate connection");
}

void
testResultSetErrors() {
  MockConnection conn;
  int rows = 0;
  conn.queryMapRow("SELECT a FROM t", [&rows](ResultSet&) { ++rows; });
  anch::ut::assertThat(rows == 2, "{} rows", rows);
  conn._fetchFailure = Failure::FATAL;
  ResultSet* res = conn.query("SELECT a FROM t");
  expectError([res]() { res->next(); }, "Fetch should fail");
  delete res;
  anch::ut::assertFalse(conn.isValid(), "Fatal error while fetching rows should invalidate connection");
}

void
testPoolDiscardsInvalid() {
  MockPool pool(0, 1);
  {
    auto res = pool.borrowResource();
    MockConnection& conn = static_cast<MockConnection&>(res.get());
    conn._failure = Failure::FATAL;
    expectError([&conn]() { conn.update("UPDATE t SET a = 1"); }, "Update should fail");
  }
  {
    auto res = pool.borrowResource();
    anch::ut::assertTrue(res.get().isValid(), "A new valid connection should be borrowed");
  }
  anch::ut::assertThat(calls.created == 2, "Invalid connection should be replaced: {} created", calls.created);
}

void
testPoolReleasesTransaction() {
  MockPool pool(0, 1);
  {
    auto res = pool.borrowResource();
    res.get().begin();
    res.get().update("UPDATE t SET a = 1");
  } // released: rolled back, even without error
  anch::ut::assertThat(calls.rollbacks == 1 && calls.commits == 0, "Unfinished transaction should be rolled back on return: {} rollbacks, {} commits", calls.rollbacks, calls.commits);
  {
    auto res = pool.borrowResource();
    res.get().begin();
    res.get().update("UPDATE t SET a = 1");
    res.get().commit();
  } // released: nothing to do
  anch::ut::assertThat(calls.commits == 1 && calls.rollbacks == 1, "Committed transaction should be kept: {} commits, {} rollbacks", calls.commits, calls.rollbacks);
  {
    auto res = pool.borrowResource();
    MockConnection& conn = static_cast<MockConnection&>(res.get());
    conn.begin();
    conn._failure = Failure::SQL;
    expectError([&conn]() { conn.update("UPDATE t SET a = 1"); }, "Update should fail");
    conn._failure = Failure::NONE;
  } // released: rolled back, connection kept
  anch::ut::assertThat(calls.rollbacks == 2, "Transaction with error should be rolled back on return: {} rollbacks", calls.rollbacks);
  {
    auto res = pool.borrowResource();
    anch::ut::assertTrue(res.get().isValid(), "Connection should be reused");
  }
  anch::ut::assertThat(calls.created == 1, "Valid connection should be reused: {} created", calls.created);
}

void
testStatementCacheLru() {
  MockConnection conn(2);
  PreparedStatement* a = &conn.prepareStatement("SELECT a FROM t WHERE id = ?");
  PreparedStatement* b = &conn.prepareStatement("SELECT b FROM t WHERE id = ?");
  anch::ut::assertTrue(&conn.prepareStatement("SELECT a FROM t WHERE id = ?") == a, "Cached statement should be reused");
  anch::ut::assertThat(calls.prepared == 2, "Cached statement should not be prepared again: {} prepared", calls.prepared);
  conn.prepareStatement("SELECT c FROM t WHERE id = ?"); // 'b' is the least recently used statement
  anch::ut::assertThat(conn.getCachedStatementCount() == 2, "Cache should be bounded: {} statements", conn.getCachedStatementCount());
  anch::ut::assertThat(calls.deallocated == 1 && calls.deleted == 1, "Evicted statement should be deallocated and deleted: {} deallocated, {} deleted", calls.deallocated, calls.deleted);
  anch::ut::assertTrue(&conn.prepareStatement("SELECT a FROM t WHERE id = ?") == a, "Recently used statement should be kept");
  anch::ut::assertThat(conn.update("UPDATE t SET b = ? WHERE id = ?", 1, 2) == 1, "Update should succeed");
  anch::ut::assertThat(calls.prepared == 4, "Evicted statement should be prepared again: {} prepared", calls.prepared);
  (void)b;
}

void
testStatementCacheBusy() {
  MockConnection conn(1);
  ResultSet* res = conn.query("SELECT a FROM t WHERE id = ?", 1);
  conn.prepareStatement("SELECT b FROM t WHERE id = ?");
  anch::ut::assertThat(conn.getCachedStatementCount() == 2, "Statement with an alive result set should not be evicted: {} statements", conn.getCachedStatementCount());
  anch::ut::assertThat(calls.deleted == 0, "No statement should be deleted: {} deleted", calls.deleted);
  int rows = 0;
  while(res->next()) {
    ++rows;
  }
  anch::ut::assertThat(rows == 2, "Result set should still be readable: {} rows", rows);
  delete res;
  conn.prepareStatement("SELECT c FROM t WHERE id = ?");
  anch::ut::assertThat(conn.getCachedStatementCount() == 1, "Cache should come back to its size: {} statements", conn.getCachedStatementCount());
  anch::ut::assertThat(calls.deallocated == 2, "Both statements should be evicted: {} deallocated", calls.deallocated);
  // Nested queries: the outer statement is kept while its rows are read +
  rows = 0;
  conn.queryMapRow("SELECT a FROM t WHERE id = ?", [&conn, &rows](ResultSet&) {
    conn.queryMapRow("SELECT d FROM t WHERE id = ?", [&rows](ResultSet&) { ++rows; }, 2);
  }, 1);
  anch::ut::assertThat(rows == 4, "Nested queries should read every row: {} rows", rows);
  // Nested queries: the outer statement is kept while its rows are read -
}

void
testStatementCacheSize() {
  {
    MockConnection conn;
    anch::ut::assertThat(conn.getStatementCacheSize() == anch::sql::DEFAULT_STATEMENT_CACHE_SIZE, "Default size: {}", conn.getStatementCacheSize());
    expectError([&conn]() { conn.setStatementCacheSize(0); }, "Size 0 should be refused");
    for(int i = 0 ; i < 5 ; ++i) {
      conn.prepareStatement("SELECT a FROM t WHERE id = " + std::to_string(i));
    }
    conn.setStatementCacheSize(2);
    anch::ut::assertThat(conn.getCachedStatementCount() == 2, "Reducing size should evict statements: {} statements", conn.getCachedStatementCount());
    anch::ut::assertThat(calls.deallocated == 3, "{} deallocated", calls.deallocated);
  }
  anch::ut::assertThat(calls.deallocated == 3 && calls.deleted == 5, "Connection destruction should delete statements without deallocating them: {} deallocated, {} deleted", calls.deallocated, calls.deleted);
  expectError([]() { MockConnection conn(0); }, "Connection with size 0 should be refused");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH SQL connection errors unit tests")
    .description("Test error reporting, connection validity and pool release without database")
    .beforeTest(resetCalls)
    .add("fatal-invalidates", testFatalInvalidates)
    .add("sql-error-keeps-valid", testSqlErrorKeepsValid)
    .add("release-rolls-back", testReleaseRollsBack)
    .add("release-unknown-state", testReleaseUnknownState)
    .add("prepared-statement-errors", testPreparedStatementErrors)
    .add("result-set-errors", testResultSetErrors)
    .add("pool-discards-invalid", testPoolDiscardsInvalid)
    .add("pool-releases-transaction", testPoolReleasesTransaction)
    .add("statement-cache-lru", testStatementCacheLru)
    .add("statement-cache-busy", testStatementCacheBusy)
    .add("statement-cache-size", testStatementCacheSize)
    ;
}
