/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2017 Vincent Lachenal

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
#include "sql/placeholders.hpp"

#include <string>
#include <vector>

#include "sql/sqlException.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::NamedQuery;
using anch::sql::ParameterStyle;
using anch::sql::SqlException;
using anch::sql::SqlSyntax;

using Binding = anch::sql::NamedQuery::Binding;

namespace {

  constexpr std::size_t SCALAR = NamedQuery::SCALAR;

  void
  checkNames(const std::string& query, const SqlSyntax& syntax, const std::vector<std::string>& expected) {
    NamedQuery named(query, syntax);
    std::string found;
    for(const std::string& name : named.names()) {
      found += name + ' ';
    }
    anch::ut::assertThat(named.names() == expected, "Query '{}': found names '{}'", query, found);
  }

  void
  checkRendered(const std::string& query, const SqlSyntax& syntax, ParameterStyle style, const std::vector<std::size_t>& sizes,
		const std::string& expected, const std::vector<Binding>& bindings) {
    NamedQuery::Rendered rendered = NamedQuery(query, syntax).render(style, sizes);
    anch::ut::assertThat(rendered.query == expected, "Query '{}' rendered as '{}' instead of '{}'", query, rendered.query, expected);
    std::string found;
    for(const Binding& binding : rendered.bindings) {
      found += std::to_string(binding.slot) + ':' + (binding.element == SCALAR ? std::string("-") : std::to_string(binding.element)) + ' ';
    }
    anch::ut::assertThat(rendered.bindings == bindings, "Query '{}': unexpected bindings '{}'", query, found);
  }

  void
  checkRefused(const std::string& query, const SqlSyntax& syntax, const std::string& expected) {
    try {
      NamedQuery named(query, syntax);
    } catch(const SqlException& error) {
      anch::ut::assertThat(std::string(error.what()).find(expected) != std::string::npos, "Query '{}': unexpected error '{}'", query,
			   error.what());
      return;
    }
    anch::ut::fail("Query '" + query + "' has to be refused");
  }

  void
  checkRenderRefused(const std::string& query, const SqlSyntax& syntax, ParameterStyle style, const std::vector<std::size_t>& sizes,
		     const std::string& expected) {
    NamedQuery named(query, syntax);
    try {
      named.render(style, sizes);
    } catch(const SqlException& error) {
      anch::ut::assertThat(std::string(error.what()).find(expected) != std::string::npos, "Query '{}': unexpected error '{}'", query,
			   error.what());
      return;
    }
    anch::ut::fail("Rendering of query '" + query + "' has to be refused");
  }

}

void
testNames() {
  const std::string query = "SELECT * FROM pet WHERE id = :id AND name = :name";
  checkNames(query, SqlSyntax::STANDARD, {"id", "name"});
  checkRendered(query, SqlSyntax::POSTGRESQL, ParameterStyle::DOLLAR, {}, "SELECT * FROM pet WHERE id = $1 AND name = $2",
		{{0, SCALAR}, {1, SCALAR}});
  checkRendered(query, SqlSyntax::SQLITE, ParameterStyle::NUMBERED, {}, "SELECT * FROM pet WHERE id = ?1 AND name = ?2",
		{{0, SCALAR}, {1, SCALAR}});
  checkRendered(query, SqlSyntax::MYSQL, ParameterStyle::POSITIONAL, {}, "SELECT * FROM pet WHERE id = ? AND name = ?",
		{{0, SCALAR}, {1, SCALAR}});
  checkNames("INSERT INTO pet(id, name) VALUES(:id,:name_2)", SqlSyntax::STANDARD, {"id", "name_2"});
  checkNames("SELECT 1", SqlSyntax::STANDARD, {});
  NamedQuery named(query, SqlSyntax::STANDARD);
  anch::ut::assertThat(named.slot("name") == 1 && !named.slot("other") && named.query() == query, "Unexpected slots");
}

