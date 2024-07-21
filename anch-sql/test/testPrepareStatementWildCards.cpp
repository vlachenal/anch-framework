#include "sql/preparedStatement.hpp"

#include <iostream>
#include <sstream>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::sql::SqlException;
using anch::sql::ResultSet;

class PrepareStatementMock: public anch::sql::PreparedStatement {

public:
  PrepareStatementMock(): anch::sql::PreparedStatement() {}
  ~PrepareStatementMock() {}
  virtual ResultSet* executeQuery() override {return NULL;}
  virtual std::size_t executeUpdate() override {return 0;}
  std::set<std::size_t> getPositions(const std::string& query) const {
    return getPlaceholders(query);
  }

};


void
beforeAll() {
  std::cout << "Enter in prepared statement placeholders unit tests" << std::endl;
}

void
afterAll() {
  std::cout << "Exit prepared statement placeholders unit tests" << std::endl;
}

void
test2WC() {
  std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND b = ?" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {29,39};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND b = ?");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a = ? AND b = ? has been correctly parsed" << std::endl;
}

void
test2WC1F() {
  std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND b = ? AND c = 'tata'" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {29,39};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND b = ? AND c = 'tata'");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a = ? AND b = ? AND c = 'tata' has been correctly parsed" << std::endl;
}

void
test1WC1F1WC() {
  std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = 'tata' AND b = ?" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {29,54};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = 'tata' AND b = ?");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = 'tata' AND b = ? has been correctly parsed" << std::endl;
}

void
test1WC1Fquote1WC() {
  std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = 'ta''ta' AND b = ?" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {29,56};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = 'ta''ta' AND b = ?");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = 'ta''ta' AND b = ? has been correctly parsed" << std::endl;
}

void
test1WC1Fbslash1WC() {
  std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = 'ta\\'ta' AND b = ?" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {29,56};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = 'ta\\'ta' AND b = ?");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = 'ta\\'ta' AND b = ? has been correctly parsed" << std::endl;
}

void
test1WC1Fquoteafter1WC() {
  std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = 'tata''' AND b = ?" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {29,56};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = 'tata''' AND b = ?");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = 'tata''' AND b = ? has been correctly parsed" << std::endl;
}

void
test1WC1Fquotebefore1WC() {
  std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = '''tata' AND b = ?" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {29,56};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = '''tata' AND b = ?");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = '''tata' AND b = ? has been correctly parsed" << std::endl;
}

void
testIns2WC() {
  std::cout << "Check query: INSERT INTO toto (a,b) VALUES (?,?)" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {31,33};
  std::set<std::size_t> pos = stmt.getPositions("INSERT INTO toto (a,b) VALUES (?,?)");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: INSERT INTO toto (a,b) VALUES (?,?) has been correctly parsed" << std::endl;
}

void
testIns1WC1F1WC() {
  std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'tata',?)" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {33,42};
  std::set<std::size_t> pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'tata',?)");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'tata',?) has been correctly parsed" << std::endl;
}

void
testIns1WC1Fquote1WC() {
  std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'ta''ta',?)" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {33,44};
  std::set<std::size_t> pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta''ta',?)");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'ta''ta',?) has been correctly parsed" << std::endl;
}

void
testIns1WC1Fbslash1WC() {
  std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'ta\\'ta',?)" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {33,44};
  std::set<std::size_t> pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta\\'ta',?)");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'ta\\'ta',?) has been correctly parsed" << std::endl;
}

void
testIns1WC1Fquoteafter1WC() {
  std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'tata''',?)" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {33,44};
  std::set<std::size_t> pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'tata''',?)");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'tata''',?) has been correctly parsed" << std::endl;
}

void
testIns1WC1Fquotebefore1WC() {
  std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'''tata',?)" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {33,44};
  std::set<std::size_t> pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'''tata',?)");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'''tata',?) has been correctly parsed" << std::endl;
}

void
testIns1WC1Frobracket1WC() {
  std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'ta(ta',?)" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {33,43};
  std::set<std::size_t> pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta(ta',?)");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'ta(ta',?) has been correctly parsed" << std::endl;
}

void
testIns1WC1Frcbracket1WC() {
  std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'ta)ta',?)" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {33,43};
  std::set<std::size_t> pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta)ta',?)");
  anch::ut::assertTrue(pos.size() == 2, "Should found 2 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'ta)ta',?) has been correctly parsed" << std::endl;
}

