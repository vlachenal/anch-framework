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
#include "sql/mysqlConnection.hpp"

#include <iostream>
#include <functional>
#include <list>

#include "ut/assert.hpp"
#include "commonTests.hpp"
#include "ut/unit.hpp"

using anch::sql::Connection;
using anch::sql::MySQLConnection;
using anch::sql::SqlException;
using anch::sql::ResultSet;
using anch::sql::PreparedStatement;


std::string nullStr("NULL");

class Person {
public:
  uint32_t _id;
  std::string _firstName;
  std::string _lastName;
  std::string _birthDate;
  std::string _email;

public:
  Person(): _id(), _firstName(), _lastName(), _birthDate(), _email() {}
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

MySQLConnection* dbCon;

void
beforeAll() {
  std::cout << "Enter in MySQL unit tests" << std::endl << std::endl;
  dbCon = new MySQLConnection({
      .driver = "",
      .database = "anch_test",
      .hostname = "localhost",
      .port = 3306,
      .user = "anch",
      .password = "anch",
      .application = "mysql"
    });
}

void
afterAll() {
  std::cout << "Exit MySQL unit tests" << std::endl;
  if(dbCon != NULL) {
    delete dbCon;
  }
}

/*!
 * Reset test data before each test: only the 4 rows created by the database initialization script (ids 1 to 4) are
 * kept, so that tests depend neither on each other nor on previous executions
 */
void
resetData() {
  dbCon->update("DELETE FROM T_Test WHERE id > 4");
}

/*!
 * Insert the 'Test' person used as precondition by update and delete tests
 *
 * \param firstName the person first name
 */
void
insertTestPerson(const std::string& firstName) {
  uint64_t nbRow = dbCon->update("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES('" + firstName + "','Test','2016-03-22','precondition.test@yopmail.com')");
  anch::ut::assertThat(nbRow == 1, "Precondition: number of inserted rows should be 1 instead of {}", nbRow);
}

void
testQueryManualHandling() {
  std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' with manual result handling" << std::endl;
  ResultSet* res = dbCon->query("SELECT id,first_name,last_name,birth_date,email FROM T_Test");
  try {
    int row = 0;
    while(res->next()) {
      std::cout << "Row " << row << std::endl;
      std::optional<uint32_t> id = res->get<uint32_t>(0);
      std::cout << "id=";
      if(!id) {
	std::cout << "NULL";
      } else {
	std::cout << id.value();
      }
      std::cout << std::endl;
      std::optional<std::string> fName = res->get<std::string>(1);
      std::cout << "first name=" << (!fName ? nullStr : fName.value()) << std::endl;
      std::optional<std::string> lName = res->get<std::string>(2);
      std::cout << "last name=" << (!lName ? nullStr : lName.value()) << std::endl;
      std::optional<std::string> bDate = res->get<std::string>("birth_date");
      std::cout << "birth date=" << (!bDate ? nullStr : bDate.value()) << std::endl;
      std::optional<std::string> email = res->get<std::string>(4);
      std::cout << "email=" << (!email ? nullStr : email.value()) << std::endl;
      row++;
    }
    delete res;
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    delete res;
    anch::ut::fail(oss.str());
  } catch(...) {
    delete res;
    throw;
  }
}

void
testQueryRowResultHandling() {
  std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' with per row result handling" << std::endl;
  std::list<Person> persons;
  try {
    dbCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test", [&persons](ResultSet& resSet) {
      Person person;
      resSet.get<uint32_t>(0,person._id);
      resSet.get<std::string>(1,person._firstName);
      resSet.get<std::string>(2,person._lastName);
      resSet.get<std::string>(3,person._birthDate);
      resSet.get<std::string>(4,person._email);
      persons.push_back(person);
    });
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    anch::ut::fail(oss.str());
  }
  std::cout << "Found " << persons.size() << " persons." << std::endl;
  for(const Person& pers : persons) {
    std::cout << "Person " << pers._id << ":" << std::endl;
    std::cout << "first name: " << pers._firstName << std::endl;
    std::cout << "last name: " << pers._lastName << std::endl;
    std::cout << "birth data: " << pers._birthDate << std::endl;
    std::cout << "email: " << pers._email << std::endl;
  }
}