void
testRepeated() {
  // A name used several times is bound once (MySQL: each '?' is bound to the same value)
  const std::string query = "SELECT * FROM pet WHERE owner = :id OR (creator = :id AND :status IS NULL OR status = :status)";
  checkNames(query, SqlSyntax::STANDARD, {"id", "status"});
  checkRendered(query, SqlSyntax::POSTGRESQL, ParameterStyle::DOLLAR, {},
		"SELECT * FROM pet WHERE owner = $1 OR (creator = $1 AND $2 IS NULL OR status = $2)", {{0, SCALAR}, {1, SCALAR}});
  checkRendered(query, SqlSyntax::SQLITE, ParameterStyle::NUMBERED, {},
		"SELECT * FROM pet WHERE owner = ?1 OR (creator = ?1 AND ?2 IS NULL OR status = ?2)", {{0, SCALAR}, {1, SCALAR}});
  checkRendered(query, SqlSyntax::MYSQL, ParameterStyle::POSITIONAL, {},
		"SELECT * FROM pet WHERE owner = ? OR (creator = ? AND ? IS NULL OR status = ?)",
		{{0, SCALAR}, {0, SCALAR}, {1, SCALAR}, {1, SCALAR}});
}

void
testIgnored() {
  // String literals, quoted identifiers and comments
  checkNames("SELECT ':a', \":b\", c FROM t -- :d\nWHERE e = :e /* :f */", SqlSyntax::POSTGRESQL, {"e"});
  checkNames("SELECT 'it''s :a' FROM t WHERE b = :b", SqlSyntax::STANDARD, {"b"});
  checkNames("SELECT `:a`, \":b\", 'x\\' :c' FROM t # :d\nWHERE e = :e", SqlSyntax::MYSQL, {"e"});
  checkNames("SELECT [:a], `:b` FROM t WHERE c = :c", SqlSyntax::SQLITE, {"c"});
  checkNames("SELECT $$ :a $$, $tag$ :b $tag$, E'\\' :c' FROM t WHERE d = :d /* /* :e */ :f */", SqlSyntax::POSTGRESQL, {"d"});
  // Casts, assignments, words
  checkNames("SELECT x::int, y::text[] FROM t WHERE z = :z::bigint", SqlSyntax::POSTGRESQL, {"z"});
  checkNames("SET @total := :total", SqlSyntax::MYSQL, {"total"});
  checkNames("SELECT a:b, 1:2, :3 FROM t WHERE c = :c", SqlSyntax::STANDARD, {"c"});
  // PostgreSQL array slices: ':' in brackets are not parameters, parameters in brackets are in parentheses
  checkNames("SELECT arr[1:2], arr[:n], arr[n:], arr[(:low):(:high)], m[1][:k] FROM t WHERE x = :x", SqlSyntax::POSTGRESQL,
	     {"low", "high", "x"});
  // jsonb operators are not parameters in PostgreSQL
  checkNames("SELECT * FROM t WHERE data ? 'key' AND data ?| array['a'] AND data ?& :keys", SqlSyntax::POSTGRESQL, {"keys"});
}

void
testPositionalRefused() {
  checkRefused("SELECT * FROM t WHERE a = ?", SqlSyntax::MYSQL, "positional parameters ('?') are not supported");
  checkRefused("SELECT * FROM t WHERE a = :a AND b = ?", SqlSyntax::STANDARD, "found at offset 37");
  checkRefused("SELECT * FROM t WHERE a = ?1", SqlSyntax::SQLITE, "('?')");
  checkRefused("SELECT * FROM t WHERE a = @a", SqlSyntax::SQLITE, "('@name', '$name')");
  checkRefused("SELECT * FROM t WHERE a = $a", SqlSyntax::SQLITE, "('@name', '$name')");
  checkRefused("SELECT * FROM t WHERE a = $1", SqlSyntax::POSTGRESQL, "('$1')");
  // Accepted: in literals, MySQL user variables, PostgreSQL dollar quotes
  checkNames("SELECT '?', @total FROM t WHERE a = :a", SqlSyntax::MYSQL, {"a"});
  checkNames("SELECT $q$ $1 :a $q$ FROM t", SqlSyntax::POSTGRESQL, {});
}

