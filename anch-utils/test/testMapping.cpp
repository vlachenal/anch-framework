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
#include "mapping.hpp"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

namespace {

  // Field registered by a mapper
  struct Field {
    std::string name;
    std::string kind;
    std::size_t constraints;
  };

  // Mapper which records registered fields
  template<typename T>
  class Recorder {
  public:
    std::vector<Field> fields;

    template<typename P, typename... C>
    Recorder& registerField(const std::string& name, P T::*, C&&...) {
      fields.push_back(Field{name, "member", sizeof...(C)});
      return *this;
    }

    template<typename P, typename... C>
    Recorder& registerField(const std::string& name, std::function<P(const T&)>, C&&...) {
      fields.push_back(Field{name, "getter", sizeof...(C)});
      return *this;
    }

    template<typename P, typename... C>
    Recorder& registerField(const std::string& name, std::function<void(T&, const P&)>, C&&...) {
      fields.push_back(Field{name, "setter", sizeof...(C)});
      return *this;
    }

    template<typename P, typename... C>
    Recorder& registerField(const std::string& name, std::function<P(const T&)>, std::function<void(T&, const P&)>, C&&...) {
      fields.push_back(Field{name, "accessors", sizeof...(C)});
      return *this;
    }
  };

  // Mapper without accessors registration
  template<typename T>
  struct MembersOnly {
    template<typename P>
    MembersOnly& registerField(const std::string&, P T::*) {
      return *this;
    }
  };

  // Mapper which does not return itself
  template<typename T>
  struct NotFluent {
    template<typename P>
    void registerField(const std::string&, P T::*) {}
    template<typename P>
    void registerField(const std::string&, std::function<P(const T&)>) {}
    template<typename P>
    void registerField(const std::string&, std::function<void(T&, const P&)>) {}
    template<typename P>
    void registerField(const std::string&, std::function<P(const T&)>, std::function<void(T&, const P&)>) {}
  };

  // Constraints are opaque for descriptions: format mappers receive them
  struct Required {};
  struct MinLength {
    std::size_t value;
  };

  struct Pet {
    uint64_t id;
    std::string name;
    std::optional<std::string> owner;
    int age;
    int getAge() const { return age; }
    void setAge(const int& value) { age = value; }
  };

  struct Undescribed {
    int value;
  };

}

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("id", &Pet::id, Required())
      .registerField("name", &Pet::name, Required(), MinLength{1})
      .registerField("owner", &Pet::owner)
      .registerField("age", std::function<int(const Pet&)>(&Pet::getAge), std::function<void(Pet&, const int&)>(&Pet::setAge))
      .registerField("nextAge", std::function<int(const Pet&)>([](const Pet& pet) { return pet.age + 1; }))
      .registerField("birthYear", std::function<void(Pet&, const int&)>([](Pet& pet, const int& year) { pet.age = 2026 - year; }))
      ;
  }
};

// std and third party types can be described
template<>
struct anch::Mapping<std::pair<std::string, int>> {
  static void
  registerObject(anch::ObjectMapper<std::pair<std::string, int>> auto& mapper) {
    mapper.registerField("key", &std::pair<std::string, int>::first).registerField("value", &std::pair<std::string, int>::second);
  }
};

static_assert(anch::Described<Pet> && anch::Described<std::pair<std::string, int>>);
static_assert(!anch::Described<Undescribed> && !anch::Described<int> && !anch::Described<std::string>);
static_assert(anch::ObjectMapper<Recorder<Pet>, Pet>);
static_assert(!anch::ObjectMapper<MembersOnly<Pet>, Pet>, "Accessors registration is required");
static_assert(!anch::ObjectMapper<NotFluent<Pet>, Pet>, "Registration has to return the mapper");
static_assert(!anch::ObjectMapper<Recorder<Pet>, int>, "Described types are classes");

void
testDescribe() {
  Recorder<Pet> recorder;
  anch::describe<Pet>(recorder);
  const std::vector<std::string> expected{"id", "name", "owner", "age", "nextAge", "birthYear"};
  anch::ut::assertThat(recorder.fields.size() == expected.size(), "6 fields expected, found {}", recorder.fields.size());
  for(std::size_t i = 0; i < expected.size(); ++i) {
    anch::ut::assertThat(recorder.fields[i].name == expected[i], "Field {} is '{}' instead of '{}'", i, recorder.fields[i].name, expected[i]);
  }
  anch::ut::assertThat(recorder.fields[0].kind == "member" && recorder.fields[0].constraints == 1
		       && recorder.fields[1].constraints == 2 && recorder.fields[2].constraints == 0, "Constraints are given to the mapper");
  anch::ut::assertThat(recorder.fields[3].kind == "accessors" && recorder.fields[4].kind == "getter" && recorder.fields[5].kind == "setter",
		       "Accessors registrations");
}

void
testThirdParty() {
  Recorder<std::pair<std::string, int>> recorder;
  anch::describe<std::pair<std::string, int>>(recorder);
  anch::ut::assertThat(recorder.fields.size() == 2 && recorder.fields[0].name == "key" && recorder.fields[1].name == "value",
		       "std::pair description");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH objects description unit tests")
    .description("Test anch::Mapping, anch::ObjectMapper and anch::Described")
    .add("describe", testDescribe)
    .add("third-party", testThirdParty)
    ;
}
