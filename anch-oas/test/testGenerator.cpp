/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "json/mappingError.hpp"
#include "oas/modelGenerator.hpp"
#include "oas/spec.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::oas::GeneratedModels;
using anch::oas::GeneratorError;
using anch::oas::ModelOptions;


/*!
 * OpenAPI document with schemas
 */
std::string
spec(const std::string& schemas) {
  return R"({"openapi":"3.0.3","info":{"title":"Test","version":"1"},"components":{"schemas":)" + schemas + "}}";
}

/*!
 * Generate models from document
 */
GeneratedModels
generate(const std::string& document, const ModelOptions& options = ModelOptions()) {
  std::istringstream input(document);
  GeneratedModels models = anch::oas::generateModels(anch::oas::parse(input), options);
  for(const std::string& warning : models.warnings) {
    std::cout << "warning: " << warning << std::endl;
  }
  return models;
}

/*!
 * Check that function raises exception with message which contains text
 */
template<typename E>
void
checkError(std::function<void()> func, const std::string& expected) {
  try {
    func();
    anch::ut::fail("Error expected: " + expected);
  } catch(const E& error) {
    std::string message = error.what();
    std::cout << "Error: " << message << std::endl;
    anch::ut::assertThat(message.find(expected) != std::string::npos, "'{}' expected in '{}'", expected, message);
  }
}

void
contains(const std::string& text, const std::string& expected) {
  anch::ut::assertThat(text.find(expected) != std::string::npos, "'{}' not found in:\n{}", expected, text);
}

void
testVersion() {
  checkError<std::invalid_argument>([]() { generate(R"({"openapi":"3.1.0","info":{"title":"t","version":"1"}})"); }, "Unsupported OpenAPI version 3.1.0");
  checkError<std::invalid_argument>([]() { generate(R"({"swagger":"2.0","info":{"title":"t","version":"1"}})"); }, "Swagger 2.0");
  GeneratedModels models = generate(R"({"openapi":"3.0.0","info":{"title":"t","version":"1"}})");
  contains(models.header, "namespace model {");
}

void
testInvalidJson() {
  checkError<anch::json::MappingError>([]() { generate(R"({"openapi":"3.0.0","components":{"schemas":{"A":{"type":}}}})"); }, "");
}

void
testUnknownReference() {
  checkError<GeneratorError>([]() {
    generate(spec(R"({"Pet":{"type":"object","properties":{"x":{"$ref":"#/components/schemas/Missing"}}}})"));
  }, "Pet.x: Unknown reference '#/components/schemas/Missing'");
}

void
testExternalReference() {
  GeneratedModels models = generate(spec(R"({"Pet":{"type":"object","properties":{"x":{"$ref":"other.json#/components/schemas/X"}}}})"));
  anch::ut::assertThat(models.warnings.size() == 1, "1 warning expected ; found {}", models.warnings.size());
  contains(models.warnings[0], "Pet.x: reference 'other.json#/components/schemas/X'");
  contains(models.header, "// Not generated: x: reference");
}

void
testStrict() {
  ModelOptions options;
  options.strict = true;
  checkError<GeneratorError>([&]() {
    generate(spec(R"({"Pet":{"type":"object","properties":{"p":{"oneOf":[{"type":"string"},{"type":"integer"}]}}}})"), options);
  }, "Pet.p: oneOf / anyOf");
}

void
testOptions() {
  for(const char* ns : {"", "a::", "::a", "a:b", "a::1b", "class", "a-b"}) {
    ModelOptions options;
    options.ns = ns;
    checkError<GeneratorError>([&]() { generate(spec("{}"), options); }, "Invalid namespace");
  }
  for(const char* name : {"", "a/b", "a\\b"}) {
    ModelOptions options;
    options.name = name;
    checkError<GeneratorError>([&]() { generate(spec("{}"), options); }, "Invalid file name");
  }
  ModelOptions options;
  options.ns = "my_api::v1::model";
  options.name = "api";
  GeneratedModels models = generate(spec(R"({"A":{"type":"object","properties":{"a":{"type":"string"}}}})"), options);
  contains(models.header, "namespace my_api::v1::model {");
  contains(models.source, "#include \"api.hpp\"");
  contains(models.source, "anch::json::ObjectMapper<::my_api::v1::model::A>& mapper");
}

void
testMemberNames() {
  GeneratedModels models = generate(spec(R"({"A":{"type":"object","required":["class"],"properties":{
    "class":{"type":"string"},"first-name":{"type":"string"},"first_name":{"type":"string"},"$id":{"type":"string"},
    "2fa":{"type":"boolean"},"_Links":{"type":"string"},"NULL":{"type":"string"},"errno":{"type":"string"},"B":{"type":"string"}}},
    "B":{"type":"object","properties":{"a":{"type":"string"}}}})"));
  contains(models.header, "std::string class_;");
  contains(models.header, "std::optional<std::string> first_name;");
  contains(models.header, "std::optional<std::string> first_name_;");
  contains(models.header, "std::optional<std::string> _id;");
  contains(models.header, "std::optional<bool> _2fa;");
  contains(models.header, "std::optional<std::string> m_Links;");
  contains(models.header, "std::optional<std::string> NULL_;");
  contains(models.header, "std::optional<std::string> errno_;");
  contains(models.header, "std::optional<std::string> B_;");
  contains(models.source, ".registerField(\"class\", &::model::A::class_)");
  contains(models.source, ".registerField<std::string>(\"$id\", &::model::A::_id)");
}

void
testTypeNames() {
  GeneratedModels models = generate(spec(R"({"pet-store":{"type":"object","properties":{"a":{"type":"string"}}},
    "pet_store":{"type":"object","properties":{"a":{"type":"string"}}},"2fa":{"type":"string"},"my.status":{"type":"string","enum":["a"]}})"));
  contains(models.header, "struct PetStore {");
  contains(models.header, "struct PetStore2 {");
  contains(models.header, "using M2fa = std::string;");
  contains(models.header, "enum class MyStatus {");
}

void
testEnumValues() {
  GeneratedModels models = generate(spec(R"({"E":{"type":"string","enum":["in-stock","inStock","","null","a\"b\\c","2x","in-stock",null]}})"));
  contains(models.header, "IN_STOCK, ");
  contains(models.header, "IN_STOCK_2, ");
  contains(models.header, "EMPTY, ");
  contains(models.header, "NULL_, ");
  contains(models.header, "A_B_C, ");
  contains(models.header, "_2X ");
  contains(models.source, "{\"a\\\"b\\\\c\", ::model::E::A_B_C}");
  anch::ut::assertThat(models.source.find("{\"in-stock\", ::model::E::IN_STOCK_2}") == std::string::npos, "Duplicated value should be ignored");
}