void
testIn() {
  std::cout << "Check query: SELECT * FROM toto WHERE a IN (?, ?, ?) AND c = '''tata' AND b = ?" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {31,34,37,65};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a IN (?, ?, ?) AND c = '''tata' AND b = ?");
  anch::ut::assertTrue(pos.size() == 4, "Should found 4 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a IN (?, ?, ?) AND c = '''tata' AND b = ? has been correctly parsed" << std::endl;
}

void
testIn1F() {
  std::cout << "Check query: SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ?" << std::endl;
  PrepareStatementMock stmt;
  std::set<std::size_t> expPos = {31,46,74};
  std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ?");
  anch::ut::assertTrue(pos.size() == 3, "Should found 3 placehodlers");
  anch::ut::assertTrue(expPos == pos, "Bad positions");
  std::cout << "Query: SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ? has been correctly parsed" << std::endl;
}

void
testPostgres() {
  std::cout << "Check PostgreSQL placeholders replacement" << std::endl;
  std::string query = "SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ? AND d = 'titi'";
  PrepareStatementMock stmt;
  std::set<std::size_t> pos = stmt.getPositions(query);
  std::size_t offset = 0;
  std::ostringstream oss;
  std::size_t idx = 0;
  for(auto iter = pos.cbegin() ; iter != pos.cend() ; ++iter) {
    oss << query.substr(offset, *iter - offset) << '$' << ++idx;
    offset = *iter + 1;
  }
  if(offset < query.length()) {
    oss << query.substr(offset);
  }
  std::string pgQuery = oss.str();
  if(pgQuery != "SELECT * FROM toto WHERE a IN ($1, ''''tata'', $2) AND c = '''tata' AND b = $3 AND d = 'titi'") {
    std::ostringstream oss;
    oss << "Invalid PostgreSQL replacement. Found " << pgQuery << " instead of SELECT * FROM toto WHERE a IN ($1, ''''tata'', $2) AND c = '''tata' AND b = $3 AND d = 'titi'";
    anch::ut::fail(oss.str());
  }
  std::cout << "PostgreSQL prepared statement query will be: " << pgQuery << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH prepared statement wildcards unit tests")
    .description("Test AnCH prepared statement wildcards library")
    .initialize(beforeAll)
    .uninitialize(afterAll)
    .add("select-2-wildcards", test2WC)
    .add("select-2-wildcards-1-fixed", test2WC1F)
    .add("select-1-wildcard-1-fixed-1-wildcard", test1WC1F1WC)
    .add("select-1-wildcard-1-fixed-quote-1-wildcard", test1WC1Fquote1WC)
    .add("select-1-wildcard-1-fixed-backslash-1-wildcard", test1WC1Fbslash1WC)
    .add("select-1-wildcard-1-fixed-quoteafter-1-wildcard", test1WC1Fquoteafter1WC)
    .add("select-1-wildcard-1-fixed-quotebefore-1-wildcard", test1WC1Fquotebefore1WC)
    .add("insert-2-wildcards", testIns2WC)
    .add("insert-1-wildcard-1fixed-1-wildcard", testIns1WC1F1WC)
    .add("insert-1-wildcard-1fixed-quote-1-wildcard", testIns1WC1Fquote1WC)
    .add("insert-1-wildcard-1fixed-backslash-1-wildcard", testIns1WC1Fbslash1WC)
    .add("insert-1-wildcard-1fixed-quoteafter-1-wildcard", testIns1WC1Fquoteafter1WC)
    .add("insert-1-wildcard-1fixed-quotebefore-1-wildcard", testIns1WC1Fquotebefore1WC)
    .add("insert-1-wildcard-1fixed-robracket-1-wildcard", testIns1WC1Frobracket1WC)
    .add("insert-1-wildcard-1fixed-rcbracket-1-wildcard", testIns1WC1Frcbracket1WC)
    .add("select-in", testIn)
    .add("select-in-1F", testIn1F)
    .add("select-postgres", testPostgres)
    ;
}

