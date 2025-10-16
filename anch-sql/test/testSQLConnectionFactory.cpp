#include "sql/sqlConnectionFactory.hpp"
#include "sql/sqlSharedLibraries.hpp"

#include <iostream>
#include <list>
#include <functional>
#include <string.h>

#include "conf/configuration.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::Connection;
using anch::sql::SqlConnectionFactory;
using anch::sql::SqlConnectionPool;
using anch::sql::SqlException;
using anch::sql::ResultSet;
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
    pool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id IN (?,?,?)",
					    func, // Row mapping function
					    1, 2, 3); // Parameters to bind to statement
    std::cout << "Found " << persons.size() << " persons." << std::endl;
    for(const Person& pers : persons) {
      std::cout << "Person " << pers._id << ":" << std::endl;
      std::cout << "first name: " << pers._firstName << std::endl;
      std::cout << "last name: " << pers._lastName << std::endl;
      std::cout << "birth data: " << pers._birthDate << std::endl;
      std::cout << "email: " << pers._email << std::endl;
    }

    std::cout << std::endl;

    pool.borrowResource().get().batchUpdate<Person>("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES (?,?,?,?)", [](PreparedStatement& stmt, const Person& pers) {
      stmt.set(1, pers._firstName);
      stmt.set(2, pers._lastName);
      stmt.set(3, pers._birthDate);
      stmt.set(4, pers._email);
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

    pool.borrowResource().get().batchUpdate<Person>("UPDATE T_Test SET first_name = ?, last_name = ? WHERE id = ?", [](PreparedStatement& stmt, const Person& pers) {
      Person p;
      p._id = pers._id;
      p._firstName = pers._firstName;
      p._lastName = pers._lastName;
      stmt.set(1, p._firstName.replace(p._firstName.find_first_of(std::string("I")), 6, "Update"));
      stmt.set(2, p._lastName.replace(p._lastName.find_first_of(std::string("I")), 6, "UPDATE"));
      stmt.set(3, p._id);
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

    pool.borrowResource().get().batchUpdate<Person>("DELETE FROM T_Test WHERE id = ?", [](PreparedStatement& stmt, const Person& pers) {
      stmt.set(1, pers._id);
    }, persons);

    std::cout << std::endl;

  } catch(const SqlException& e) {
    std::ostringstream oss;
    oss << "Error: " << e.what();
    anch::ut::fail(oss.str());
  }
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
#endif
#ifdef ANCH_SQL_POSTGRESQL
    .add("sql-factory-postgresql", std::bind(testSQLFactory, "anch_pgsql"))
#endif
#ifdef ANCH_SQL_SQLITE3
    .add("sql-factory-sqlite3", std::bind(testSQLFactory, "anch_sqlite"))
#endif
    ;
}
