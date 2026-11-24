/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2016 Vincent Lachenal

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
#include "sql/sqlConnectionFactory.hpp"
#include "sql/sqlSharedLibraries.hpp"

#include <iostream>
#include <list>
#include <functional>
#include <memory>
#include <string.h>

#include "conf/configuration.hpp"

#include "ut/assert.hpp"
#include "commonTests.hpp"
#include "ut/unit.hpp"

using anch::sql::Connection;
using anch::sql::SqlConnectionFactory;
using anch::sql::SqlConnectionPool;
using anch::sql::SqlException;
using anch::sql::ResultSet;
using anch::sql::Parameters;
using anch::sql::PreparedStatement;
using anch::sql::SQLSharedLibraries;

class Person {
public:
  uint32_t _id;
  std::string _firstName;
  std::string _lastName;
  std::string _birthDate;
  std::string _email;

public:
  Person(): _id(), _firstName(), _lastName(), _birthDate(), _email() {}
  Person(const std::string& firstName,
	 const std::string& lastName,
	 const std::string& birthDate,
	 const std::string& email):
    _id(),
    _firstName(firstName),
    _lastName(lastName),
    _birthDate(birthDate),
    _email(email) {
  }
  virtual ~Person() {}

  inline uint32_t getId() const {return _id;}
  inline void setId(uint32_t id) {_id = id;}
  inline const std::string& getFirstName() const {return _firstName;}
  inline void setFirstName(const std::string& firstName) {_firstName = firstName;}
  inline const std::string& getLastName() const {return _lastName;}
  inline void setLastName(const std::string& lastName) {_lastName = lastName;}
  inline const std::string& getBirthDate() const {return _birthDate;}
  inline void setBirthDate(const std::string& birthDate) {_birthDate = birthDate;}
  inline const std::string& getEmail() const {return _email;}
  inline void setEmail(const std::string& email) {_email = email;}
};

void
mapPersonRows(ResultSet& resSet, std::list<Person>& persons) {
  Person person;
  resSet.get<uint32_t>(0,person._id);
  resSet.get<std::string>(1,person._firstName);
  resSet.get<std::string>(2,person._lastName);
  resSet.get<std::string>(3,person._birthDate);
  resSet.get<std::string>(4,person._email);
  persons.push_back(person);
}

SqlConnectionFactory* fact;

void
beforeAll() {
  std::cout << "Enter in SQL connection factory unit test" << std::endl;
  std::cout << "To run tests outside of ctest, be sure LD_LIBRARY_PATH environment variable is set to cmake libraries folder (ex: export LD_LIBRARY_PATH=lib)" << std::endl;
  char* dir = std::getenv("LD_LIBRARY_PATH");
  if(dir == NULL) {
    std::cerr << "LD_LIBRARY_PATH is not set" << std::endl;
  }
  anch::conf::Configuration& conf = anch::conf::Configuration::loader().name("db_con").load();
  anch::ut::assertFalse(conf.section("anch::sql") == NULL);
  const anch::conf::Section* sql = conf.section("anch::sql");
  std::cout << sql->debug() << std::endl;
  fact = &(SqlConnectionFactory::getInstance());
  std::cout << "Factory has been correctly initialized" << std::endl;
}

void
afterAll() {
  std::cout << "Exit SQL connection factory unit test" << std::endl;
}

