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
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "petstore.hpp"

#include "date/rfc3339.hpp"

#include "json/json.hpp"
#include "json/mappingError.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using petstore::model::Category;
using petstore::model::Dog;
using petstore::model::Empty;
using petstore::model::House;
using petstore::model::Pet;
using petstore::model::PetSize;
using petstore::model::Pets;
using petstore::model::Status;


/*! Complete pet (every generated property) */
const std::string FULL_PET = R"({
  "id": 42, "name": "Rex", "uuid": "deb94ebc-be28-4899-981a-29199b7a487d", "birth": "2020-01-02T03:04:05Z",
  "birthday": "2020-01-02", "visits": [ "2021-03-04T05:06:07.890+01:00", "2022-01-01T00:00:00Z" ],
  "age": 6, "weight": 12.5, "price": 99.99, "vaccinated": true, "status": "in-stock", "previousStatus": "2nd-hand",
  "size": "large", "level": 2,
  "category": { "id": 1, "name": "dogs", "parent": { "name": "animals" }, "children": [ { "name": "puppies" }, { "name": "seniors" } ] },
  "tags": [ { "id": 1, "name": "cute" }, { "name": "loud" } ],
  "nicknames": [ "R", "Rexy" ],
  "photos": [ { "url": "http://localhost/rex.png", "width": 640 } ],
  "attributes": { "color": "brown", "eyes": "blue" },
  "scores": { "agility": 8, "obedience": -1 },
  "owner": { "firstName": "Vincent", "first-name": "Vince" },
  "friends": [ { "id": 43, "name": "Médor", "status": "sold" } ],
  "comment": null, "class": "mammal", "$id": "pet-42", "2fa": false, "Category": "big"
})";

void
testPet() {
  anch::json::MappingOptions options;
  options.deserialize_max_discard_char = -1;
  std::istringstream input(FULL_PET);
  Pet pet = anch::json::deserialize<Pet>(input, options);
  anch::ut::assertThat(pet.id == 42 && pet.name == "Rex", "id / name: {} / {}", pet.id, pet.name);
  anch::ut::assertThat(pet.uuid.has_value() && pet.uuid->toString() == "deb94ebc-be28-4899-981a-29199b7a487d", "Invalid UUID");
  anch::ut::assertThat(pet.birth.has_value() && anch::date::rfc3339::formatDateTime(*pet.birth) == "2020-01-02T03:04:05Z", "Invalid birth");
  anch::ut::assertThat(pet.birthday == std::chrono::year_month_day(std::chrono::year(2020), std::chrono::January, std::chrono::day(2)), "Invalid birthday");
  anch::ut::assertThat(pet.visits.size() == 2 && anch::date::rfc3339::formatDateTime(pet.visits[0]) == "2021-03-04T04:06:07.890Z", "Invalid visits");
  anch::ut::assertThat(pet.age == 6 && pet.weight == 12.5f && pet.price == 99.99 && pet.vaccinated == true, "Invalid primitives");
  anch::ut::assertThat(pet.status == Status::IN_STOCK, "status should be IN_STOCK");
  anch::ut::assertThat(pet.previousStatus == Status::_2ND_HAND, "previousStatus should be _2ND_HAND");
  anch::ut::assertThat(pet.size == PetSize::LARGE, "size should be LARGE");
  anch::ut::assertThat(pet.level == 2, "level: {}", pet.level.value_or(-1));
  // Recursive model +
  anch::ut::assertThat(pet.category.has_value() && pet.category->name == "dogs" && pet.category->id == 1, "Invalid category");
  anch::ut::assertThat(pet.category->parent != nullptr && pet.category->parent->name == "animals" && pet.category->parent->parent == nullptr, "Invalid parent category");
  anch::ut::assertThat(pet.category->children.size() == 2 && pet.category->children[1].name == "seniors", "Invalid children categories");
  // Recursive model -
  anch::ut::assertThat(pet.tags.size() == 2 && pet.tags[0].name == "cute" && !pet.tags[1].id.has_value(), "Invalid tags");
  anch::ut::assertThat((pet.nicknames == std::vector<std::string>{"R", "Rexy"}), "Invalid nicknames");
  anch::ut::assertThat(pet.photos.size() == 1 && pet.photos[0].url == "http://localhost/rex.png" && pet.photos[0].width == 640, "Invalid photos");
  anch::ut::assertThat((pet.attributes == std::map<std::string,std::string>{{"color", "brown"}, {"eyes", "blue"}}), "Invalid attributes");
  anch::ut::assertThat(pet.scores.size() == 2 && pet.scores["obedience"] == -1, "Invalid scores");
  anch::ut::assertThat(pet.owner.has_value() && pet.owner->firstName == "Vincent" && pet.owner->first_name == "Vince", "Invalid owner");
  anch::ut::assertThat(pet.friends.size() == 1 && pet.friends[0].name == "Médor" && pet.friends[0].status == Status::SOLD, "Invalid friends");
  anch::ut::assertThat(!pet.comment.has_value(), "comment is null");
  anch::ut::assertThat(pet.class_ == "mammal" && pet._id == "pet-42" && pet._2fa == false && pet.Category_ == "big", "Renamed members");
}

