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
#include "codec/all.hpp"
#include "codec/format.hpp"

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::codec::Available;
using anch::codec::Csv;
using anch::codec::MediaType;
using anch::codec::Side;

namespace {

  struct Pet {
    uint64_t id = 0;
    std::string name;
  };

}

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id, anch::val::required)
      .registerField("name", &Pet::name, anch::val::minLength(1));
  }
};

// Csv handles lists of described objects, Json any value
static_assert(anch::codec::Serializer<Csv, std::vector<Pet>> && anch::codec::Deserializer<Csv, std::vector<Pet>>);
static_assert(!anch::codec::Serializer<Csv, Pet> && !anch::codec::Serializer<Csv, std::vector<int>> && !anch::codec::Serializer<Csv, int>);

void
testNegotiation() {
  const std::vector<Pet> pets{{1, "Rex"}, {2, "Médor"}};
  std::ostringstream csv;
  auto type = Available::serialize(pets, csv, MediaType::parseAccept("text/csv, application/json;q=0.5"), Side::SERVER);
  anch::ut::assertThat(type && type->essence() == "text/csv" && csv.str() == "id,name\r\n1,Rex\r\n2,Médor\r\n", "CSV expected: '{}'", csv.str());
  std::ostringstream json;
  type = Available::serialize(pets, json, {}, Side::SERVER);
  anch::ut::assertThat(type && type->essence() == "application/json", "Json is the first codec");
  // A single object is not CSV
  std::ostringstream single;
  type = Available::serialize(pets[0], single, MediaType::parseAccept("text/csv"), Side::SERVER);
  anch::ut::assertThat(!type, "No CSV codec for an object");
  // Reading
  std::vector<Pet> read;
  std::istringstream input("name,id\nFélix,3\n");
  anch::ut::assertThat(Available::deserialize(read, input, *MediaType::parse("text/csv; charset=utf-8"), Side::SERVER)
		       && read.size() == 1 && read[0].id == 3 && read[0].name == "Félix", "CSV reading");
}

void
testSides() {
  // SERVER checks read objects, CLIENT written ones
  auto readFails = [](Side side) {
    std::vector<Pet> pets;
    std::istringstream input("id,name\n1,\n");
    try {
      Csv::deserialize(pets, input, side);
    } catch(const anch::csv::CsvError&) {
      return true;
    }
    return false;
  };
  anch::ut::assertThat(readFails(Side::SERVER) && !readFails(Side::CLIENT), "Constraints checked on SERVER reading");
  std::ostringstream out;
  bool raised = false;
  try {
    Csv::serialize(std::vector<Pet>{{1, ""}}, out, Side::CLIENT);
  } catch(const anch::csv::CsvError&) {
    raised = true;
  }
  anch::ut::assertThat(raised, "Constraints checked on CLIENT writing");
  // CLIENT ignores unknown columns, SERVER bounds the input
  anch::ut::assertThat(Csv::options(Side::CLIENT).ignoreUnknownColumns && Csv::options(Side::SERVER).maxSize == 2 * 1024 * 1024,
		       "Side options");
  const std::size_t previous = Csv::options(Side::SERVER).maxSize;
  Csv::options(Side::SERVER).maxSize = 16;
  std::vector<Pet> pets;
  std::istringstream large("id,name\n1,Rex\n2,Médor\n3,Félix\n");
  raised = false;
  try {
    Csv::deserialize(pets, large, Side::SERVER);
  } catch(const anch::codec::ContentTooLarge&) {
    raised = true;
  }
  Csv::options(Side::SERVER).maxSize = previous;
  anch::ut::assertThat(raised, "TOO_LARGE has to be translated into ContentTooLarge");
}

void
testFormats() {
  const std::vector<anch::codec::Format> formats = anch::codec::formats();
  anch::ut::assertThat(std::any_of(formats.begin(), formats.end(), [](const anch::codec::Format& format) {
	return format.name == "csv" && format.mediaTypes == std::vector<std::string>{"text/csv"};
      }), "csv format has to be registered");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH codec CSV unit tests")
    .description("Test AnCH codec CSV plugin")
    .add("negotiation", testNegotiation)
    .add("sides", testSides)
    .add("formats", testFormats)
    ;
}
