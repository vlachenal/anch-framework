/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2019 Vincent Lachenal

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
#include "sql/builder/sqlFunctions.hpp"
#include "sql/builder/clauses.hpp"

#include <map>
#include <functional>
#include <iostream>
#include <any>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::SQLQuery;
using anch::ut::assertTrue;


void
testNoFromNoWhere() {
  SQLQuery query = anch::sql::select().field("nextval()").build();
  std::cout << query.getQuery() << std::endl;
  assertTrue(query.getQuery() == "SELECT nextval()");
  assertTrue(query.getParameters().values().empty());
}

void
testQuerySingleColSingleTableWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("titi")
    .from("toto")
    .build();
  std::cout << query.getQuery() << std::endl;
  assertTrue(query.getQuery() == "SELECT titi FROM toto");
  assertTrue(query.getParameters().values().empty());
}

void
testQueryDistinctSingleColSingleTableWithoutWhere() {
  SQLQuery query = anch::sql::select().distinct()
    .field("titi")
    .from("toto")
    .build();
  std::cout << query.getQuery() << std::endl;
  assertTrue(query.getQuery() == "SELECT DISTINCT titi FROM toto");
  assertTrue(query.getParameters().values().empty());
}

void
testQuerySingleColWithAliasSingleTableWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("titi").as("t")
    .from("toto")
    .build();
  std::cout << query.getQuery() << std::endl;
  assertTrue(query.getQuery() == "SELECT titi AS t FROM toto");
  assertTrue(query.getParameters().values().empty());
}

void
testQuerySingleTableWithoutWhere() {
  SQLQuery query = anch::sql::select()
    .field("titi")
    .field("tata")
    .from("toto")
    .build();
  std::cout << query.getQuery() << std::endl;
  assertTrue(query.getQuery() == "SELECT titi,tata FROM toto");
  assertTrue(query.getParameters().values().empty());
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
  assertTrue(query.getQuery() == "SELECT t.titi,u.tata FROM toto t,tutu u");
  assertTrue(query.getParameters().values().empty());
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
  assertTrue(query.getQuery() == "SELECT t.titi,u.tata "
	     "FROM toto t,"
	     "(SELECT * FROM tutu) u,"
	     "(SELECT * FROM titi) i");
  assertTrue(query.getParameters().values().empty());
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
  assertTrue(query.getQuery() == "SELECT t.titi,u.tata FROM toto t NATURAL JOIN tutu u");
  assertTrue(query.getParameters().values().empty());
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
  assertTrue(query.getQuery() == "SELECT t.titi,u.tata "
	 "FROM toto t "
	 "NATURAL JOIN (SELECT * FROM tutu) u "
	 "NATURAL JOIN (SELECT * FROM titi) i");
  assertTrue(query.getParameters().values().empty());
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
  assertTrue(query.getQuery() == "SELECT t.titi,u.tata FROM toto t CROSS JOIN tutu u");
  assertTrue(query.getParameters().values().empty());
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
  assertTrue(query.getQuery() == "SELECT t.titi,u.tata "
	 "FROM toto t "
	 "CROSS JOIN (SELECT * FROM tutu) u "
	 "CROSS JOIN (SELECT * FROM titi) i");
  assertTrue(query.getParameters().values().empty());
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
  assertTrue(query.getQuery() == "SELECT t.titi,u.tata FROM toto t INNER JOIN tutu u ON t.i = u.a");
  assertTrue(query.getParameters().values().empty());
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
  assertTrue(query.getQuery() == "SELECT t.titi,u.tata FROM toto t INNER JOIN tutu u ON t.i = u.a");
  assertTrue(query.getParameters().values().empty());
}

namespace {

  void
  checkParameters(const SQLQuery& query, const std::vector<std::pair<std::string, std::string>>& expected) {
    std::vector<std::pair<std::string, std::string>> found;
    std::string text;
    for(const anch::sql::Parameter& parameter : query.getParameters().values()) {
      found.emplace_back(parameter.name, parameter.value.value_or("NULL"));
      text += parameter.name + '=' + parameter.value.value_or("NULL") + ' ';
    }
    anch::ut::assertThat(found == expected, "Unexpected parameters '{}' of query '{}'", text, query.getQuery());
  }

}

void
testWhereValues() {
  // Parameters are named from their columns ; clauses of absent values are skipped
  int64_t min = 1;
  std::optional<std::string> email;
  SQLQuery query = anch::sql::select()
    .field("id")
    .from("T_Test t")
    .where(anch::sql::clauses()
	   .AND("t.id", anch::sql::greater, min)
	   .AND("t.id", anch::sql::between, 2, 5)
	   .AND("last_name", anch::sql::like, std::string("%o%"))
	   .AND("email", anch::sql::equals, email)
	   .AND("first_name", anch::sql::equals, std::string())
	   .OR("t.id", anch::sql::equals, 9))
    .build();
  anch::ut::assertThat(query.getQuery() == "SELECT id FROM T_Test t WHERE t.id > :t_id AND t.id BETWEEN :t_id_min AND :t_id_max"
		       " AND last_name LIKE :last_name OR t.id = :t_id_2", "Unexpected query '{}'", query.getQuery());
  checkParameters(query, {{"t_id", "1"}, {"t_id_min", "2"}, {"t_id_max", "5"}, {"last_name", "%o%"}, {"t_id_2", "9"}});
}

void
testMergedNames() {
  // Names of merged parts which are already used are renamed
  SQLQuery query = anch::sql::select()
    .field("a")
    .from("t")
    .where(anch::sql::clauses()
	   .AND("id", anch::sql::equals, 1)
	   .AND(anch::sql::clauses().AND("id", anch::sql::equals, 2).OR("id", anch::sql::equals, 3)))
    .build();
  anch::ut::assertThat(query.getQuery() == "SELECT a FROM t WHERE id = :id AND (id = :id_2 OR id = :id_2_2)", "Unexpected query '{}'",
		       query.getQuery());
  checkParameters(query, {{"id", "1"}, {"id_2", "2"}, {"id_2_2", "3"}});
  // Join on a subquery
  query = anch::sql::select()
    .field("t.a")
    .from("t")
    .innerJoin(anch::sql::select().field("*").from("u").where(anch::sql::clauses().AND("id", anch::sql::equals, 2)), "s",
	       anch::sql::clauses().field("s.id").equals().field("t.id").AND("id", anch::sql::equals, 1))
    .build();
  anch::ut::assertThat(query.getQuery() == "SELECT t.a FROM t INNER JOIN (SELECT * FROM u WHERE id = :id) s ON s.id = t.id AND id = :id_2",
		       "Unexpected query '{}'", query.getQuery());
  checkParameters(query, {{"id", "2"}, {"id_2", "1"}});
  // Union (the parameters of the other query were lost)
  anch::sql::SelectBuilder other = anch::sql::select();
  other.field("a").from("u").where(anch::sql::clauses().AND("a", anch::sql::equals, 2));
  anch::sql::SelectBuilder first = anch::sql::select();
  first.field("a").from("t").where(anch::sql::clauses().AND("a", anch::sql::equals, 1));
  query = first.unionAll(other).build();
  anch::ut::assertThat(query.getQuery() == "SELECT a FROM t WHERE a = :a UNION ALL SELECT a FROM u WHERE a = :a_2", "Unexpected query '{}'",
		       query.getQuery());
  checkParameters(query, {{"a", "1"}, {"a_2", "2"}});
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
    .add("where_values", testWhereValues)
    .add("merged_names", testMergedNames)
    ;
}