void
testOptional() {
  std::istringstream input(R"({"id":1,"name":"a","status":"sold"})");
  Pet pet = anch::json::deserialize<Pet>(input);
  anch::ut::assertThat(pet.id == 1 && pet.name == "a" && pet.status == Status::SOLD, "Invalid required fields");
  anch::ut::assertThat(!pet.uuid && !pet.age && !pet.weight && !pet.size && !pet.category && !pet.owner, "Optional fields should be empty");
  anch::ut::assertThat(pet.tags.empty() && pet.attributes.empty() && pet.friends.empty(), "Containers should be empty");
}

void
testNullableEnum() {
  std::istringstream input(R"({"id":1,"name":"a","status":"sold","size":null})");
  Pet pet = anch::json::deserialize<Pet>(input);
  anch::ut::assertThat(!pet.size.has_value(), "size should be empty");
}

void
testEnumValues() {
  std::map<std::string,Status> expected = {
    {"available", Status::AVAILABLE}, {"pending", Status::PENDING}, {"sold", Status::SOLD},
    {"in-stock", Status::IN_STOCK}, {"outOfStock", Status::OUT_OF_STOCK}, {"2nd-hand", Status::_2ND_HAND}
  };
  for(const auto& [value, status] : expected) {
    std::istringstream input(R"({"id":1,"name":"a","status":")" + value + "\"}");
    Pet pet = anch::json::deserialize<Pet>(input);
    anch::ut::assertThat(pet.status == status, "Invalid status for '{}'", value);
    std::string json = anch::json::serialize(pet);
    anch::ut::assertThat(json.find("\"status\":\"" + value + "\"") != std::string::npos, "Serialized status for '{}': {}", value, json);
  }
}

void
testEnumInvalid() {
  for(const char* json : {R"({"id":1,"name":"a","status":"unknown"})", R"({"id":1,"name":"a","status":"SOLD"})",
			  R"({"id":1,"name":"a","status":"sold","size":"huge"})"}) {
    try {
      std::istringstream input(json);
      [[maybe_unused]] Pet pet = anch::json::deserialize<Pet>(input);
      anch::ut::fail(std::string("Invalid enumeration value should be refused: ") + json);
    } catch(const anch::json::MappingError& error) {
      std::cout << error.what() << std::endl;
      anch::ut::assertThat(error.getErrorCode() == anch::json::ErrorCode::INVALID_FORMAT, "INVALID_FORMAT expected");
    }
  }
}

void
testUnknownField() {
  std::string json = R"({"id":1,"name":"a","status":"sold","extra":{"a":1,"b":[1,2]},"pet":{"breed":"x"}})";
  try {
    std::istringstream input(json);
    [[maybe_unused]] Pet pet = anch::json::deserialize<Pet>(input);
    anch::ut::fail("Not generated properties are unknown fields: refused by default");
  } catch(const anch::json::MappingError& error) {
    std::cout << error.what() << std::endl;
  }
  anch::json::MappingOptions options;
  options.deserialize_ignore_unknown_field = true;
  std::istringstream input(json);
  Pet pet = anch::json::deserialize<Pet>(input, options);
  anch::ut::assertThat(pet.name == "a", "name: {}", pet.name);
}

