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
#include <tuple>
#include <limits>
#include <initializer_list>
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

using petstore::model::Adoption;
using petstore::model::Category;
using petstore::model::Constrained;
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

template<typename T>
void
checkMappingError(const std::string& json, anch::json::ErrorCode code, const std::string& path, const std::string& message) {
  std::cout << "Deserialize " << json << std::endl;
  std::istringstream input(json);
  try {
    T res;
    anch::json::deserialize(res, input);
    anch::ut::fail("This test should have failed");
  } catch(const anch::json::MappingError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assertThat(e.getErrorCode() == code, "Expected error code {} ; found {}", static_cast<int>(code), static_cast<int>(e.getErrorCode()));
    anch::ut::assertThat(e.getPath() == path, "Expected path '{}' ; found '{}'", path, e.getPath());
    anch::ut::assertThat(std::string(e.what()).find(message) != std::string::npos, "'{}' not found in '{}'", message, e.what());
  }
}

void
testRequiredMissing() {
  checkMappingError<Pet>(R"({"id":1,"status":"sold"})", anch::json::ErrorCode::MISSING_FIELD, "", "Missing required field 'name'");
  checkMappingError<Pet>("{}", anch::json::ErrorCode::MISSING_FIELD, "", "Missing required fields 'id', 'name', 'status'");
  // allOf: inherited and own required properties
  checkMappingError<Dog>(R"({"id":1,"name":"a","status":"sold"})", anch::json::ErrorCode::MISSING_FIELD, "", "Missing required field 'breed'");
  checkMappingError<Dog>(R"({"name":"a","status":"sold","breed":"x"})", anch::json::ErrorCode::MISSING_FIELD, "", "Missing required field 'id'");
  // inline item and nested model
  checkMappingError<Pet>(R"({"id":1,"name":"a","status":"sold","photos":[{"url":"u"},{"width":1}]})", anch::json::ErrorCode::MISSING_FIELD,
			 "/photos/1", "Missing required field 'url'");
  checkMappingError<Pet>(R"({"id":1,"name":"a","status":"sold","category":{"name":"c","parent":{"id":2}}})", anch::json::ErrorCode::MISSING_FIELD,
			 "/category/parent", "Missing required field 'name'");
  checkMappingError<Pets>(R"([{"id":1,"name":"a","status":"sold"},{"id":2,"name":"b"}])", anch::json::ErrorCode::MISSING_FIELD,
			 "/1", "Missing required field 'status'");
}

void
testRequiredNull() {
  checkMappingError<Pet>(R"({"id":1,"name":null,"status":"sold"})", anch::json::ErrorCode::CONSTRAINT_VIOLATION, "/name", "Required field can not be null");
  checkMappingError<Pet>(R"({"id":1,"name":"a","status":null})", anch::json::ErrorCode::CONSTRAINT_VIOLATION, "/status", "Required field can not be null");
  // Property which is not required can be null
  std::istringstream input(R"({"id":1,"name":"a","status":"sold","age":null,"photos":null})");
  Pet pet = anch::json::deserialize<Pet>(input);
  anch::ut::assertThat(!pet.age.has_value() && pet.photos.empty(), "age and photos should be empty");
}

void
testRequiredNullable() {
  {
    std::istringstream input(R"({"petId":1,"comment":null,"visitors":[]})");
    Adoption adoption = anch::json::deserialize<Adoption>(input);
    anch::ut::assertThat(adoption.petId == 1 && !adoption.comment.has_value() && adoption.visitors.empty(), "Invalid adoption");
  }
  {
    std::istringstream input(R"({"petId":1,"comment":"ok","visitors":["Vincent"]})");
    Adoption adoption = anch::json::deserialize<Adoption>(input);
    anch::ut::assertThat(adoption.comment == "ok" && adoption.visitors.size() == 1, "Invalid adoption");
  }
  // nullable required property has to be present
  checkMappingError<Adoption>(R"({"petId":1,"visitors":[]})", anch::json::ErrorCode::MISSING_FIELD, "", "Missing required field 'comment'");
  // required array: absent or null is refused
  checkMappingError<Adoption>(R"({"petId":1,"comment":null})", anch::json::ErrorCode::MISSING_FIELD, "", "Missing required field 'visitors'");
  checkMappingError<Adoption>(R"({"petId":1,"comment":null,"visitors":null})", anch::json::ErrorCode::CONSTRAINT_VIOLATION, "/visitors",
			      "Required field can not be null");
}

void
testConstraints() {
  std::istringstream input(R"({"name":"AB","count":9,"ratio":2.5,"big":-9223372036854775808,"step":-15,"codes":["AB","CD"],)"
			   R"("tags":[{"id":1},{"id":2}],"weights":{"a":0,"b":1.5},"grade":1.5,"flag":true,"email":"a@b.c","ref":"123-45"})");
  Constrained value = anch::json::deserialize<Constrained>(input);
  anch::ut::assertThat(value.name == "AB" && value.count == 9 && value.ratio == 2.5f && value.big == std::numeric_limits<int64_t>::min(),
		       "Invalid values");
  anch::ut::assertThat(value.codes.size() == 2 && value.tags.size() == 2 && value.weights.size() == 2 && value.grade == 1.5 && value.flag == true,
		       "Invalid values");
}