void
testQueryResultSetHandling() {
  std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' with result set extracting" << std::endl;
  try {
    dbCon->queryExtract("SELECT id,first_name,last_name,birth_date,email FROM T_Test", [](ResultSet& resSet) {
      while(resSet.next()) {
	Person pers;
	resSet.get<uint32_t>(0,pers._id);
	resSet.get<std::string>(1,pers._firstName);
	resSet.get<std::string>(2,pers._lastName);
	resSet.get<std::string>(3,pers._birthDate);
	resSet.get<std::string>(4,pers._email);
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }
    });
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    anch::ut::fail(oss.str());
  }
}

void
testInsert() {
  std::cout << "Execute 'INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES('Insert','Test','2016-03-22','insert.test@yopmail.com')'" << std::endl;
  try {
    uint64_t nbRow = dbCon->update("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES('Insert','Test','2016-03-22','insert.test@yopmail.com')");
    if(nbRow != 1) {
      std::ostringstream oss;
      oss << "Number of affected rows should be 1 instead of " << nbRow;
      anch::ut::fail(oss.str());
    }
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    anch::ut::fail(oss.str());
  }
}

void
testUpdate() {
  insertTestPerson("Insert");
  std::cout << "Execute 'UPDATE T_Test SET first_name = 'Update', email = 'update.test@yopmail.com' WHERE first_name = 'Insert' AND last_name = 'Test''" << std::endl;
  try {
    uint64_t nbRow = dbCon->update("UPDATE T_Test SET first_name = 'Update', email = 'update.test@yopmail.com' WHERE first_name = 'Insert' AND last_name = 'Test'");
    if(nbRow != 1) {
      std::ostringstream oss;
      oss << "Number of affected rows should be 1 instead of " << nbRow;
      anch::ut::fail(oss.str());
    }
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    anch::ut::fail(oss.str());
  }
}

void
testDelete() {
  insertTestPerson("Update");
  std::cout << "Execute 'DELETE FROM T_Test WHERE first_name = 'Update' AND last_name = 'Test''" << std::endl;
  try {
    uint64_t nbRow = dbCon->update("DELETE FROM T_Test WHERE first_name = 'Update' AND last_name = 'Test'");
    if(nbRow != 1) {
      std::ostringstream oss;
      oss << "Number of affected rows should be 1 instead of " << nbRow;
      anch::ut::fail(oss.str());
    }
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    anch::ut::fail(oss.str());
  }
}

void
testQueryStmtRowHandling() {
  std::cout << "Prepare and execute statement 'SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id = :id' with per row result handling" << std::endl;
  ResultSet* res = NULL;
  try {
    PreparedStatement& stmtS = dbCon->prepareStatement("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id = :id");
    stmtS.set("id", 1);
    res = stmtS.executeQuery();
    int row = 0;
    while(res->next()) {
      std::cout << "Row " << row << std::endl;
      std::optional<uint32_t> id = res->get<uint32_t>(0);
      std::cout << "id=";
      if(!id) {
	std::cout << "NULL";
      } else {
	std::cout << id.value();
      }
      std::cout << std::endl;
      std::optional<std::string> fName = res->get<std::string>(1);
      std::cout << "first name=" << (!fName ? nullStr : fName.value()) << std::endl;
      std::optional<std::string> lName = res->get<std::string>(2);
      std::cout << "last name=" << (!lName ? nullStr : lName.value()) << std::endl;
      std::optional<std::string> bDate = res->get<std::string>("birth_date");
      std::cout << "birth date=" << (!bDate ? nullStr : bDate.value()) << std::endl;
      std::optional<std::string> email = res->get<std::string>(4);
      std::cout << "email=" << (!email ? nullStr : email.value()) << std::endl;
      row++;
    }
    delete res;
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    if(res != NULL) {
      delete res;
    }
    anch::ut::fail(oss.str());
  }
}

