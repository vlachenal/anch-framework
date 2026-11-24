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
#include "sql/builder/clauses.hpp"
#include "sql/builder/sqlFunctions.hpp"
#include "sql/placeholders.hpp"

#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "resourcePool.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::Connection;
using anch::sql::PreparedStatement;
using anch::sql::ResultSet;
using anch::sql::SqlException;

// Described type (anch::Mapping) bound by fieldsOf +
namespace {

  struct Pet {
    int64_t id = 0;
    std::string name;
    std::optional<std::string> nickname;
    double weight = 0.;
    std::string code;

    std::string upperName() const {
      std::string res(name);
      for(char& c : res) {
	c = (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
      }
      return res;
    }
  };

}

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id)
      .registerField("name", &Pet::name)
      .registerField("nickname", &Pet::nickname)
      .registerField("weight", &Pet::weight)
      .registerField("upper", std::function<std::string(const Pet&)>(&Pet::upperName))
      .registerField("legacy", std::function<void(Pet&, const std::string&)>([](Pet& pet, const std::string& code) { pet.code = code; }))
      ;
  }
};
static_assert(anch::ObjectMapper<anch::sql::FieldsMapper<Pet>, Pet>);
// Described type (anch::Mapping) bound by fieldsOf -

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
  virtual const anch::date::DateFormatter& getTimestampFormatter() override {
    return getDefaultTimestampFormatter();
  }
};

