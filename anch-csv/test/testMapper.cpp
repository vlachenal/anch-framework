/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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
#include "csv/csv.hpp"
#include "csv/mapper.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <list>
#include <ranges>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::csv::CsvError;
using anch::csv::ErrorCode;
using anch::csv::ObjectReader;
using anch::csv::ObjectWriter;
using anch::csv::Options;
using anch::csv::Reader;
using anch::csv::Writer;

// Model +
namespace {

  struct Pet {
    uint64_t id = 0;
    std::string name;
    std::optional<std::string> nickname;
    double weight = 0.;
    bool vaccinated = false;
    std::optional<int32_t> age;
#ifdef ANCH_UUID
    std::optional<anch::UUID> chip;
#endif
#ifdef ANCH_DATE
    std::optional<std::chrono::year_month_day> birth;
#endif
    std::string upperName() const {
      std::string res(name);
      for(char& c : res) {
	c = (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
      }
      return res;
    }
  };

}

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id, anch::val::required)
      .registerField("name", &Pet::name, anch::val::required, anch::val::minLength(1))
      .registerField("nickname", &Pet::nickname, anch::val::maxLength(8))
      .registerField("weight", &Pet::weight, anch::val::minimum(0))
      .registerField("vaccinated", &Pet::vaccinated)
      .registerField("age", &Pet::age)
#ifdef ANCH_UUID
      .registerField("chip", &Pet::chip)
#endif
#ifdef ANCH_DATE
      .registerField("birth", &Pet::birth)
#endif
      .registerField("upper", std::function<std::string(const Pet&)>(&Pet::upperName))
      ;
  }
};
static_assert(anch::ObjectMapper<anch::csv::ObjectMapper<Pet>, Pet>);
static_assert(anch::csv::FieldType<std::optional<std::string>> && !anch::csv::FieldType<std::vector<int>>
	      && !anch::csv::FieldType<std::optional<std::vector<int>>> && !anch::csv::FieldType<Pet>);
// Model -

namespace {

  std::vector<Pet>
  readPets(const std::string& content, const Options& options = Options()) {
    std::istringstream input(content);
    Reader reader(input, options);
    ObjectReader<Pet> objects(reader);
    std::vector<Pet> pets;
    Pet pet;
    while(objects.read(pet)) {
      pets.push_back(pet);
      pet = Pet();
    }
    return pets;
  }

  CsvError
  readError(const std::string& content, const Options& options = Options()) {
    try {
      readPets(content, options);
    } catch(const CsvError& error) {
      return error;
    }
    throw std::runtime_error("CsvError expected");
  }

}

void
testReadByName() {
  // Columns order of the header, unknown column ignored, getter only field (upper) not read
  std::vector<Pet> pets = readPets("name,id,color,weight,vaccinated,nickname,age,upper\r\n"
				   "Rex,1,brown,12.5,true,,7,X\r\n"
				   "Médor,2,black,3,false,\"\",,\r\n");
  anch::ut::assertThat(pets.size() == 2, "2 pets expected, found {}", pets.size());
  anch::ut::assertThat(pets[0].id == 1 && pets[0].name == "Rex" && pets[0].weight == 12.5 && pets[0].vaccinated && !pets[0].nickname
		       && pets[0].age == 7, "Unexpected first pet");
  anch::ut::assertThat(pets[1].id == 2 && pets[1].name == "Médor" && !pets[1].vaccinated && pets[1].nickname == std::string()
		       && !pets[1].age, "Empty field is absent, empty quoted field is an empty string");
#ifdef ANCH_UUID
  pets = readPets("id,name,chip\n3,Félix,4a1b5c2d-3e4f-4a5b-8c6d-7e8f9a0b1c2d\n");
  anch::ut::assertThat(pets.size() == 1 && pets[0].chip && pets[0].chip->toString() == "4a1b5c2d-3e4f-4a5b-8c6d-7e8f9a0b1c2d", "UUID");
#endif
#ifdef ANCH_DATE
  pets = readPets("id,name,birth\n4,Garfield,2020-02-29\n");
  anch::ut::assertThat(pets.size() == 1 && pets[0].birth == std::chrono::year_month_day(std::chrono::year(2020), std::chrono::February, std::chrono::day(29)),
		       "Date");
#endif
}