// int
// main(void) {
//   std::cout << "Enter in prepared statement placeholders unit tests" << std::endl << std::endl;

  // PrepareStatementMock stmt;
  // std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND b = ?" << std::endl;
  // std::set<std::size_t> expPos = {29,39};
  // std::set<std::size_t> pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND b = ?");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a = ? AND b = ? has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND b = ? AND c = 'tata'" << std::endl;
  // expPos = {29,39};
  // pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND b = ? AND c = 'tata'");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a = ? AND b = ? AND c = 'tata' has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = 'tata' AND b = ?" << std::endl;
  // expPos = {29,54};
  // pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = 'tata' AND b = ?");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = 'tata' AND b = ? has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = 'ta''ta' AND b = ?" << std::endl;
  // expPos = {29,56};
  // pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = 'ta''ta' AND b = ?");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = 'ta''ta' AND b = ? has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = 'ta\\'ta' AND b = ?" << std::endl;
  // expPos = {29,56};
  // pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = 'ta\\'ta' AND b = ?");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = 'ta\\'ta' AND b = ? has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = 'tata''' AND b = ?" << std::endl;
  // expPos = {29,56};
  // pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = 'tata''' AND b = ?");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = 'tata''' AND b = ? has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: SELECT * FROM toto WHERE a = ? AND c = '''tata' AND b = ?" << std::endl;
  // expPos = {29,56};
  // pos = stmt.getPositions("SELECT * FROM toto WHERE a = ? AND c = '''tata' AND b = ?");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a = ? AND c = '''tata' AND b = ? has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: INSERT INTO toto (a,b) VALUES (?,?)" << std::endl;
  // expPos = {31,33};
  // pos = stmt.getPositions("INSERT INTO toto (a,b) VALUES (?,?)");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: INSERT INTO toto (a,b) VALUES (?,?) has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'tata',?)" << std::endl;
  // expPos = {33,42};
  // pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'tata',?)");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'tata',?) has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'ta''ta',?)" << std::endl;
  // expPos = {33,44};
  // pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta''ta',?)");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'ta''ta',?) has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'ta\\'ta',?)" << std::endl;
  // expPos = {33,44};
  // pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta\\'ta',?)");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'ta\\'ta',?) has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'tata''',?)" << std::endl;
  // expPos = {33,44};
  // pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'tata''',?)");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'tata''',?) has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'''tata',?)" << std::endl;
  // expPos = {33,44};
  // pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'''tata',?)");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'''tata',?) has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'ta(ta',?)" << std::endl;
  // expPos = {33,43};
  // pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta(ta',?)");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'ta(ta',?) has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: INSERT INTO toto (a,b,c) VALUES (?,'ta)ta',?)" << std::endl;
  // expPos = {33,43};
  // pos = stmt.getPositions("INSERT INTO toto (a,b,c) VALUES (?,'ta)ta',?)");
  // if(pos.size() != 2) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 2" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: INSERT INTO toto (a,b,c) VALUES (?,'ta)ta',?) has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: SELECT * FROM toto WHERE a IN (?, ?, ?) AND c = '''tata' AND b = ?" << std::endl;
  // expPos = {31,34,37,65};
  // pos = stmt.getPositions("SELECT * FROM toto WHERE a IN (?, ?, ?) AND c = '''tata' AND b = ?");
  // if(pos.size() != 4) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 4" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a IN (?, ?, ?) AND c = '''tata' AND b = ? has been correctly parsed" << std::endl << std::endl;

  // std::cout << "Check query: SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ?" << std::endl;
  // expPos = {31,46,74};
  // pos = stmt.getPositions("SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ?");
  // if(pos.size() != 3) {
  //   std::cerr << "Found " << pos.size() << " positions instead of 3" << std::endl;
  //   return 1;
  // }
  // if(expPos != pos) {
  //   std::cerr << "Positions of placeholders differ" << std::endl;
  //   return 1;
  // }
  // std::cout << "Query: SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ? has been correctly parsed" << std::endl << std::endl;

//   std::cout << "Check PostgreSQL placeholders replacement" << std::endl;
//   std::string query = "SELECT * FROM toto WHERE a IN (?, ''''tata'', ?) AND c = '''tata' AND b = ? AND d = 'titi'";
//   pos = stmt.getPositions(query);
//   std::size_t offset = 0;
//   std::ostringstream oss;
//   std::size_t idx = 0;
//   for(auto iter = pos.cbegin() ; iter != pos.cend() ; ++iter) {
//     oss << query.substr(offset, *iter - offset) << '$' << ++idx;
//     offset = *iter + 1;
//   }
//   if(offset < query.length()) {
//     oss << query.substr(offset);
//   }
//   std::string pgQuery = oss.str();
//   if(pgQuery != "SELECT * FROM toto WHERE a IN ($1, ''''tata'', $2) AND c = '''tata' AND b = $3 AND d = 'titi'") {
//     std::cerr << "Invalid PostgreSQL replacement. Found " << pgQuery << " instead of SELECT * FROM toto WHERE a IN ($1, ''''tata'', $2) AND c = '''tata' AND b = $3 AND d = 'titi'" << std::endl;
//     return 1;
//   }
//   std::cout << "PostgreSQL prepared statement query will be: " << pgQuery << std::endl << std::endl;

//   std::cout << "Exit prepared statement placeholders unit tests" << std::endl;
//   return 0;
// }