void
testConstraintsInvalid() {
  using anch::json::ErrorCode;
  for(auto [fields, path, message] : std::initializer_list<std::tuple<const char*, const char*, const char*>>{
      {R"("name":"A")", "/name", "Length 1 is lower than minLength 2"},
      {R"("name":"ABCDEFGHI")", "/name", "Length 9 is greater than maxLength 8"},
      {R"("name":"AB","count":0)", "/count", "Value 0 is lower than minimum 1"},
      {R"("name":"AB","count":10)", "/count", "Value 10 is greater than or equal to exclusiveMaximum 10"},
      {R"("name":"AB","ratio":0)", "/ratio", "Value 0 is lower than or equal to exclusiveMinimum 0"},
      {R"("name":"AB","ratio":0.7)", "/ratio", "is not a multiple of 0.5"},
      {R"("name":"AB","step":7)", "/step", "Value 7 is not a multiple of 5"},
      {R"("name":"AB","codes":[])", "/codes", "Item count 0 is lower than minItems 1"},
      {R"("name":"AB","codes":["AB","CD","AB"])", "/codes", "Items 0 and 2 are equal (uniqueItems)"},
      {R"("name":"AB","codes":["AB","C"])", "/codes/1", "Length 1 is lower than minLength 2"},
      {R"("name":"AB","tags":[{"id":1,"name":"a"},{"id":1,"name":"a"}])", "/tags", "Items 0 and 1 are equal (uniqueItems)"},
      {R"("name":"AB","weights":{"a":1,"b":2,"c":3})", "/weights", "Property count 3 is greater than maxProperties 2"},
      {R"("name":"AB","weights":{"a":-1})", "/weights/a", "Value -1 is lower than minimum 0"},
      {R"("name":"AB","grade":2)", "/grade", "Value 2 is not allowed (allowed: 0.5, 1, 1.5)"},
      {R"("name":"AB","flag":false)", "/flag", "Value false is not allowed (allowed: true)"},
      {R"("name":"Ab")", "/name", "Value does not match pattern ^[A-Z]+$"},
      {R"("name":"AB","codes":["AB","c1"])", "/codes/1", "Value does not match pattern ^[A-Z]+$"},
      {R"("name":"AB","email":"vincent")", "/email", "Value is not a valid email (format)"},
      {R"("name":"AB","ref":"12-345")", "/ref", "Value does not match pattern ^\\d{3}-\\d{2}$"}}) {
    std::string json = std::string("{") + fields + "}";
    std::cout << "Deserialize " << json << std::endl;
    std::istringstream input(json);
    try {
      [[maybe_unused]] Constrained value = anch::json::deserialize<Constrained>(input);
      anch::ut::fail(std::string("Constraint should be checked: ") + json);
    } catch(const anch::json::MappingError& e) {
      std::cout << e.what() << std::endl;
      anch::ut::assertThat(e.getErrorCode() == ErrorCode::CONSTRAINT_VIOLATION, "CONSTRAINT_VIOLATION expected");
      anch::ut::assertThat(e.getPath() == path, "Expected path '{}' ; found '{}'", path, e.getPath());
      anch::ut::assertThat(std::string(e.what()).find(message) != std::string::npos, "'{}' not found in '{}'", message, e.what());
    }
  }
}

void
testRequiredSerialize() {
  // Required nullable property is written as null: the document is valid for the server
  Adoption adoption;
  adoption.petId = 1;
  std::string json = anch::json::serialize(adoption);
  anch::ut::assertThat(json == R"({"comment":null,"petId":1,"visitors":[]})", "Unexpected JSON {}", json);
  std::istringstream input(json);
  [[maybe_unused]] Adoption copy = anch::json::deserialize<Adoption>(input);
}

void
testConstraintsSerialize() {
  // Client side: requests are checked before being sent, responses are trusted
  anch::json::MappingOptions client;
  client.serialize_check_constraints = true;
  client.deserialize_check_constraints = false;
  Constrained value;
  value.name = "AB";
  value.codes = {"AB", "c"};
  try {
    std::string json = anch::json::serialize(value, client);
    anch::ut::fail("Invalid request should not be serialized: " + json);
  } catch(const anch::json::MappingError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assertThat(e.getPath() == "/codes/1", "Expected path '/codes/1' ; found '{}'", e.getPath());
  }
  std::istringstream response(R"({"codes":["c"],"count":42})");
  Constrained trusted = anch::json::deserialize<Constrained>(response, client);
  anch::ut::assertThat(trusted.name.empty() && trusted.count == 42, "Response should be trusted");
}

void
testValidate() {
  Constrained value;
  value.name = "a1";
  value.count = 10;
  value.codes = {"AB", "AB", "x"};
  value.weights = {{"a", -1.0}};
  std::vector<anch::json::Violation> violations = anch::json::validate(value);
  for(const anch::json::Violation& violation : violations) {
    std::cout << violation.path << ": " << violation.message << std::endl;
  }
  std::vector<std::string> paths;
  for(const anch::json::Violation& violation : violations) {
    paths.push_back(violation.path);
  }
  std::vector<std::string> expected = {"/codes", "/codes/2", "/codes/2", "/count", "/name", "/weights/a"};
  anch::ut::assertThat(paths == expected, "Unexpected violations paths");
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
    .add("models-required-missing", testRequiredMissing)
    .add("models-required-null", testRequiredNull)
    .add("models-required-nullable", testRequiredNullable)
    .add("models-constraints", testConstraints)
    .add("models-constraints-invalid", testConstraintsInvalid)
    .add("models-required-serialize", testRequiredSerialize)
    .add("models-constraints-serialize", testConstraintsSerialize)
    .add("models-validate", testValidate)
    .add("models-date-invalid", testDateInvalid)
    ;
}
