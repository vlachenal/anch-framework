/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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
//! [example]
#include "sql/postgresqlConnection.hpp"

#include <iostream>
#include <functional>
#include <list>
#include <optional>
#include <string>

using anch::sql::Connection;
using anch::sql::PostgreSQLConnection;
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

//! [description]
// Common description of Person (JSON, CSV, validation, SQL parameters, ...): fields are SQL parameters of the same
// names with anch::sql::fieldsOf
template<>
struct anch::Mapping<Person> {
  static void
  registerObject(anch::ObjectMapper<Person> auto& mapper) {
    mapper
      .registerField("id", &Person::_id)
      .registerField("first_name", &Person::_firstName)
      .registerField("last_name", &Person::_lastName)
      .registerField("birth_date", &Person::_birthDate)
      .registerField("email", &Person::_email)
      ;
  }
};
//! [description]


int
main(void) {
  std::cout << "Enter in PostgreSQL unit tests" << std::endl << std::endl;

  try {
    anch::sql::SqlConnectionConfiguration config;
    config.driver = "PostgreSQL";
    config.database = "anch_test";
    config.hostname = "localhost";
    config.port = 5432;
    config.user = "anch";
    config.password = "anch";
    PostgreSQLConnection dbCon(config);
    std::cout << "Connected to database" << std::endl;

    // Without transaction, each query is committed by the database (auto-commit mode). Parameters are named
    // (':name'): each one has to be given, a name used several times is bound once
    //! [parameters]
    dbCon.update("UPDATE T_Test SET email = :email WHERE first_name = :first", {{"email", "john@anch.org"}, {"first", "John"}});
    dbCon.update("UPDATE T_Test SET email = :email WHERE first_name = :first",
		 anch::sql::Parameters().add("first", "Jane").addNull("email"));
    //! [parameters]

    // A transaction is committed only by commit(): an unfinished transaction is rolled back when the connection is
    // released (returned to its pool or destroyed), even without SQL error
    dbCon.begin();
    try {
      const std::string insert = "INSERT INTO T_Test (first_name,last_name,birth_date) VALUES (:first,:last,:birth)";
      dbCon.update(insert, {{"first", "Jane"}, {"last", "Doe"}, {"birth", "1971-01-01"}});
      dbCon.update(insert, {{"first", "Jack"}, {"last", "Doe"}, {"birth", "1972-01-01"}});
      dbCon.commit();
    } catch(...) {
      dbCon.rollback();
      throw;
    }

    // Parameters from the fields of a described object (the other fields are ignored)
    //! [fields]
    Person joe;
    joe.setFirstName("Joe");
    joe.setLastName("Doe");
    joe.setBirthDate("1973-01-01");
    joe.setEmail("joe@anch.org");
    dbCon.update("INSERT INTO T_Test (first_name,last_name,birth_date,email) VALUES (:first_name,:last_name,:birth_date,:email)",
		 anch::sql::fieldsOf(joe));
    //! [fields]

    std::cout << std::endl;

    std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' with manual result handling" << std::endl;
    ResultSet* res = dbCon.query("SELECT id,first_name,last_name,birth_date,email FROM T_Test");
    int row = 0;
    std::string nullStr("NULL");
    while(res->next()) {
      std::cout << "Row " << row << std::endl;
      std::optional<uint32_t> id = res->get<uint32_t>(0);
      std::cout << "id=" << (id ? std::to_string(*id) : nullStr) << std::endl;
      std::optional<std::string> fName = res->get<std::string>(1);
      std::cout << "first name=" << fName.value_or(nullStr) << std::endl;
      std::optional<std::string> lName = res->get<std::string>(2);
      std::cout << "last name=" << lName.value_or(nullStr) << std::endl;
      std::optional<std::string> bDate = res->get<std::string>("birth_date");
      std::cout << "birth date=" << bDate.value_or(nullStr) << std::endl;
      std::optional<std::string> email = res->get<std::string>(4);
      std::cout << "email=" << email.value_or(nullStr) << std::endl;
      row++;
    }
    delete res;

    std::cout << std::endl;

    std::cout << "Execute 'SELECT id,first_name,last_name,birth_date,email FROM T_Test' with per row result handling" << std::endl;
    std::list<Person> persons;
    dbCon.queryMapRow("SELECT id,first_name,last_name,birth_date,email FROM T_Test", [&persons](ResultSet& resSet) {
	Person person;
	resSet.get<uint32_t>(0,person._id);
	resSet.get<std::string>(1, person._firstName);
	resSet.get<std::string>(2, person._lastName);
	resSet.get<std::string>(3, person._birthDate);
	resSet.get<std::string>(4, person._email);
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
	  resSet.get<std::string>(1, pers._firstName);
	  resSet.get<std::string>(2, pers._lastName);
	  resSet.get<std::string>(3, pers._birthDate);
	  resSet.get<std::string>(4, pers._email);
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

  std::cout << "Exit PostgreSQL unit tests" << std::endl;
  return 0;
}
//! [example]
