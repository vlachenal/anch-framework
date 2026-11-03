#include "sql/placeholders.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::SqlSyntax;
using anch::sql::findPlaceholders;
using anch::sql::numberPlaceholders;

/*!
 * Check placeholders positions
 */
void
checkPositions(const std::string& query, const SqlSyntax& syntax, const std::vector<std::size_t>& expected) {
  std::vector<std::size_t> pos = findPlaceholders(query, syntax);
  std::string found;
  for(std::size_t p : pos) {
    found += std::to_string(p) + ' ';
  }
  anch::ut::assertThat(pos == expected, "Query {} : found positions {}", query, found);
}

/*!
 * Check placeholders count and PostgreSQL numbering
 */
void
checkNumbered(const std::string& query, const SqlSyntax& syntax, const std::string& expected) {
  std::string numbered = numberPlaceholders(query, findPlaceholders(query, syntax));
  anch::ut::assertThat(numbered == expected, "Query {} : numbered as {} instead of {}", query, numbered, expected);
}

void
testLegacy() {
  // Cases of the previous implementation (backslash escapes: MySQL syntax)
  checkPositions("SELECT * FROM toto WHERE a = ? AND b = ?", SqlSyntax::STANDARD, {29, 39});
  checkPositions("SELECT * FROM toto WHERE a = ? AND b = ? AND c = 'tata'", SqlSyntax::STANDARD, {29, 39});
  checkPositions("SELECT * FROM toto WHERE a = ? AND c = 'tata' AND b = ?", SqlSyntax::STANDARD, {29, 54});
  checkPositions("SELECT * FROM toto WHERE a = ? AND c = 'ta''ta' AND b = ?", SqlSyntax::STANDARD, {29, 56});
  checkPositions("SELECT * FROM toto WHERE a = ? AND c = 'ta\\'ta' AND b = ?", SqlSyntax::MYSQL, {29, 56});
  checkPositions("SELECT * FROM toto WHERE a = ? AND c = 'tata''' AND b = ?", SqlSyntax::STANDARD, {29, 56});
  checkPositions("SELECT * FROM toto WHERE a = ? AND c = '''tata' AND b = ?", SqlSyntax::STANDARD, {29, 56});
  checkPositions("INSERT INTO toto (a,b) VALUES (?,?)", SqlSyntax::STANDARD, {31, 33});
  checkPositions("INSERT INTO toto (a,b,c) VALUES (?,'tata',?)", SqlSyntax::STANDARD, {33, 42});
  checkPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta''ta',?)", SqlSyntax::STANDARD, {33, 44});
  checkPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta\\'ta',?)", SqlSyntax::MYSQL, {33, 44});
  checkPositions("INSERT INTO toto (a,b,c) VALUES (?,'tata''',?)", SqlSyntax::STANDARD, {33, 44});
  checkPositions("INSERT INTO toto (a,b,c) VALUES (?,'''tata',?)", SqlSyntax::STANDARD, {33, 44});
  checkPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta(ta',?)", SqlSyntax::STANDARD, {33, 43});
  checkPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta)ta',?)", SqlSyntax::STANDARD, {33, 43});
  checkPositions("SELECT * FROM toto WHERE a IN (?, ?, ?) AND c = '''tata' AND b = ?", SqlSyntax::STANDARD, {31, 34, 37, 65});
  checkPositions("SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ?", SqlSyntax::STANDARD, {31, 46, 74});
}

void
testOperators() {
  // Placeholders were only found after '=' or in parentheses: builder clauses were not parsed
  checkNumbered("SELECT * FROM t WHERE a <> ? AND b > ? AND c < ? AND d >= ? AND e <= ? AND f LIKE ? AND g NOT LIKE ? AND h BETWEEN ? AND ? LIMIT ? OFFSET ?",
		SqlSyntax::POSTGRESQL,
		"SELECT * FROM t WHERE a <> $1 AND b > $2 AND c < $3 AND d >= $4 AND e <= $5 AND f LIKE $6 AND g NOT LIKE $7 AND h BETWEEN $8 AND $9 LIMIT $10 OFFSET $11");
  checkNumbered("UPDATE t SET a = ?, b = COALESCE(?, b) + ? WHERE c IN (SELECT d FROM u WHERE e = ?)", SqlSyntax::POSTGRESQL,
		"UPDATE t SET a = $1, b = COALESCE($2, b) + $3 WHERE c IN (SELECT d FROM u WHERE e = $4)");
  checkNumbered("SELECT CASE WHEN ? THEN 1 END, ?::int", SqlSyntax::POSTGRESQL, "SELECT CASE WHEN $1 THEN 1 END, $2::int");
}

