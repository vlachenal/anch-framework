#include "sql/sqlite3Connection.hpp"

#include <iostream>
#include <functional>
#include <list>

using anch::sql::Connection;
using anch::sql::SQLite3Connection;
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
  std::cout << "Enter in MySQL unit tests" << std::endl << std::endl;

  try {
    SQLite3Connection dbCon("anch.db");
    std::cout << "Connected to database" << std::endl;
    // dbCon.setAutoCommit(false);
    // std::cout << "Auto commit set to false" << std::endl;
    // dbCon.commit();
    // std::cout << "Commit no transaction ..." << std::endl;
    // dbCon.rollback();
    std::cout << "Rollback no transaction ..." << std::endl;
    dbCon.setAutoCommit(true);

    std::cout << std::endl;

    std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' with manual result handling" << std::endl;
    ResultSet* res = dbCon.query("SELECT id,first_name,last_name,birth_date,email FROM T_Test");
    int row = 0;
    std::string nullStr("NULL");
    while(res->next()) {
      std::cout << "Row " << row << std::endl;
      const uint32_t* id = res->get<uint32_t>(0);
      std::cout << "id=";
      if(id == NULL) {
	std::cout << "NULL";
      } else {
	std::cout << *id;
      }
      std::cout << std::endl;
      delete id;
      const std::string* fName = res->get<std::string>(1);
      std::cout << "first name=" << (fName == NULL ? nullStr : *fName) << std::endl;
      delete fName;
      const std::string* lName = res->get<std::string>(2);
      std::cout << "last name=" << (lName == NULL ? nullStr : *lName) << std::endl;
      delete lName;
      const std::string* bDate = res->get<std::string>("birth_date");
      std::cout << "birth date=" << (bDate == NULL ? nullStr : *bDate) << std::endl;
      delete bDate;
      const std::string* email = res->get<std::string>(4);
      std::cout << "email=" << (email == NULL ? nullStr : *email) << std::endl;
      delete email;
      row++;
    }
    delete res;

    std::cout << std::endl;

    std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' with per row result handling" << std::endl;
    std::list<Person> persons;
    dbCon.queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test", [&persons](ResultSet& resSet) {
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

    std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' with result set extracting" << std::endl;
    dbCon.queryExtract("SELECT id,first_name,last_name,birth_date,email FROM T_Test", [](ResultSet& resSet) {
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

    std::cout << std::endl;

  } catch(const SqlException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Exit MySQL unit tests" << std::endl;
  return 0;
}
