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
#include <type_traits>
#include <vector>

#include "json/json.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::codec::Json;
using anch::codec::MediaType;
using anch::codec::Side;

// Model +
struct Owner {
  std::string name;
  uint64_t age;
};

template<>
struct anch::Mapping<Owner> {
  static void
  registerObject(anch::ObjectMapper<Owner> auto& mapper) {
    mapper
      .registerField("name", &Owner::name, anch::val::required, anch::val::minLength(1))
      .registerField("age", &Owner::age)
      ;
  }
};
// Model -

static_assert(anch::codec::Serializer<Json, Owner> && anch::codec::Deserializer<Json, Owner>);
// Json is the first available codec, Text the last one
template<typename L>
struct Ends;

template<typename F, typename... Cs>
struct Ends<anch::codec::Codecs<F, Cs...>> {
  using First = F;
  using Last = typename decltype((std::type_identity<F>{}, ..., std::type_identity<Cs>{}))::type;
};
static_assert(std::is_same_v<Ends<anch::codec::Available>::First, Json>
	      && std::is_same_v<Ends<anch::codec::Available>::Last, anch::codec::Text>);
#ifndef ANCH_CODEC_JSON_ENABLED
#error "ANCH_CODEC_JSON_ENABLED has to be defined by codec/all.hpp"
#endif

void
testSerialize() {
  std::ostringstream out;
  anch::codec::serialize<Json>(Owner{"Ann", 42}, out, anch::codec::contentTypeOf<Json>(Owner{}), Side::SERVER);
  anch::ut::assertThat(out.str() == R"({"name":"Ann","age":42})", "Unexpected JSON '{}'", out.str());
  std::ostringstream text;
  Json::serialize(std::string("a\"b"), text);
  anch::ut::assertThat(text.str() == R"("a\"b")", "std::string has to be serialized as a JSON string, found '{}'", text.str());
  std::istringstream input(R"({"name":"Bob","age":7})");
  Owner owner;
  anch::codec::deserialize<Json>(owner, input, MediaType("application", "json"), Side::CLIENT);
  anch::ut::assertThat(owner.name == "Bob" && owner.age == 7, "Unexpected owner {} {}", owner.name, owner.age);
}

void
testConstraints() {
  // SERVER checks what it reads, CLIENT what it writes
  auto readFails = [](Side side) {
    std::istringstream input(R"({"name":"","age":1})");
    Owner owner;
    try {
      Json::deserialize(owner, input, side);
    } catch(const anch::json::MappingError&) {
      return true;
    }
    return false;
  };
  auto writeFails = [](Side side) {
    std::ostringstream out;
    try {
      Json::serialize(Owner{"", 1}, out, side);
    } catch(const anch::json::MappingError&) {
      return true;
    }
    return false;
  };
  anch::ut::assertThat(readFails(Side::SERVER) && !readFails(Side::CLIENT), "Constraints have to be checked on SERVER reading only");
  anch::ut::assertThat(writeFails(Side::CLIENT) && !writeFails(Side::SERVER), "Constraints have to be checked on CLIENT writing only");
  // CLIENT ignores unknown fields
  std::istringstream newer(R"({"name":"Ann","age":1,"color":"red"})");
  Owner owner;
  Json::deserialize(owner, newer, Side::CLIENT);
  anch::ut::assertThat(owner.name == "Ann", "Unknown fields have to be ignored by CLIENT");
}

void
testTooLarge() {
  const std::size_t previous = Json::options(Side::SERVER).max_payload;
  Json::options(Side::SERVER).max_payload = 16;
  std::istringstream input(R"({"name":"a long name","age":1})");
  Owner owner;
  bool raised = false;
  try {
    Json::deserialize(owner, input, Side::SERVER);
  } catch(const anch::codec::ContentTooLarge&) {
    raised = true;
  }
  Json::options(Side::SERVER).max_payload = previous;
  anch::ut::assertThat(raised, "ContentTooLarge expected");
}

void
testAvailable() {
  // Owner is only serialized by Json, int by Json and Text
  std::ostringstream out;
  auto selected = anch::codec::Available::serialize(Owner{"Ann", 1}, out, MediaType::parseAccept("text/plain, */*;q=0.1"), Side::SERVER);
  anch::ut::assertThat(selected && selected->essence() == "application/json", "Json expected");
  std::ostringstream number;
  selected = anch::codec::Available::serialize(3, number, MediaType::parseAccept("text/plain, */*;q=0.1"), Side::SERVER);
  anch::ut::assertThat(selected && selected->essence() == "text/plain" && number.str() == "3", "Text expected");
  std::istringstream input(R"({"name":"Ann","age":2})");
  Owner owner;
  anch::ut::assertThat(anch::codec::Available::deserialize(owner, input, *MediaType::parse("application/json; charset=utf-8"), Side::SERVER)
		       && owner.age == 2, "Json expected for reading");
}

void
testFormats() {
  const std::vector<anch::codec::Format> formats = anch::codec::formats();
  std::vector<std::string> names;
  for(const anch::codec::Format& format : formats) {
    names.push_back(format.name);
  }
  anch::ut::assertThat(std::find(names.begin(), names.end(), "json") != names.end()
		       && std::find(names.begin(), names.end(), "text") != names.end(), "json and text formats expected");
  auto json = std::find_if(formats.begin(), formats.end(), [](const anch::codec::Format& format) { return format.name == "json"; });
  anch::ut::assertThat(json != formats.end() && json->mediaTypes == std::vector<std::string>{"application/json"},
		       "Unexpected json media types");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH codec JSON unit tests")
    .description("Test AnCH codec JSON plugin")
    .add("serialize", testSerialize)
    .add("constraints", testConstraints)
    .add("too-large", testTooLarge)
    .add("available", testAvailable)
    .add("formats", testFormats)
    ;
}