void
testLists() {
  const std::string query = "SELECT * FROM pet WHERE kind = :kind AND id IN (:ids) AND owner NOT IN ( :owners ) AND name = :kind";
  checkNames(query, SqlSyntax::STANDARD, {"kind", "ids", "owners"});
  // PostgreSQL: arrays
  checkRendered(query, SqlSyntax::POSTGRESQL, ParameterStyle::DOLLAR, {SCALAR, 3, 0},
		"SELECT * FROM pet WHERE kind = $1 AND id = ANY($2) AND owner <> ALL($3) AND name = $1",
		{{0, SCALAR}, {1, SCALAR}, {2, SCALAR}});
  // SQLite: numbered expansion, empty list as empty subquery
  checkRendered(query, SqlSyntax::SQLITE, ParameterStyle::NUMBERED, {SCALAR, 3, 0},
		"SELECT * FROM pet WHERE kind = ?1 AND id IN (?2, ?3, ?4) AND owner NOT IN (SELECT NULL FROM (SELECT 1) AS anch_empty WHERE 1 = 0)"
		" AND name = ?1", {{0, SCALAR}, {1, 0}, {1, 1}, {1, 2}});
  // MySQL: positional expansion
  checkRendered(query, SqlSyntax::MYSQL, ParameterStyle::POSITIONAL, {SCALAR, 2, 1},
		"SELECT * FROM pet WHERE kind = ? AND id IN (?, ?) AND owner NOT IN (?) AND name = ?",
		{{0, SCALAR}, {1, 0}, {1, 1}, {2, 0}, {0, SCALAR}});
  // Value which is not a list in an IN list ; keywords case
  checkRendered("SELECT * FROM pet WHERE id in(:id)", SqlSyntax::POSTGRESQL, ParameterStyle::DOLLAR, {}, "SELECT * FROM pet WHERE id in($1)",
		{{0, SCALAR}});
  checkRendered("SELECT * FROM pet WHERE id not  in (:ids)", SqlSyntax::POSTGRESQL, ParameterStyle::DOLLAR, {2},
		"SELECT * FROM pet WHERE id <> ALL($1)", {{0, SCALAR}});
  // PostgreSQL array anywhere
  checkRendered("SELECT * FROM pet WHERE id = ANY(:ids)", SqlSyntax::POSTGRESQL, ParameterStyle::DOLLAR, {2},
		"SELECT * FROM pet WHERE id = ANY($1)", {{0, SCALAR}});
  // Expanded lists have to be the only item of an IN list
  checkRenderRefused("SELECT * FROM pet WHERE id = ANY(:ids)", SqlSyntax::MYSQL, ParameterStyle::POSITIONAL, {2},
		     "list parameter 'ids' has to be the only item of an IN list");
  checkRenderRefused("SELECT * FROM pet WHERE id IN (:a, :b)", SqlSyntax::SQLITE, ParameterStyle::NUMBERED, {SCALAR, 2}, "list parameter 'b'");
  checkRenderRefused("SELECT * FROM pet WHERE MIN (:ids)", SqlSyntax::MYSQL, ParameterStyle::POSITIONAL, {2}, "list parameter 'ids'");
  checkRenderRefused("SELECT * FROM pet WHERE id = :id", SqlSyntax::MYSQL, ParameterStyle::POSITIONAL, {SCALAR, SCALAR},
		     "parameters sizes count (2) is not the parameters count (1)");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH SQL named parameters unit tests")
    .description("Test AnCH SQL named parameters parsing and rendering")
    .add("names", testNames)
    .add("repeated", testRepeated)
    .add("ignored", testIgnored)
    .add("positional-refused", testPositionalRefused)
    .add("lists", testLists)
    ;
}
