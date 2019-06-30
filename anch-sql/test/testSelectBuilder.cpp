#include "sql/builder/select.hpp"

#include <map>
#include <functional>
#include <iostream>
#include <assert.h>

using anch::sql::SQLQuery;

std::map<std::string, std::function<void()>> TEST_SUITE;


void
testNoFromNoWhere() {
  SQLQuery query = anch::sql::select().field("nextval()").build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT nextval()");
  assert(query.getValues().size() == 0);
}

void
testQuerySingleColSingleTableWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("titi")
    .from("toto")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT titi FROM toto");
  assert(query.getValues().size() == 0);
}

void
testQueryDistinctSingleColSingleTableWithoutWhere() {
  SQLQuery query = anch::sql::select().distinct()
    .field("titi")
    .from("toto")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT DISTINCT titi FROM toto");
  assert(query.getValues().size() == 0);
}

void
testQuerySingleColWithAliasSingleTableWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("titi").as("t")
    .from("toto")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT titi AS t FROM toto");
  assert(query.getValues().size() == 0);
}

void
testQuerySingleTableWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("titi")
    .field("tata")
    .from("toto")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT titi,tata FROM toto");
  assert(query.getValues().size() == 0);
}

void
testQueryWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("t.titi")
    .field("u.tata")
    .from("toto t")
    .selfJoin("tutu u")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT t.titi,u.tata FROM toto t,tutu u");
  assert(query.getValues().size() == 0);
}

void
testQueryWithSubqueryWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("t.titi")
    .field("u.tata")
    .from("toto t")
    .selfJoin(anch::sql::select().field("*").from("tutu").done(), "u")
    .selfJoin(anch::sql::select().field("*").from("titi").build(), "i")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT t.titi,u.tata "
	 "FROM toto t,"
	 "(SELECT * FROM tutu) u,"
	 "(SELECT * FROM titi) i");
  assert(query.getValues().size() == 0);
}

void
testQueryNaturalJoinWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("t.titi")
    .field("u.tata")
    .from("toto t")
    .naturalJoin("tutu u")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT t.titi,u.tata FROM toto t NATURAL JOIN tutu u");
  assert(query.getValues().size() == 0);
}

void
testQueryNaturalJoinWithSubqueryWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("t.titi")
    .field("u.tata")
    .from("toto t")
    .naturalJoin(anch::sql::select().field("*").from("tutu").done(), "u")
    .naturalJoin(anch::sql::select().field("*").from("titi").build(), "i")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT t.titi,u.tata "
	 "FROM toto t "
	 "NATURAL JOIN (SELECT * FROM tutu) u "
	 "NATURAL JOIN (SELECT * FROM titi) i");
  assert(query.getValues().size() == 0);
}

void
testQueryCrossJoinWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("t.titi")
    .field("u.tata")
    .from("toto t")
    .crossJoin("tutu u")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT t.titi,u.tata FROM toto t CROSS JOIN tutu u");
  assert(query.getValues().size() == 0);
}

void
testQueryCrossJoinWithSubqueryWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("t.titi")
    .field("u.tata")
    .from("toto t")
    .crossJoin(anch::sql::select().field("*").from("tutu").done(), "u")
    .crossJoin(anch::sql::select().field("*").from("titi").build(), "i")
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT t.titi,u.tata "
	 "FROM toto t "
	 "CROSS JOIN (SELECT * FROM tutu) u "
	 "CROSS JOIN (SELECT * FROM titi) i");
  assert(query.getValues().size() == 0);
}

void
registerTests() {
  TEST_SUITE["0_from_no_where"] = testNoFromNoWhere;
  TEST_SUITE["1_from_no_where"] = testQuerySingleColSingleTableWithoutWhere;
  TEST_SUITE["distinct_1_from_no_where"] = testQueryDistinctSingleColSingleTableWithoutWhere;
  TEST_SUITE["1_as_from_no_where"] = testQuerySingleColWithAliasSingleTableWithoutWhere;
  TEST_SUITE["2_from_no_where"] = testQuerySingleTableWithoutWhere;
  TEST_SUITE["self_join_no_where"] = testQueryWithoutWhere;
  TEST_SUITE["self_join_subquery_no_where"] = testQueryWithSubqueryWithoutWhere;
  TEST_SUITE["natural_join_no_where"] = testQueryNaturalJoinWithoutWhere;
  TEST_SUITE["natural_join_subquery_no_where"] = testQueryNaturalJoinWithSubqueryWithoutWhere;
  TEST_SUITE["cross_join_no_where"] = testQueryCrossJoinWithoutWhere;
  TEST_SUITE["cross_join_subquery_no_where"] = testQueryCrossJoinWithSubqueryWithoutWhere;
}

int
main(int argc, char** argv) {
  registerTests();

  bool allTests = true;
  if(argc > 1) {
    allTests = false;
  }

  if(allTests) {
    std::cout << "Run all tests" << std::endl;
    for(auto iter = TEST_SUITE.cbegin() ; iter != TEST_SUITE.cend() ; ++iter) {
      std::cout << "Run unit test: " << iter->first << std::endl;
      iter->second();
    }

  } else {
    for(int i = 1 ; i < argc ; ++i) {
      std::cout << "Run unit test: " << argv[i] << std::endl;
      auto iter = TEST_SUITE.find(argv[i]);
      if(iter == TEST_SUITE.cend()) {
	std::cout << "Test " << argv[i] << " does not exist. CONTINUE." << std::endl;
      } else {
	iter->second();
      }
    }
  }

  return 0;
}