class MockStatement: public PreparedStatement {
public:
  Failure& _failure;
  MockStatement(const std::string& query, Failure& failure): PreparedStatement(), _failure(failure) {
    doPrepare(parse(query, anch::sql::SqlSyntax::STANDARD, anch::sql::ParameterStyle::POSITIONAL));
    ++calls.prepared;
  }
  virtual ~MockStatement() noexcept {
    ++calls.deleted;
  }
  void markInvalid() noexcept {
    invalidate();
  }
  /*! Prepared queries (constructor: first one) */
  std::vector<std::string> _prepared;
protected:
  virtual void doPrepare(const std::string& query) override {
    _prepared.push_back(query);
  }
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

template<typename F>
void
expectMessage(F func, const std::string& expected) {
  try {
    func();
  } catch(const SqlException& error) {
    anch::ut::assertThat(std::string(error.what()).find(expected) != std::string::npos, "Unexpected error '{}' instead of '{}'", error.what(),
			 expected);
    anch::ut::assertTrue(error.isOK(), "Parameters errors should not be fatal");
    return;
  }
  anch::ut::fail("Error '" + expected + "' expected");
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
  PreparedStatement& stmt = conn.prepareStatement("UPDATE t SET a = :a, c = :c WHERE b = :b OR d = :b");
  anch::ut::assertThat(stmt.getParameterCount() == 3 && stmt.getParameterNames() == std::vector<std::string>{"a", "c", "b"},
		       "{} parameters", stmt.getParameterCount());
  expectMessage([&stmt]() { stmt.set("other", 1); }, "no parameter 'other' in query (parameters: a, c, b): UPDATE t SET a = :a");
  stmt.set("a", 1);
  expectMessage([&stmt]() { stmt.executeUpdate(); }, "parameters 'c', 'b' have not been bound: UPDATE t SET");
  stmt.set("c", std::optional<int32_t>(2));
  expectMessage([&stmt]() { stmt.executeUpdate(); }, "parameter 'b' has not been bound");
  anch::ut::assertTrue(conn.isValid(), "Binding errors should not invalidate connection");
  stmt.setNull("b");
  // MySQL style: one value for each '?', a name used several times gives the same value (no copy)
  anch::ut::assertThat(stmt.getBindingCount() == 4, "{} bindings", stmt.getBindingCount());
  anch::ut::assertTrue(stmt.getBindingValue(0) == "1" && stmt.getBindingValue(1) == "2" && !stmt.getBindingValue(2).has_value()
		       && &stmt.getBindingValue(2) == &stmt.getBindingValue(3), "Values should be '1', '2', NULL and NULL");
  anch::ut::assertThat(stmt.executeUpdate() == 1, "Update should succeed");
  // Parameters: values of previous executions are not reused, unknown names are refused
  const std::string query = "UPDATE t SET a = :a, c = :c WHERE b = :b OR d = :b";
  anch::ut::assertThat(conn.update(query, {{"a", 1}, {"c", "x"}, {"b", std::nullopt}}) == 1, "Update with parameters should succeed");
  expectMessage([&conn, &query]() { conn.update(query, {{"a", 1}, {"c", "x"}}); }, "parameter 'b' has not been bound");
  expectMessage([&conn, &query]() { conn.update(query, anch::sql::Parameters().add("a", 1).add("c", "x").addNull("b").add("e", 2)); },
		"no parameter 'e' in query");
  // Positional parameters are refused
  expectMessage([&conn]() { conn.prepareStatement("UPDATE t SET a = ? WHERE b = ?"); }, "positional parameters ('?') are not supported");
  anch::ut::assertTrue(conn.isValid(), "Binding errors should not invalidate connection");
  conn._failure = Failure::FATAL;
  expectError([&stmt]() { stmt.executeUpdate(); }, "Execution should fail");
  anch::ut::assertFalse(conn.isValid(), "Fatal error of prepared statement should invalidate connection");
}

void
testFieldsOf() {
  MockConnection conn;
  Pet pet;
  pet.id = 3;
  pet.name = "rex";
  pet.weight = 1.5;
  // Parameters are the fields of the same names (other fields are ignored), getters give values, setters are ignored
  const std::string query = "UPDATE pet SET name = :name, upper = :upper, nickname = :nickname, weight = :weight WHERE id = :id";
  anch::ut::assertThat(conn.update(query, anch::sql::fieldsOf(pet)) == 1, "Update with fields should succeed");
  PreparedStatement& stmt = conn.prepareStatement(query);
  anch::ut::assertTrue(stmt.getBindingValue(0) == "rex" && stmt.getBindingValue(1) == "REX" && !stmt.getBindingValue(2).has_value()
		       && stmt.getBindingValue(3) == "1.5" && stmt.getBindingValue(4) == "3", "Unexpected fields values");
  // A parameter which is not a field (or which is only written) is not bound
  expectMessage([&conn, &pet]() { conn.update("UPDATE pet SET code = :legacy WHERE id = :id AND owner = :owner", anch::sql::fieldsOf(pet)); },
		"parameters 'legacy', 'owner' have not been bound");
  // Batch of described objects
  Pet other = pet;
  other.id = 4;
  other.nickname = "Bob";
  std::vector<Pet> pets{pet, other};
  anch::ut::assertThat(conn.batchUpdate("UPDATE pet SET nickname = :nickname WHERE id = :id", pets) == 2, "Batch should update 2 rows");
  PreparedStatement& batch = conn.prepareStatement("UPDATE pet SET nickname = :nickname WHERE id = :id");
  anch::ut::assertTrue(batch.getBindingValue(0) == "Bob" && batch.getBindingValue(1) == "4", "Last object values expected");
  anch::ut::assertTrue(conn.isValid(), "Binding errors should not invalidate connection");
}

void
testLists() {
  // Lists: PostgreSQL arrays, expanded IN lists for other databases (MySQL style here) ; strings are not lists
  static_assert(anch::sql::BindableList<std::vector<int32_t>> && anch::sql::BindableList<std::set<std::string>>
		&& !anch::sql::BindableList<std::string> && !anch::sql::BindableList<std::vector<std::vector<int32_t>>>
		&& anch::sql::Bindable<std::list<std::optional<int64_t>>>);
  anch::ut::assertThat(anch::sql::arrayLiteral({"1", "a\"b", "c\\d", std::nullopt}) == "{\"1\",\"a\\\"b\",\"c\\\\d\",NULL}" && anch::sql::arrayLiteral({}) == "{}",
		       "Unexpected array literal '{}'", anch::sql::arrayLiteral({"1", "a\"b", "c\\d", std::nullopt}));
  MockConnection conn;
  const std::string query = "SELECT a FROM t WHERE id IN (:ids) AND b = :b AND c NOT IN (:ids)";
  MockStatement& stmt = dynamic_cast<MockStatement&>(conn.prepareStatement(query));
  stmt.set("ids", std::vector<int32_t>{1, 2, 3});
  stmt.set("b", "x");
  delete stmt.executeQuery();
  anch::ut::assertThat(stmt._prepared.back() == "SELECT a FROM t WHERE id IN (?, ?, ?) AND b = ? AND c NOT IN (?, ?, ?)",
		       "Unexpected expanded query '{}'", stmt._prepared.back());
  anch::ut::assertTrue(stmt.getBindingCount() == 7 && stmt.getBindingValue(0) == "1" && stmt.getBindingValue(2) == "3"
		       && stmt.getBindingValue(3) == "x" && stmt.getBindingValue(6) == "3", "Unexpected expanded values");
  // Same size: not prepared again ; other size and empty list: prepared again
  stmt.set("ids", std::list<int32_t>{4, 5, 6});
  delete stmt.executeQuery();
  anch::ut::assertThat(stmt._prepared.size() == 2 && stmt.getBindingValue(0) == "4", "{} prepared queries instead of 2", stmt._prepared.size());
  stmt.set("ids", std::vector<int32_t>());
  delete stmt.executeQuery();
  anch::ut::assertThat(stmt._prepared.back() == "SELECT a FROM t WHERE id IN (SELECT NULL FROM (SELECT 1) AS anch_empty WHERE 1 = 0)"
		       " AND b = ? AND c NOT IN (SELECT NULL FROM (SELECT 1) AS anch_empty WHERE 1 = 0)", "Unexpected query '{}'", stmt._prepared.back());
  // Value which is not a list: first query again
  stmt.set("ids", 7);
  delete stmt.executeQuery();
  anch::ut::assertThat(stmt._prepared.back() == "SELECT a FROM t WHERE id IN (?) AND b = ? AND c NOT IN (?)", "Unexpected query '{}'",
		       stmt._prepared.back());
  // Parameters, described objects and builder with lists
  anch::ut::assertThat(conn.update("UPDATE t SET a = 1 WHERE id IN (:ids)", {{"ids", std::vector<int64_t>{1, 2}}}) == 1, "Update with list");
  anch::sql::SQLQuery built = anch::sql::select().field("a").from("t")
    .where(anch::sql::clauses().AND("id", anch::sql::in, std::vector<int32_t>{1, 2}).AND("b", anch::sql::in, std::vector<int32_t>())).build();
  anch::ut::assertThat(built.getQuery() == "SELECT a FROM t WHERE id IN (:id)" && built.getParameters().values()[0].list->size() == 2,
		       "Unexpected built query '{}' (empty lists skip their clause)", built.getQuery());
  // Expanded list has to be the only item of an IN list
  expectMessage([&conn]() { conn.query("SELECT a FROM t WHERE id = ANY(:ids)", {{"ids", std::vector<int32_t>{1}}}); },
		"list parameter 'ids' has to be the only item of an IN list");
  anch::ut::assertTrue(conn.isValid(), "List errors should not invalidate connection");
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
  PreparedStatement* a = &conn.prepareStatement("SELECT a FROM t WHERE id = :id");
  PreparedStatement* b = &conn.prepareStatement("SELECT b FROM t WHERE id = :id");
  anch::ut::assertTrue(&conn.prepareStatement("SELECT a FROM t WHERE id = :id") == a, "Cached statement should be reused");
  anch::ut::assertThat(calls.prepared == 2, "Cached statement should not be prepared again: {} prepared", calls.prepared);
  conn.prepareStatement("SELECT c FROM t WHERE id = :id"); // 'b' is the least recently used statement
  anch::ut::assertThat(conn.getCachedStatementCount() == 2, "Cache should be bounded: {} statements", conn.getCachedStatementCount());
  anch::ut::assertThat(calls.deallocated == 1 && calls.deleted == 1, "Evicted statement should be deallocated and deleted: {} deallocated, {} deleted", calls.deallocated, calls.deleted);
  anch::ut::assertTrue(&conn.prepareStatement("SELECT a FROM t WHERE id = :id") == a, "Recently used statement should be kept");
  anch::ut::assertThat(conn.update("UPDATE t SET b = :b WHERE id = :id", {{"b", 1}, {"id", 2}}) == 1, "Update should succeed");
  anch::ut::assertThat(calls.prepared == 4, "Evicted statement should be prepared again: {} prepared", calls.prepared);
  (void)b;
}

void
testStatementCacheBusy() {
  MockConnection conn(1);
  ResultSet* res = conn.query("SELECT a FROM t WHERE id = :id", {{"id", 1}});
  conn.prepareStatement("SELECT b FROM t WHERE id = :id");
  anch::ut::assertThat(conn.getCachedStatementCount() == 2, "Statement with an alive result set should not be evicted: {} statements", conn.getCachedStatementCount());
  anch::ut::assertThat(calls.deleted == 0, "No statement should be deleted: {} deleted", calls.deleted);
  int rows = 0;
  while(res->next()) {
    ++rows;
  }
  anch::ut::assertThat(rows == 2, "Result set should still be readable: {} rows", rows);
  delete res;
  conn.prepareStatement("SELECT c FROM t WHERE id = :id");
  anch::ut::assertThat(conn.getCachedStatementCount() == 1, "Cache should come back to its size: {} statements", conn.getCachedStatementCount());
  anch::ut::assertThat(calls.deallocated == 2, "Both statements should be evicted: {} deallocated", calls.deallocated);
  // Nested queries: the outer statement is kept while its rows are read +
  rows = 0;
  conn.queryMapRow("SELECT a FROM t WHERE id = :id", [&conn, &rows](ResultSet&) {
    conn.queryMapRow("SELECT d FROM t WHERE id = :id", [&rows](ResultSet&) { ++rows; }, {{"id", 2}});
  }, {{"id", 1}});
  anch::ut::assertThat(rows == 4, "Nested queries should read every row: {} rows", rows);
  // Nested queries: the outer statement is kept while its rows are read -
}

void
testStatementCacheNestedSameQuery() {
  // A statement is not executed again while its result set is read: an other one is prepared
  MockConnection conn(4);
  int rows = 0;
  PreparedStatement* outer = &conn.prepareStatement("SELECT a FROM t WHERE id = :id"); // used by queryMapRow
  conn.queryMapRow("SELECT a FROM t WHERE id = :id", [&conn, &rows, &outer](ResultSet&) {
    PreparedStatement& inner = conn.prepareStatement("SELECT a FROM t WHERE id = :id");
    anch::ut::assertTrue(&inner != outer, "Busy statement should not be returned");
    conn.queryMapRow("SELECT a FROM t WHERE id = :id", [&rows](ResultSet&) { ++rows; }, {{"id", 2}});
  }, {{"id", 1}});
  anch::ut::assertThat(rows == 4, "Nested queries should read every row: {} rows", rows);
  anch::ut::assertThat(calls.prepared == 2, "Inner statement should be reused for each outer row: {} prepared", calls.prepared);
  anch::ut::assertThat(conn.getCachedStatementCount() == 2, "Both statements should be cached: {} statements", conn.getCachedStatementCount());
  PreparedStatement& again = conn.prepareStatement("SELECT a FROM t WHERE id = :id");
  anch::ut::assertThat(calls.prepared == 2, "Free statement should be reused: {} prepared", calls.prepared);
  // Busy statement replaced by an other one is evicted as any statement
  conn.setStatementCacheSize(1);
  anch::ut::assertThat(conn.getCachedStatementCount() == 1 && calls.deleted == 1, "{} statements, {} deleted", conn.getCachedStatementCount(), calls.deleted);
  anch::ut::assertTrue(&conn.prepareStatement("SELECT a FROM t WHERE id = :id") == &again, "Indexed statement should be kept");
}

void
testStatementCacheInvalidated() {
  MockConnection conn(4);
  dynamic_cast<MockStatement&>(conn.prepareStatement("SELECT a FROM t WHERE id = :id")).markInvalid();
  conn.prepareStatement("SELECT a FROM t WHERE id = :id");
  anch::ut::assertThat(calls.prepared == 2 && calls.deallocated == 1 && calls.deleted == 1,
		       "Invalidated statement should be replaced: {} prepared, {} deallocated, {} deleted", calls.prepared, calls.deallocated, calls.deleted);
  anch::ut::assertThat(conn.getCachedStatementCount() == 1, "{} statements", conn.getCachedStatementCount());
  // Invalidated statement used by a result set: replaced, kept until the result set is destroyed
  PreparedStatement& busy = conn.prepareStatement("SELECT a FROM t WHERE id = :id");
  busy.set("id", 1);
  ResultSet* res = busy.executeQuery();
  dynamic_cast<MockStatement&>(busy).markInvalid();
  calls = Calls();
  PreparedStatement& other = conn.prepareStatement("SELECT a FROM t WHERE id = :id");
  anch::ut::assertTrue(&other != &busy && calls.prepared == 1 && calls.deleted == 0,
		       "Busy invalidated statement should be replaced and kept");
  // Replacement invalidated too (free): prepared again
  dynamic_cast<MockStatement&>(other).markInvalid();
  conn.prepareStatement("SELECT a FROM t WHERE id = :id");
  anch::ut::assertThat(calls.prepared == 2 && calls.deleted == 1, "{} prepared, {} deleted", calls.prepared, calls.deleted);
  int rows = 0;
  while(res->next()) {
    ++rows;
  }
  anch::ut::assertThat(rows == 2, "Result set should still be readable: {} rows", rows);
  delete res;
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
    .add("fields-of", testFieldsOf)
    .add("lists", testLists)
    .add("result-set-errors", testResultSetErrors)
    .add("pool-discards-invalid", testPoolDiscardsInvalid)
    .add("pool-releases-transaction", testPoolReleasesTransaction)
    .add("statement-cache-lru", testStatementCacheLru)
    .add("statement-cache-busy", testStatementCacheBusy)
    .add("statement-cache-nested-same-query", testStatementCacheNestedSameQuery)
    .add("statement-cache-invalidated", testStatementCacheInvalidated)
    .add("statement-cache-size", testStatementCacheSize)
    ;
}