void
testReadByPosition() {
  Options options;
  options.header = false;
  options.sameColumnCount = false;
  std::vector<Pet> pets = readPets("5,Bill,,1.5,true\n6,Boule\n", options);
  anch::ut::assertThat(pets.size() == 2 && pets[0].id == 5 && pets[0].name == "Bill" && pets[0].weight == 1.5 && pets[0].vaccinated
		       && pets[1].id == 6 && pets[1].name == "Boule" && pets[1].weight == 0., "Fields by position");
  CsvError error = readError("7\n", options);
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::MISSING_COLUMN && error.getColumn() == 2, "Required name: {}", error.what());
}

void
testReadErrors() {
  CsvError error = readError("name,weight\nRex,1\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::MISSING_COLUMN && std::string(error.what()) == "line 1: missing required column 'id'",
		       "Missing column: {}", error.what());
  Options strict;
  strict.ignoreUnknownColumns = false;
  error = readError("id,name,color\n1,Rex,brown\n", strict);
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::UNKNOWN_COLUMN && error.getColumn() == 3, "Unknown column: {}", error.what());
  error = readError("id,name,weight\n1,Rex,1\n2,Médor,heavy\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::CONVERSION && error.getLine() == 3 && error.getColumn() == 3
		       && std::string(error.what()) == "line 3, column 3: weight: invalid value 'heavy'", "Conversion: {}", error.what());
  error = readError("id,name\n-1,Rex\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::CONVERSION, "Negative unsigned: {}", error.what());
  error = readError("id,name,vaccinated\n1,Rex,yes\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::CONVERSION, "Invalid boolean: {}", error.what());
  error = readError("id,name\n1,\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::CONSTRAINT_VIOLATION && error.getLine() == 2 && error.getColumn() == 2
		       && std::string(error.what()) == "line 2, column 2: name: Length 0 is lower than minLength 1", "Constraint: {}", error.what());
  error = readError("id,name,nickname\n1,Rex,\"very long nickname\"\n");
  anch::ut::assertThat(error.getErrorCode() == ErrorCode::CONSTRAINT_VIOLATION, "Optional constraint: {}", error.what());
  Options unchecked;
  unchecked.readCheckConstraints = false;
  anch::ut::assertThat(readPets("id,name\n1,\n", unchecked).size() == 1, "Constraints are not checked");
}

void
testWrite() {
  Pet rex;
  rex.id = 1;
  rex.name = "Rex, the dog";
  rex.weight = 12.5;
  rex.vaccinated = true;
  rex.nickname = std::string();
  Pet medor;
  medor.id = 2;
  medor.name = "Médor";
  medor.age = 3;
  std::ostringstream out;
  Options options;
  options.crlf = false;
  options.trueValue = "yes";
  options.falseValue = "no";
  Writer writer(out, options);
  ObjectWriter<Pet> objects(writer);
  objects.write(rex);
  objects.write(medor);
  std::string expected = "id,name,nickname,weight,vaccinated,age";
#ifdef ANCH_UUID
  expected += ",chip";
#endif
#ifdef ANCH_DATE
  expected += ",birth";
#endif
  expected += ",upper\n";
  std::string extra;
#ifdef ANCH_UUID
  extra += ",";
#endif
#ifdef ANCH_DATE
  extra += ",";
#endif
  expected += "1,\"Rex, the dog\",\"\",12.5,yes," + extra + ",\"REX, THE DOG\"\n";
  expected += "2,Médor,,0,no,3" + extra + ",MéDOR\n";
  anch::ut::assertThat(out.str() == expected, "Unexpected CSV:\n{}\nexpected:\n{}", out.str(), expected);
  // Round trip: empty string and absent value are kept
  std::vector<Pet> pets = readPets(out.str(), options);
  anch::ut::assertThat(pets.size() == 2 && pets[0].name == "Rex, the dog" && pets[0].nickname == std::string() && pets[0].vaccinated
		       && !pets[1].nickname && pets[1].age == 3, "Round trip");
}

void
testWriteConstraints() {
  Pet invalid;
  invalid.id = 1;
  std::ostringstream out;
  Writer writer(out);
  ObjectWriter<Pet> unchecked(writer, false);
  unchecked.write(invalid); // constraints are not checked by default
  Options options;
  options.writeCheckConstraints = true;
  std::ostringstream checkedOut;
  Writer checkedWriter(checkedOut, options);
  ObjectWriter<Pet> checked(checkedWriter);
  bool raised = false;
  try {
    checked.write(invalid);
  } catch(const CsvError& error) {
    raised = error.getErrorCode() == ErrorCode::CONSTRAINT_VIOLATION && error.getLine() == 2 && error.getColumn() == 2;
  }
  anch::ut::assertThat(raised, "Constraint violation on line 2 (after the header) expected");
}

static_assert(std::ranges::input_range<anch::csv::Records<Pet>&>);

void
testRecords() {
  std::istringstream input("id,name,age\n1,Rex,3\n2,Médor,\n3,Félix,5\n");
  std::vector<std::string> names;
  int ages = 0;
  for(const Pet& pet : anch::csv::records<Pet>(input)) {
    names.push_back(pet.name);
    ages += pet.age.value_or(0);
  }
  anch::ut::assertThat(names == std::vector<std::string>{"Rex", "Médor", "Félix"} && ages == 8, "Unexpected records");
  // Range algorithms
  std::istringstream second("id,name\n1,Rex\n2,Médor\n");
  auto records = anch::csv::records<Pet>(second);
  auto ids = records | std::views::transform([](const Pet& pet) { return pet.id; });
  uint64_t sum = 0;
  for(uint64_t id : ids) {
    sum += id;
  }
  anch::ut::assertThat(sum == 3, "Range adaptors");
  // Errors are raised while iterating, with the line of the record
  std::istringstream invalid("id,name\n1,Rex\nx,Médor\n");
  std::size_t read = 0;
  bool raised = false;
  try {
    for([[maybe_unused]] const Pet& pet : anch::csv::records<Pet>(invalid)) {
      ++read;
    }
  } catch(const CsvError& error) {
    raised = error.getErrorCode() == ErrorCode::CONVERSION && error.getLine() == 3;
  }
  anch::ut::assertThat(raised && read == 1, "Conversion error on line 3 after 1 record");
  // Header errors are raised by records()
  std::istringstream noId("name\nRex\n");
  raised = false;
  try {
    [[maybe_unused]] auto missing = anch::csv::records<Pet>(noId);
  } catch(const CsvError& error) {
    raised = error.getErrorCode() == ErrorCode::MISSING_COLUMN;
  }
  anch::ut::assertThat(raised, "Missing column raised by records()");
  std::istringstream empty("");
  for([[maybe_unused]] const Pet& pet : anch::csv::records<Pet>(empty)) {
    anch::ut::assertThat(false, "No record expected");
  }
}

void
testVectors() {
  Pet rex;
  rex.id = 1;
  rex.name = "Rex";
  Pet medor;
  medor.id = 2;
  medor.name = "Médor";
  medor.nickname = "Mémé";
  Options options;
  options.separator = ';';
  std::ostringstream out;
  anch::csv::serialize(std::list<Pet>{rex, medor}, out, options);
  std::vector<Pet> pets{Pet()};
  std::istringstream input(out.str());
  anch::csv::deserialize(pets, input, options);
  anch::ut::assertThat(pets.size() == 2 && pets[0].name == "Rex" && !pets[0].nickname && pets[1].nickname == "Mémé",
		       "Round trip of a list (values are replaced)");
  // Header is written without object
  std::ostringstream header;
  anch::csv::serialize(std::vector<Pet>(), header);
  anch::ut::assertThat(header.str().starts_with("id,name,nickname,") && header.str().ends_with(",upper\r\n"),
		       "Header expected: '{}'", header.str());
  Options noHeader;
  noHeader.header = false;
  std::ostringstream none;
  anch::csv::serialize(std::vector<Pet>(), none, noHeader);
  anch::ut::assertThat(none.str().empty(), "Nothing written without header and object");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH CSV objects mapping unit tests")
    .description("Test AnCH CSV objects reader and writer")
    .add("read-by-name", testReadByName)
    .add("read-by-position", testReadByPosition)
    .add("read-errors", testReadErrors)
    .add("write", testWrite)
    .add("write-constraints", testWriteConstraints)
    .add("records", testRecords)
    .add("vectors", testVectors)
    ;
}
