#include "sql/sqlConnectionFactory.hpp"
#include "sql/sqlSharedLibraries.hpp"

#include <iostream>
#include <list>
#include <functional>
#include <string.h>

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

int
main(int argc, char** argv) {
  std::cout << "Enter in SQL connection factory unit test" << std::endl;

  try {
    SqlConnectionFactory& fact = SqlConnectionFactory::getInstance();
    std::cout << "Factory has been correctly initialized" << std::endl;

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

    bool all = true;
    if(argc == 2) {
      all = false;
    }

    std::list<Person> persons;
#ifdef ANCH_SQL_MYSQL
    if(all || strcmp("MySQL", argv[1]) == 0) {
      SqlConnectionPool& myPool = fact.getPool("anch_mysql");
      std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' on MySQL database" << std::endl;
      auto mysqlFct = std::bind(mapPersonRows, std::placeholders::_1, std::ref(persons));
      myPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test", mysqlFct);
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
      myPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id IN (?,?,?)",
						mysqlFct, // Row mapping function
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

      myPool.borrowResource().get().batchUpdate<Person>("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES (?,?,?,?)", [](PreparedStatement& stmt, const Person& pers) {
	  stmt.set(1, pers._firstName);
	  stmt.set(2, pers._lastName);
	  stmt.set(3, pers._birthDate);
	  stmt.set(4, pers._email);
	}, values);

      std::cout << std::endl;

      persons.clear();
      myPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name LIKE 'Batch Insert %'", mysqlFct);
      std::cout << "Found " << persons.size() << " persons." << std::endl;
      for(const Person& pers : persons) {
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }

      std::cout << std::endl;

      myPool.borrowResource().get().batchUpdate<Person>("UPDATE T_Test SET first_name = ?, last_name = ? WHERE id = ?", [](PreparedStatement& stmt, const Person& pers) {
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
      myPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name LIKE 'Batch Update %'", mysqlFct);
      std::cout << "Found " << persons.size() << " persons." << std::endl;
      for(const Person& pers : persons) {
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }

      std::cout << std::endl;

      myPool.borrowResource().get().batchUpdate<Person>("DELETE FROM T_Test WHERE id = ?", [](PreparedStatement& stmt, const Person& pers) {
	  stmt.set(1, pers._id);
	}, persons);

      std::cout << std::endl;

    }
#endif // ANCH_SQL_MYSQL

#ifdef ANCH_SQL_POSTGRESQL
    if(all || strcmp("PostgreSQL", argv[1]) == 0) {
      SqlConnectionPool& pgPool = fact.getPool("anch_pgsql");
      persons.clear();
      std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' on PostgreSQL database" << std::endl;
      pgPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test", [&persons](ResultSet& resSet) {
	  Person person;
	  resSet.get<uint32_t>(0,person._id);
	  resSet.get<std::string>(1,person._firstName);
	  resSet.get<std::string>(2,person._lastName);
	  resSet.get<std::string>(3,person._birthDate);
	  resSet.get<std::string>(4,person._email);
	  persons.push_back(person);
	});
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
      std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' on PostgreSQL database" << std::endl;
      pgPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id IN (?,?,?)", [&persons](ResultSet& resSet) {
	  Person person;
	  resSet.get<uint32_t>(0,person._id);
	  resSet.get<std::string>(1,person._firstName);
	  resSet.get<std::string>(2,person._lastName);
	  resSet.get<std::string>(3,person._birthDate);
	  resSet.get<std::string>(4,person._email);
	  persons.push_back(person);
	},
	1, 2, 3);
      std::cout << "Found " << persons.size() << " persons." << std::endl;
      for(const Person& pers : persons) {
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }

      std::cout << std::endl;

      pgPool.borrowResource().get().batchUpdate<Person>("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES (?,?,?,?)", [](PreparedStatement& stmt, const Person& pers) {
	  stmt.set(1, pers._firstName);
	  stmt.set(2, pers._lastName);
	  stmt.set(3, pers._birthDate);
	  stmt.set(4, pers._email);
	}, values);

      std::cout << std::endl;

      auto pgsqlFct = std::bind(mapPersonRows, std::placeholders::_1, std::ref(persons));
      persons.clear();
      pgPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name LIKE 'Batch Insert %'", pgsqlFct);
      std::cout << "Found " << persons.size() << " persons." << std::endl;
      for(const Person& pers : persons) {
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }

      std::cout << std::endl;

      pgPool.borrowResource().get().batchUpdate<Person>("UPDATE T_Test SET first_name = ?, last_name = ? WHERE id = ?", [](PreparedStatement& stmt, const Person& pers) {
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
      pgPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name LIKE 'Batch Update %'", pgsqlFct);
      std::cout << "Found " << persons.size() << " persons." << std::endl;
      for(const Person& pers : persons) {
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }

      std::cout << std::endl;

      pgPool.borrowResource().get().batchUpdate<Person>("DELETE FROM T_Test WHERE id = ?", [](PreparedStatement& stmt, const Person& pers) {
	  stmt.set(1, pers._id);
	}, persons);

      std::cout << std::endl;

    }
#endif // ANCH_SQL_POSTGRESQL

#ifdef ANCH_SQL_SQLITE3
    if(all || strcmp("SQLite", argv[1]) == 0) {
      Connection* sqliteCon = fact.createConnection("anch_sqlite");
      persons.clear();
      std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' on SQLite3 database" << std::endl;
      sqliteCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test", [&persons](ResultSet& resSet) {
	  Person person;
	  resSet.get<uint32_t>(0,person._id);
	  resSet.get<std::string>(1,person._firstName);
	  resSet.get<std::string>(2,person._lastName);
	  resSet.get<std::string>(3,person._birthDate);
	  resSet.get<std::string>(4,person._email);
	  persons.push_back(person);
	});
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
      std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' on PostgreSQL database" << std::endl;
      sqliteCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE id IN (?,?,?)", [&persons](ResultSet& resSet) {
	  Person person;
	  resSet.get<uint32_t>(0,person._id);
	  resSet.get<std::string>(1,person._firstName);
	  resSet.get<std::string>(2,person._lastName);
	  resSet.get<std::string>(3,person._birthDate);
	  resSet.get<std::string>(4,person._email);
	  persons.push_back(person);
	},
	1, 2, 3);
      std::cout << "Found " << persons.size() << " persons." << std::endl;
      for(const Person& pers : persons) {
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }

      std::cout << std::endl;

      sqliteCon->batchUpdate<Person>("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES (?,?,?,?)", [](PreparedStatement& stmt, const Person& pers) {
	  stmt.set(1, pers._firstName);
	  stmt.set(2, pers._lastName);
	  stmt.set(3, pers._birthDate);
	  stmt.set(4, pers._email);
	}, values);

      std::cout << std::endl;

      auto sqliteFct = std::bind(mapPersonRows, std::placeholders::_1, std::ref(persons));
      persons.clear();
      sqliteCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name LIKE 'Batch Insert %'", sqliteFct);
      std::cout << "Found " << persons.size() << " persons." << std::endl;
      for(const Person& pers : persons) {
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }

      std::cout << std::endl;

      sqliteCon->batchUpdate<Person>("UPDATE T_Test SET first_name = ?, last_name = ? WHERE id = ?", [](PreparedStatement& stmt, const Person& pers) {
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
      sqliteCon->queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test WHERE first_name LIKE 'Batch Update %'", sqliteFct);
      std::cout << "Found " << persons.size() << " persons." << std::endl;
      for(const Person& pers : persons) {
	std::cout << "Person " << pers._id << ":" << std::endl;
	std::cout << "first name: " << pers._firstName << std::endl;
	std::cout << "last name: " << pers._lastName << std::endl;
	std::cout << "birth data: " << pers._birthDate << std::endl;
	std::cout << "email: " << pers._email << std::endl;
      }

      std::cout << std::endl;

      sqliteCon->batchUpdate<Person>("DELETE FROM T_Test WHERE id = ?", [](PreparedStatement& stmt, const Person& pers) {
	  stmt.set(1, pers._id);
	}, persons);

      std::cout << std::endl;

      delete sqliteCon;
    }
#endif // ANCH_SQL_SQLITE3

  } catch(const SqlException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Exit SQL connection factory unit test" << std::endl;
  return 0;
}