void
testRecursiveAlias() {
  GeneratedModels models = generate(spec(R"({"A":{"type":"array","items":{"$ref":"#/components/schemas/A"}}})"));
  anch::ut::assertThat(models.warnings.size() == 1, "1 warning expected ; found {}", models.warnings.size());
  contains(models.warnings[0], "recursive reference '#/components/schemas/A'");
}

void
testInheritedWarnings() {
  GeneratedModels models = generate(spec(R"({"A":{"type":"object","properties":{"any":{}}},
    "B":{"allOf":[{"$ref":"#/components/schemas/A"}]},"C":{"allOf":[{"$ref":"#/components/schemas/B"}]}})"));
  anch::ut::assertThat(models.warnings.size() == 1, "1 warning expected ; found {}", models.warnings.size());
  contains(models.warnings[0], "A.any: schema without type");
}

void
testRecursiveAllOf() {
  GeneratedModels models = generate(spec(R"({"A":{"allOf":[{"$ref":"#/components/schemas/B"}]},"B":{"allOf":[{"$ref":"#/components/schemas/A"}]}})"));
  anch::ut::assertThat(models.warnings.size() == 2, "2 warnings expected ; found {}", models.warnings.size());
  contains(models.warnings[0], "recursive allOf composition");
}

void
testNoUuid() {
  std::string document = spec(R"({"A":{"type":"object","properties":{"id":{"type":"string","format":"uuid"}}}})");
  GeneratedModels models = generate(document);
  contains(models.header, "#include \"uuid.hpp\"");
  contains(models.header, "std::optional<anch::UUID> id;");
  ModelOptions options;
  options.uuid = false;
  models = generate(document, options);
  anch::ut::assertThat(models.header.find("uuid.hpp") == std::string::npos, "uuid.hpp should not be included");
  contains(models.header, "std::optional<std::string> id;");
}

void
testNoDate() {
  std::string document = spec(R"({"A":{"type":"object","required":["at","day"],"properties":{
    "at":{"type":"string","format":"date-time"},"day":{"type":"string","format":"date"},
    "when":{"type":"string","format":"date-time"},"days":{"type":"array","items":{"type":"string","format":"date"}}}}})");
  GeneratedModels models = generate(document);
  contains(models.header, "#include <chrono>");
  contains(models.header, "#include \"date/date.hpp\"");
  contains(models.header, "anch::date::Date at{false};");
  contains(models.header, "std::chrono::year_month_day day{};");
  contains(models.header, "std::optional<anch::date::Date> when;");
  contains(models.header, "std::vector<std::chrono::year_month_day> days;");
  contains(models.source, ".registerField<std::chrono::year_month_day>(\"days\", &::model::A::days)");
  ModelOptions options;
  options.date = false;
  models = generate(document, options);
  anch::ut::assertThat(models.header.find("date/date.hpp") == std::string::npos, "date/date.hpp should not be included");
  contains(models.header, "std::string at;");
  contains(models.header, "std::optional<std::string> when;");
}

void
testPointers() {
  GeneratedModels models = generate(spec(R"({"N":{"type":"object","required":["self"],"properties":{
    "self":{"$ref":"#/components/schemas/N"},"list":{"type":"array","items":{"$ref":"#/components/schemas/N"}},
    "map":{"type":"object","additionalProperties":{"$ref":"#/components/schemas/N"}},"other":{"$ref":"#/components/schemas/O"}}},
    "O":{"type":"object","properties":{"a":{"type":"string"}}}})"));
  contains(models.header, "std::shared_ptr<N> self;");
  contains(models.header, "std::vector<N> list;");
  contains(models.header, "std::map<std::string,N> map;");
  contains(models.header, "std::optional<O> other;");
  // O is declared before N
  anch::ut::assertThat(models.header.find("struct O {") < models.header.find("struct N {"), "O should be declared before N");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH OAS generator unit tests")
    .description("Test OpenAPI document parsing and models generation")
    .add("spec-version", testVersion)
    .add("spec-invalid-json", testInvalidJson)
    .add("gen-unknown-reference", testUnknownReference)
    .add("gen-external-reference", testExternalReference)
    .add("gen-strict", testStrict)
    .add("gen-options", testOptions)
    .add("gen-member-names", testMemberNames)
    .add("gen-type-names", testTypeNames)
    .add("gen-enum-values", testEnumValues)
    .add("gen-recursive-alias", testRecursiveAlias)
    .add("gen-inherited-warnings", testInheritedWarnings)
    .add("gen-recursive-allof", testRecursiveAllOf)
    .add("gen-no-uuid", testNoUuid)
    .add("gen-no-date", testNoDate)
    .add("gen-pointers", testPointers)
    ;
}