void
testSQLFactory(const std::string& db) {
  try {
    std::list<Person> values;
    values.push_back(Person("Batch Insert 1", "BATCH INSERT 1", "1990-01-01", "batch.insert1@yopmail.com"));
    values.push_back(Person("Batch Insert 2", "BATCH INSERT 2", "1990-02-02", "batch.insert2@yopmail.com"));
    values.push_back(Person("Batch Insert 3", "BATCH INSERT 3", "1990-03-03", "batch.insert3@yopmail.com"));
    values.push_back(Person("Batch Insert 4", "BATCH INSERT 4", "1990-04-04", "batch.insert4@yopmail.com"));
    values.push_back(Person("Batch Insert 5", "BATCH INSERT 5", "1990-05-05", "batch.insert5@yopmail.com"));
    values.push_back(Person("Batch Insert 6", "BATCH INSERT 6", "1990-06-06", "batch.insert6@yopmail.com"));
    values.push_back(Person("Batch Insert 7", "BATCH INSERT 7", "1990-07-07", "batch.insert7@yopmail.com"));
    values.push_back(Person("Batch Insert 8", "BATCH INSERT 8", "1990-08-08", "batch.insert8@yopmail.com"));
    values.push_back(Person("Batch Insert 9", "BATCH INSERT 9", "1990-09-09", "batch.insert9@yopmail.com"));
    values.push_back(Person("Batch Insert 10", "BATCH INSERT 10", "1990-10-10", "batch.insert10@yopmail.com"));
    values.push_back(Person("Batch Insert 11", "BATCH INSERT 11", "1990-11-11", "batch.insert11@yopmail.com"));
    values.push_back(Person("Batch Insert 12", "BATCH INSERT 12", "1990-12-12", "batch.insert12@yopmail.com"));

    std::list<Person> persons;

    SqlConnectionPool& pool = fact->getPool(db);

    std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' on " << db << " database" << std::endl;
    auto func = std::bind(mapPersonRows, std::placeholders::_1, std::ref(persons));
    pool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test", func);
    std::cout << "Found " << persons.size() << " persons." << std::endl;
    for(const Person& pers : persons) {
      std::cout << "Person " << pers._id << ":" << std::endl;
      std::cout << "first name: " << pers._firstName << std::endl;
      std::cout << "last name: " << pers._lastName << std::endl;
      std::cout << "birth data: " << pers._birthDate << std::endl;
      std::cout << "email: " << pers._email << std::endl;
    }

    std::cout << std::endl;

    persons.clear();
    pool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id IN (:a, :b, :c)",
					    func, // Row mapping function
					    {{"a", 1}, {"b", 2}, {"c", 3}}); // Parameters to bind to statement
    std::cout << "Found " << persons.size() << " persons." << std::endl;
    for(const Person& pers : persons) {
      std::cout << "Person " << pers._id << ":" << std::endl;
      std::cout << "first name: " << pers._firstName << std::endl;
      std::cout << "last name: " << pers._lastName << std::endl;
      std::cout << "birth data: " << pers._birthDate << std::endl;
      std::cout << "email: " << pers._email << std::endl;
    }

    std::cout << std::endl;

    pool.borrowResource().get().batchUpdate<Person>("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES (:first,:last,:birth,:email)", [](Parameters& params, const Person& pers) {
      params.add("first", pers._firstName).add("last", pers._lastName).add("birth", pers._birthDate).add("email", pers._email);
    }, values);

    std::cout << std::endl;

    persons.clear();
    pool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name LIKE 'Batch Insert %'", func);
    std::cout << "Found " << persons.size() << " persons." << std::endl;
    for(const Person& pers : persons) {
      std::cout << "Person " << pers._id << ":" << std::endl;
      std::cout << "first name: " << pers._firstName << std::endl;
      std::cout << "last name: " << pers._lastName << std::endl;
      std::cout << "birth data: " << pers._birthDate << std::endl;
      std::cout << "email: " << pers._email << std::endl;
    }

    std::cout << std::endl;

    pool.borrowResource().get().batchUpdate<Person>("UPDATE T_Test SET first_name = :first, last_name = :last WHERE id = :id", [](Parameters& params, const Person& pers) {
      Person p;
      p._id = pers._id;
      p._firstName = pers._firstName;
      p._lastName = pers._lastName;
      params.add("first", p._firstName.replace(p._firstName.find_first_of(std::string("I")), 6, "Update"));
      params.add("last", p._lastName.replace(p._lastName.find_first_of(std::string("I")), 6, "UPDATE"));
      params.add("id", p._id);
    }, persons);

    std::cout << std::endl;

    persons.clear();
    pool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name LIKE 'Batch Update %'", func);
    std::cout << "Found " << persons.size() << " persons." << std::endl;
    for(const Person& pers : persons) {
      std::cout << "Person " << pers._id << ":" << std::endl;
      std::cout << "first name: " << pers._firstName << std::endl;
      std::cout << "last name: " << pers._lastName << std::endl;
      std::cout << "birth data: " << pers._birthDate << std::endl;
      std::cout << "email: " << pers._email << std::endl;
    }

    std::cout << std::endl;

    pool.borrowResource().get().batchUpdate<Person>("DELETE FROM T_Test WHERE id = :id", [](Parameters& params, const Person& pers) {
      params.add("id", pers._id);
    }, persons);

    std::cout << std::endl;

  } catch(const SqlException& e) {
    std::ostringstream oss;
    oss << "Error: " << e.what();
    anch::ut::fail(oss.str());
  }
}

