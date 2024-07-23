#include "sql/builder/sqlFunctions.hpp"
#include "sql/builder/clauses.hpp"

#include <map>
#include <functional>
#include <iostream>
#include <assert.h>
#include <any>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::SQLQuery;

// std::map<std::string, std::function<void()>> TEST_SUITE;


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
testQueryInnerJoinWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("t.titi")
    .field("u.tata")
    .from("toto t")
    .innerJoin("tutu u", anch::sql::clauses().field("t.i").equals().field("u.a"))
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT t.titi,u.tata FROM toto t INNER JOIN tutu u ON t.i = u.a");
  assert(query.getValues().size() == 0);
}

void
testQueryJoinWithoutValidClause() {
  //std::string val("");
  SQLQuery query = anch::sql::select()
    .field("t.titi")
    .field("u.tata")
    .from("toto t")
    .innerJoin("tutu u", anch::sql::clauses().field("t.i").equals().field("u.a"))
    //.leftOuterJoin("tata a", anch::sql::clauses("a.a", anch::sql::equals, std::string("")))
    //.leftOuterJoin("tata a", anch::sql::clauses("a.a", anch::sql::equals, std::optional<std::string>()))
    .build();
  std::cout << query.getQuery() << std::endl;
  assert(query.getQuery() == "SELECT t.titi,u.tata FROM toto t INNER JOIN tutu u ON t.i = u.a");
  assert(query.getValues().size() == 0);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH select builder unit tests")
    .description("Test AnCH select builder library")
    .add("0_from_no_where", testNoFromNoWhere)
    .add("1_from_no_where", testQuerySingleColSingleTableWithoutWhere)
    .add("distinct_1_from_no_where", testQueryDistinctSingleColSingleTableWithoutWhere)
    .add("1_as_from_no_where", testQuerySingleColWithAliasSingleTableWithoutWhere)
    .add("2_from_no_where", testQuerySingleTableWithoutWhere)
    .add("self_join_no_where", testQueryWithoutWhere)
    .add("self_join_subquery_no_where", testQueryWithSubqueryWithoutWhere)
    .add("natural_join_no_where", testQueryNaturalJoinWithoutWhere)
    .add("natural_join_subquery_no_where", testQueryNaturalJoinWithSubqueryWithoutWhere)
    .add("cross_join_no_where", testQueryCrossJoinWithoutWhere)
    .add("cross_join_subquery_no_where", testQueryCrossJoinWithSubqueryWithoutWhere)
    .add("inner_join_no_where", testQueryInnerJoinWithoutWhere)
    .add("inner_join_without_valid_clause", testQueryJoinWithoutValidClause)
    ;
}