void
testStmtInsert() {
  std::cout << "Execute 'INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES(:first,:last,:birth,:email)'" << std::endl;
  try {
    PreparedStatement& stmtI = dbCon->prepareStatement("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES(:first,:last,:birth,:email)");
    stmtI.set("first", "Insert");
    stmtI.set("last", "Test");
    stmtI.set("birth", "2016-03-22");
    stmtI.set("email", "insert.test@yopmail.com");
    auto nbRow = stmtI.executeUpdate();
    if(nbRow != 1) {
      std::ostringstream oss;
      oss << "Number of affected rows should be 1 instead of " << nbRow;
      anch::ut::fail(oss.str());
    }
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    anch::ut::fail(oss.str());
  }
}

void
testStmtUpdate() {
  insertTestPerson("Insert");
  std::cout << "Execute 'UPDATE T_Test SET first_name = :newFirst, email = :email WHERE first_name = :first AND last_name = :last'" << std::endl;
  try {
    PreparedStatement& stmtU = dbCon->prepareStatement("UPDATE T_Test SET first_name = :newFirst, email = :email WHERE first_name = :first AND last_name = :last");
    stmtU.set("newFirst", "Update");
    stmtU.set("email", "update.test@yopmail.com");
    stmtU.set("first", "Insert");
    stmtU.set("last", "Test");
    auto nbRow = stmtU.executeUpdate();
    if(nbRow != 1) {
      std::ostringstream oss;
      oss << "Number of affected rows should be 1 instead of " << nbRow;
      anch::ut::fail(oss.str());
    }
  } catch(const anch::sql::SqlException& e) {
    std::ostringstream oss;
    oss <<  "Error on SQL query: " << e.what();
    std::cerr << oss.str() << std::endl;
    anch::ut::fail(oss.str());
  }
}

void
testTransaction() {
  std::cout << "Begin transaction tests" << std::endl;

  std::cout << "Begin transaction" << std::endl;
  dbCon->startTransaction();
  std::cout << "Execute 'INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES('Roll','Back','2016-03-22','roll.back@yopmail.com')'" << std::endl;
  auto nbRow = dbCon->update("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES('Roll','Back','2016-03-22','roll.back@yopmail.com')");
  if(nbRow != 1) {
    std::ostringstream oss;
    oss << "Number of affected rows should be 1 instead of " << nbRow;
    anch::ut::fail(oss.str());
  }
  std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name = 'Roll''" << std::endl;
  std::list<Person> persons;
  dbCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name = 'Roll'", [&persons](ResultSet& resSet) {
    Person person;
    resSet.get<uint32_t>(0,person._id);
    resSet.get<std::string>(1,person._firstName);
    resSet.get<std::string>(2,person._lastName);
    resSet.get<std::string>(3,person._birthDate);
    resSet.get<std::string>(4,person._email);
    persons.push_back(person);
  });
  if(persons.size() != 1) {
    std::ostringstream oss;
    oss << "Found " << persons.size() << " persons instead of 1";
    anch::ut::fail(oss.str());
  }
  std::cout << "Found " << persons.size() << " persons" << std::endl;
  std::cout << "Rollback transaction" << std::endl;
  dbCon->rollback();
  std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name = 'Roll''" << std::endl;
  persons.clear();
  dbCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name = 'Roll'", [&persons](ResultSet& resSet) {
    Person person;
    resSet.get<uint32_t>(0,person._id);
    resSet.get<std::string>(1,person._firstName);
    resSet.get<std::string>(2,person._lastName);
    resSet.get<std::string>(3,person._birthDate);
    resSet.get<std::string>(4,person._email);
    persons.push_back(person);
  });
  if(persons.size() != 0) {
    std::ostringstream oss;
    oss << "Found " << persons.size() << " persons instead of 0";
    anch::ut::fail(oss.str());
  }

  std::cout << "Found " << persons.size() << " persons" << std::endl;
  dbCon->startTransaction();
  std::cout << "Execute 'INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES('Com','Mit','2016-03-22','com.mit@yopmail.com')'" << std::endl;
  nbRow = dbCon->update("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES('Com','Mit','2016-03-22','com.mit@yopmail.com')");
  if(nbRow != 1) {
    std::ostringstream oss;
    oss << "Number of affected rows should be 1 instead of " << nbRow;
    anch::ut::fail(oss.str());
  }
  std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name = 'Com''" << std::endl;
  persons.clear();
  dbCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name = 'Com'", [&persons](ResultSet& resSet) {
    Person person;
    resSet.get<uint32_t>(0,person._id);
    resSet.get<std::string>(1,person._firstName);
    resSet.get<std::string>(2,person._lastName);
    resSet.get<std::string>(3,person._birthDate);
    resSet.get<std::string>(4,person._email);
    persons.push_back(person);
  });
  if(persons.size() != 1) {
    std::ostringstream oss;
    oss << "Found " << persons.size() << " persons instead of 1";
    anch::ut::fail(oss.str());
  }
  std::cout << "Found " << persons.size() << " persons" << std::endl;
  std::cout << "Commit transaction" << std::endl;
  dbCon->commit();
  std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name = 'Com''" << std::endl;
  persons.clear();
  dbCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name = 'Com'", [&persons](ResultSet& resSet) {
    Person person;
    resSet.get<uint32_t>(0,person._id);
    resSet.get<std::string>(1,person._firstName);
    resSet.get<std::string>(2,person._lastName);
    resSet.get<std::string>(3,person._birthDate);
    resSet.get<std::string>(4,person._email);
    persons.push_back(person);
  });
  if(persons.size() != 1) {
    std::ostringstream oss;
    oss << "Found " << persons.size() << " persons instead of 1";
    anch::ut::fail(oss.str());
  }
  std::cout << "Found " << persons.size() << " persons" << std::endl;

  std::cout << "Execute 'DELETE FROM T_Test WHERE first_name = 'Com' AND last_name = 'Mit''" << std::endl;
  nbRow = dbCon->update("DELETE FROM T_Test WHERE first_name = 'Com' AND last_name = 'Mit'");
  if(nbRow != 1) {
    std::ostringstream oss;
    oss << "Number of affected rows should be 1 instead of " << nbRow;
    anch::ut::fail(oss.str());
  }

  std::cout << "End transaction tests" << std::endl;
}

