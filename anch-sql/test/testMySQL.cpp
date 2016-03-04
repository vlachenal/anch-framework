#include "sql/mysqlConnection.hpp"

#include <iostream>

using anch::sql::Connection;
using anch::sql::MySQLConnection;
using anch::sql::SqlException;
using anch::sql::ResultSet;

int
main(void) {
  std::cout << "Enter in MySQL unit tests" << std::endl;

  try {
    MySQLConnection dbCon("localhost",
			  "anch",
			  "anch",
			  "anch_test",
			  3306);
    std::cout << "Connected to database" << std::endl;
    dbCon.setAutoCommit(false);
    std::cout << "Auto commit set to false" << std::endl;
    dbCon.commit();
    std::cout << "Commit no transaction ..." << std::endl;
    dbCon.rollback();
    std::cout << "Rollback no transaction ..." << std::endl;
    dbCon.setAutoCommit(true);

    ResultSet* res = dbCon.query("SELECT id,first_name,last_name,birth_date,email FROM T_Test");
    int row = 0;
    std::string nullStr("NULL");
    while(res->hasNext()) {
      res->next();
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
      std::cout << "birth date=" << (email == NULL ? nullStr : *email) << std::endl;
      delete email;
      row++;
    }
    delete res;

  } catch(const SqlException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Exit MySQL unit tests" << std::endl;
  return 0;
}