void
testAllOf() {
  std::istringstream input(R"({"id":7,"name":"Rantanplan","status":"available","size":"small","owner":{"firstName":"Lucky"},"breed":"unknown","goodBoy":true})");
  Dog dog = anch::json::deserialize<Dog>(input);
  anch::ut::assertThat(dog.id == 7 && dog.name == "Rantanplan" && dog.status == Status::AVAILABLE, "Inherited required fields");
  anch::ut::assertThat(dog.size == PetSize::SMALL && dog.owner.has_value() && dog.owner->firstName == "Lucky", "Inherited inline types");
  anch::ut::assertThat(dog.breed == "unknown" && dog.goodBoy == true, "Dog fields");
}

void
testMutualRecursion() {
  std::istringstream input(R"({"address":"Paris","owner":{"name":"Vincent","house":{"address":"Lyon"}},"pets":{"rex":{"id":1,"name":"Rex","status":"sold"}}})");
  House house = anch::json::deserialize<House>(input);
  anch::ut::assertThat(house.address == "Paris" && house.owner != nullptr && house.owner->name == "Vincent", "Invalid owner");
  anch::ut::assertThat(house.owner->house != nullptr && house.owner->house->address == "Lyon" && house.owner->house->owner == nullptr, "Invalid owner house");
  anch::ut::assertThat(house.pets.size() == 1 && house.pets["rex"].name == "Rex", "Invalid pets");
}

void
testAliases() {
  std::istringstream input(R"([{"id":1,"name":"a","status":"sold"},{"id":2,"name":"b","status":"pending"}])");
  Pets pets;
  anch::json::deserialize(pets, input);
  anch::ut::assertThat(pets.size() == 2 && pets[1].name == "b" && pets[1].status == Status::PENDING, "Invalid pets");
  std::istringstream labels(R"({"a":"1","b":"2"})");
  petstore::model::Labels values;
  anch::json::deserialize(values, labels);
  anch::ut::assertThat(values.size() == 2 && values["b"] == "2", "Invalid labels");
  [[maybe_unused]] petstore::model::Identifier id;
}

void
testEmpty() {
  std::istringstream input("{}");
  [[maybe_unused]] Empty empty = anch::json::deserialize<Empty>(input);
}

void
testRoundTrip() {
  anch::json::MappingOptions options;
  options.deserialize_max_discard_char = -1;
  std::istringstream input(FULL_PET);
  Pet pet = anch::json::deserialize<Pet>(input, options);
  std::istringstream serialized(anch::json::serialize(pet));
  Pet copy = anch::json::deserialize<Pet>(serialized);
  anch::ut::assertThat(copy.name == pet.name && copy.status == pet.status && copy.size == pet.size && copy.uuid == pet.uuid, "Invalid copy");
  anch::ut::assertThat(copy.birth == pet.birth && copy.birthday == pet.birthday && copy.visits == pet.visits, "Invalid copy dates");
  anch::ut::assertThat(copy.category->parent->name == "animals" && copy.category->children.size() == 2, "Invalid copy category");
  anch::ut::assertThat(copy.attributes == pet.attributes && copy.owner->first_name == "Vince", "Invalid copy attributes / owner");
}

void
testDateInvalid() {
  for(const char* json : {R"({"id":1,"name":"a","status":"sold","birth":"2020-01-02"})",
                          R"({"id":1,"name":"a","status":"sold","birthday":"2020-02-30"})",
                          R"({"id":1,"name":"a","status":"sold","visits":["2020-01-02T03:04:05"]})"}) {
    std::istringstream input(json);
    try {
      [[maybe_unused]] Pet pet = anch::json::deserialize<Pet>(input);
      anch::ut::fail(std::string("Invalid date should be refused: ") + json);
    } catch(const anch::json::MappingError& e) {
      std::cout << e.what() << std::endl;
      anch::ut::assertThat(e.getErrorCode() == anch::json::ErrorCode::INVALID_FORMAT, "INVALID_FORMAT expected");
    }
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH OAS generated models unit tests")
    .description("Deserialize JSON with models generated from petstore.json")
    .add("models-pet", testPet)
    .add("models-optional", testOptional)
    .add("models-nullable-enum", testNullableEnum)
    .add("models-enum-values", testEnumValues)
    .add("models-enum-invalid", testEnumInvalid)
    .add("models-unknown-field", testUnknownField)
    .add("models-allof", testAllOf)
    .add("models-mutual-recursion", testMutualRecursion)
    .add("models-aliases", testAliases)
    .add("models-empty", testEmpty)
    .add("models-round-trip", testRoundTrip)
    .add("models-date-invalid", testDateInvalid)
    ;
}