void
testComments() {
  checkNumbered("SELECT a -- b = ?\nFROM t WHERE c = ? /* d = ? */", SqlSyntax::POSTGRESQL, "SELECT a -- b = ?\nFROM t WHERE c = $1 /* d = ? */");
  // PostgreSQL block comments are nested
  checkNumbered("SELECT /* a /* ? */ ? */ ?", SqlSyntax::POSTGRESQL, "SELECT /* a /* ? */ ? */ $1");
  checkPositions("SELECT /* a /* ? */ ? */ ?", SqlSyntax::STANDARD, {20, 25});
  // MySQL: '#' comment, '--' only followed by a space
  checkPositions("SELECT a--1, ? # ?\n, ? -- ?", SqlSyntax::MYSQL, {13, 21});
  checkPositions("SELECT a -- ?", SqlSyntax::POSTGRESQL, {});
}

void
testQuotedIdentifiers() {
  checkPositions("SELECT \"a?\", \"b\"\"?\" FROM t WHERE c = ?", SqlSyntax::POSTGRESQL, {37});
  checkPositions("SELECT `a?`, [b?] FROM t WHERE c = ?", SqlSyntax::SQLITE, {35});
  // MySQL: double quotes delimit strings with backslash escapes
  checkPositions("SELECT * FROM t WHERE a = \"x\\\"?\" AND b = ?", SqlSyntax::MYSQL, {41});
}

void
testBackslash() {
  // Backslash is not an escape character in standard strings (PostgreSQL, SQLite): 'x\' is a complete literal
  checkPositions("SELECT * FROM t WHERE a = 'x\\' AND b = ?", SqlSyntax::POSTGRESQL, {39});
  checkPositions("SELECT * FROM t WHERE a = 'x\\' AND b = ?", SqlSyntax::SQLITE, {39});
  // MySQL: the literal is not terminated, the rest of the query is hidden
  checkPositions("SELECT * FROM t WHERE a = 'x\\' AND b = ?", SqlSyntax::MYSQL, {});
  // PostgreSQL escape strings
  checkPositions("SELECT * FROM t WHERE a = E'x\\' ?' AND b = ?", SqlSyntax::POSTGRESQL, {43});
  checkPositions("SELECT * FROM t WHERE a = e'x\\\\' AND b = ?", SqlSyntax::POSTGRESQL, {41});
}

void
testDollarQuotes() {
  checkNumbered("SELECT $$ ? $$, $tag$ it's ? $tag$, $1, a$b, ? FROM t WHERE c = ?", SqlSyntax::POSTGRESQL,
		"SELECT $$ ? $$, $tag$ it's ? $tag$, $1, a$b, $1 FROM t WHERE c = $2");
  // Dollar quotes are not delimiters for other dialects
  checkPositions("SELECT $$ ? $$", SqlSyntax::STANDARD, {10});
}

void
testUnterminated() {
  checkPositions("SELECT ? FROM t WHERE a = 'unterminated ?", SqlSyntax::STANDARD, {7});
  checkPositions("SELECT ? /* unterminated ?", SqlSyntax::POSTGRESQL, {7});
  checkPositions("", SqlSyntax::POSTGRESQL, {});
  checkNumbered("?", SqlSyntax::POSTGRESQL, "$1");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH SQL placeholders unit tests")
    .description("Test '?' placeholders scanner of prepared statements")
    .add("legacy", testLegacy)
    .add("operators", testOperators)
    .add("comments", testComments)
    .add("quoted-identifiers", testQuotedIdentifiers)
    .add("backslash", testBackslash)
    .add("dollar-quotes", testDollarQuotes)
    .add("unterminated", testUnterminated)
    ;
}