void
testOperators() {
  testStmtOperators(*dbCon);
}

void
testNull() {
  testStmtNull(*dbCon);
}

void
testNamed() {
  testStmtNamed(*dbCon);
}

void
testSqlError() {
  testSqlErrorKeepsConnection(*dbCon);
}

void
testFatal() {
  MySQLConnection conn({
      .driver = "",
      .database = "anch_test",
      .hostname = "localhost",
      .port = 3306,
      .user = "anch",
      .password = "anch",
      .application = "mysql"
    });
  testFatalError(conn, [](Connection& c) {
    std::string id;
    c.queryMapRow("SELECT CONNECTION_ID()", [&id](ResultSet& res) { id = res.get<std::string>(0).value_or(""); });
    c.update("KILL " + id);
  });
}

void
testCache() {
  testStatementCache(*dbCon, false);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH MySQL unit tests")
    .description("Test AnCH MySQL library")
    .initialize(beforeAll)
    .uninitialize(afterAll)
    .beforeTest(resetData)
    .add("01-query-manual-handling", testQueryManualHandling)
    .add("02-query-row-result-handling", testQueryRowResultHandling)
    .add("03-query-result-set-handling", testQueryResultSetHandling)
    .add("04-insert", testInsert)
    .add("05-update", testUpdate)
    .add("06-delete", testDelete)
    .add("07-query-stmt-row-result-handling", testQueryStmtRowHandling)
    .add("08-insert-stmt", testStmtInsert)
    .add("09-update-stmt", testStmtUpdate)
    .add("10-transactions", testTransaction)
    .add("11-stmt-operators", testOperators)
    .add("12-stmt-null", testNull)
    .add("13-sql-error", testSqlError)
    .add("14-fatal-error", testFatal)
    .add("15-statement-cache", testCache)
    .add("16-stmt-named", testNamed)
    ;
}