/*!
 * Read connection configuration
 *
 * \param db the configuration name
 *
 * \return the configuration
 */
anch::sql::SqlConnectionConfiguration
configOf(const std::string& db) {
  const anch::conf::Section& section = anch::conf::Configuration::inst().section("anch::sql")->getSections().at(db);
  anch::sql::SqlConnectionConfiguration config;
  config.driver = section.getValue<std::string>("driver").value_or("");
  config.database = section.getValue<std::string>("database").value_or("");
  config.hostname = section.getValue<std::string>("host").value_or("");
  config.user = section.getValue<std::string>("user").value_or("");
  config.password = section.getValue<std::string>("password").value_or("");
  config.port = section.getValue<int>("port").value_or(0);
  return config;
}

/*!
 * Pool of one connection: transactions are ended on return, and a killed connection is replaced
 *
 * \param db the configuration name
 * \param sessionId the function which returns the server session identifier (empty function when connection can not be killed)
 * \param kill the function which kills the connection
 */
void
testPool(const std::string& db,
	 std::function<std::string(Connection&)> sessionId,
	 std::function<void(Connection&, const std::string&)> kill) {
  SqlConnectionPool pool(configOf(db), 1, 0, std::chrono::milliseconds(1000));
  // Error in transaction: rolled back when connection is returned +
  {
    auto res = pool.borrowResource();
    res.get().update("DELETE FROM T_Test WHERE first_name IN ('PoolRollback','PoolCommit')");
    res.get().begin();
    res.get().update("INSERT INTO T_Test (first_name,last_name,birth_date) VALUES ('PoolRollback','Test','2016-03-22')");
    try {
      res.get().update("INSERT INTO T_Unknown (a) VALUES (1)");
    } catch(const SqlException& e) {
      std::cout << "Expected error: " << e.what() << std::endl;
    }
  }
  {
    auto res = pool.borrowResource();
    int64_t count = countRows(res.get(), "SELECT COUNT(*) FROM T_Test WHERE first_name = :first", {{"first", "PoolRollback"}});
    anch::ut::assertThat(count == 0, "Transaction with error should be rolled back on return: {} rows", count);
  }
  // Error in transaction: rolled back when connection is returned -
  // Transaction without error and without commit: rolled back when connection is returned +
  {
    auto res = pool.borrowResource();
    res.get().begin();
    res.get().update("INSERT INTO T_Test (first_name,last_name,birth_date) VALUES ('PoolRollback','Test','2016-03-22')");
  }
  {
    auto res = pool.borrowResource();
    int64_t count = countRows(res.get(), "SELECT COUNT(*) FROM T_Test WHERE first_name = :first", {{"first", "PoolRollback"}});
    anch::ut::assertThat(count == 0, "Transaction without commit should be rolled back on return: {} rows", count);
  }
  // Transaction without error and without commit: rolled back when connection is returned -
  // Committed transaction and auto-commit mode: kept +
  {
    auto res = pool.borrowResource();
    res.get().begin();
    res.get().update("INSERT INTO T_Test (first_name,last_name,birth_date) VALUES ('PoolCommit','Test','2016-03-22')");
    res.get().commit();
    res.get().update("INSERT INTO T_Test (first_name,last_name,birth_date) VALUES ('PoolCommit','Auto','2016-03-22')");
  }
  {
    auto res = pool.borrowResource();
    int64_t count = countRows(res.get(), "SELECT COUNT(*) FROM T_Test WHERE first_name = :first", {{"first", "PoolCommit"}});
    anch::ut::assertThat(count == 2, "Committed transaction and auto-committed query should be kept: {} rows", count);
    res.get().update("DELETE FROM T_Test WHERE first_name = 'PoolCommit'");
  }
  // Committed transaction and auto-commit mode: kept -
  if(!sessionId) {
    return;
  }
  // Killed connection: replaced by a new one +
  std::string killed;
  {
    auto res = pool.borrowResource();
    killed = sessionId(res.get());
    try {
      kill(res.get(), killed);
    } catch(const SqlException& e) {
      std::cout << "Kill: " << e.what() << std::endl;
    }
    try {
      countRows(res.get(), "SELECT COUNT(*) FROM T_Test WHERE id = :id", {{"id", 1}});
    } catch(const SqlException& e) {
      std::cout << "Query after kill: " << e.what() << std::endl;
    }
    anch::ut::assertFalse(res.get().isValid(), "Killed connection should be invalid");
  }
  {
    auto res = pool.borrowResource();
    anch::ut::assertTrue(res.get().isValid(), "Pool should provide a valid connection");
    std::string current = sessionId(res.get());
    anch::ut::assertThat(current != killed, "Killed connection {} should have been replaced", killed);
    anch::ut::assertThat(countRows(res.get(), "SELECT COUNT(*) FROM T_Test WHERE id = :id", {{"id", 1}}) == 1, "New connection should work");
  }
  // Killed connection: replaced by a new one -
}

