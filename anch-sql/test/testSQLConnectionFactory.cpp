#include "sql/sqlConnectionFactory.hpp"

#include <iostream>
#include <list>
#include <functional>

using anch::sql::Connection;
using anch::sql::SqlConnectionFactory;
using anch::sql::SqlConnectionPool;
using anch::sql::SqlException;
using anch::sql::ResultSet;

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

int
main(void) {
  std::cout << "Enter in SQL connection factory unit test" << std::endl;

  try {
    SqlConnectionFactory& fact = SqlConnectionFactory::getInstance();
    std::cout << "Factory has been correctly initialized" << std::endl;

    SqlConnectionPool& myPool = fact.getPool("anch_mysql");
    std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' on MySQL database" << std::endl;
    std::list<Person> persons;
    myPool.borrowResource().get().queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test", [&persons](ResultSet& resSet) {
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
    delete sqliteCon;

  } catch(const SqlException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Exit SQL connection factory unit test" << std::endl;
  return 0;
}
