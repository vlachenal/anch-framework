#include "sql/postgresqlConnection.hpp"

#include <iostream>
#include <functional>
#include <list>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::Connection;
using anch::sql::PostgreSQLConnection;
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


PostgreSQLConnection* dbCon;

void
beforeAll() {
  std::cout << "Enter in PostgreSQL unit tests" << std::endl << std::endl;
  dbCon = new PostgreSQLConnection({
      .driver = "",
      .database = "anch_test",
      .hostname = "localhost",
      .port = 5432,
      .user = "anch",
      .password = "anch",
      .application = "postgresql"
    });
}

void
afterAll() {
  std::cout << "Exit PostgreSQL unit tests" << std::endl;
  if(dbCon != NULL) {
    delete dbCon;
  }
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
  std::cout << "Prepare and execute statement 'SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id = ?' with per row result handling" << std::endl;
  ResultSet* res = NULL;
  try {
    PreparedStatement& stmtS = dbCon->prepareStatement("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id = ?");
    stmtS.set(1, 1);
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
  std::cout << "Execute 'INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES(?,?,?,?)'" << std::endl;
  try {
    PreparedStatement& stmtI = dbCon->prepareStatement("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES(?,?,?,?)");
    stmtI.set(1,"Insert");
    stmtI.set(2, "Test");
    stmtI.set(3, "2016-03-22");
    stmtI.set(4, "insert.test@yopmail.com");
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
  std::cout << "Execute 'UPDATE T_Test SET first_name = ?, email = ? WHERE first_name = ? AND last_name = ?'" << std::endl;
  try {
    PreparedStatement& stmtU = dbCon->prepareStatement("UPDATE T_Test SET first_name = ?, email = ? WHERE first_name = ? AND last_name = ?");
    stmtU.set(1,"Update");
    stmtU.set(2, "update.test@yopmail.com");
    stmtU.set(3, "Insert");
    stmtU.set(4, "Test");
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
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH PostgreSQL unit tests")
    .description("Test AnCH PostgreSQL library")
    .initialize(beforeAll)
    .uninitialize(afterAll)
    .add("01-query-manual-handling", testQueryManualHandling)
    .add("02-query-row-result-handling", testQueryRowResultHandling)
    .add("03-query-result-set-handling", testQueryResultSetHandling)
    .add("04-insert", testInsert)
    .add("05-update", testUpdate)
    //.add("06-delete", testDelete)
    .add("07-query-stmt-row-result-handling", testQueryStmtRowHandling)
    .add("08-insert-stmt", testStmtInsert)
    .add("09-update-stmt", testStmtUpdate)
    .add("10-transactions", testTransaction)
    ;
}