/*!
 * Get first column of first row as string
 */
std::string
firstValue(Connection& conn, const std::string& query) {
  std::string value;
  conn.queryMapRow(query, [&value](ResultSet& res) { value = res.get<std::string>(0).value_or(""); });
  return value;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH SQL factory unit tests")
    .description("Test AnCH SQL factory library")
    .initialize(beforeAll)
    .uninitialize(afterAll)
#ifdef ANCH_SQL_MYSQL
    .add("sql-factory-mysql", std::bind(testSQLFactory, "anch_mysql"))
    .add("sql-pool-mysql", []() {
      testPool("anch_mysql",
	       [](Connection& conn) { return firstValue(conn, "SELECT CONNECTION_ID()"); },
	       [](Connection& conn, const std::string& id) { conn.update("KILL " + id); });
    })
#endif
#ifdef ANCH_SQL_POSTGRESQL
    .add("sql-factory-postgresql", std::bind(testSQLFactory, "anch_pgsql"))
    .add("sql-pool-postgresql", []() {
      testPool("anch_pgsql",
	       [](Connection& conn) { return firstValue(conn, "SELECT pg_backend_pid()"); },
	       [](Connection& conn, const std::string&) { firstValue(conn, "SELECT pg_terminate_backend(pg_backend_pid())"); });
    })
#endif
#ifdef ANCH_SQL_SQLITE3
    .add("sql-factory-sqlite3", std::bind(testSQLFactory, "anch_sqlite"))
    .add("sql-pool-sqlite3", []() { testPool("anch_sqlite", nullptr, nullptr); })
    .add("sql-factory-statement-cache-size", []() {
      std::size_t size = fact->getPool("anch_sqlite").borrowResource().get().getStatementCacheSize();
      anch::ut::assertThat(size == 16, "Pool connection statement cache size is {} instead of 16", size);
      std::unique_ptr<Connection> conn(fact->createConnection("anch_sqlite"));
      anch::ut::assertThat(conn->getStatementCacheSize() == 16, "Connection statement cache size is {} instead of 16", conn->getStatementCacheSize());
    })
#endif
    ;
}
