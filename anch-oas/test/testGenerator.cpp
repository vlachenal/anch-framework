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
#include <algorithm>
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
  contains(models.source, ".registerField(\"class\", &::model::A::class_, anch::json::required)");
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
testRequired() {
  GeneratedModels models = generate(spec(R"({"A":{"type":"object","required":["a","n","l","r"],"properties":{
    "a":{"type":"string"},"n":{"type":"string","nullable":true},"l":{"type":"array","items":{"type":"integer"}},
    "r":{"$ref":"#/components/schemas/A"},"o":{"type":"string"}}},
    "B":{"allOf":[{"$ref":"#/components/schemas/A"},{"type":"object","required":["b"],"properties":{"b":{"type":"boolean"}}}]}})"));
  contains(models.header, "std::string a;");
  contains(models.header, "std::optional<std::string> n;");
  contains(models.source, ".registerField(\"a\", &::model::A::a, anch::json::required)");
  contains(models.source, ".registerField<std::string>(\"n\", &::model::A::n, anch::json::required)");
  contains(models.source, ".registerField<int64_t>(\"l\", &::model::A::l, anch::json::required)");
  contains(models.source, ".registerField<::model::A>(\"r\", &::model::A::r, anch::json::required)");
  contains(models.source, ".registerField<std::string>(\"o\", &::model::A::o)\n");
  // allOf: inherited and own required properties
  contains(models.source, ".registerField(\"a\", &::model::B::a, anch::json::required)");
  contains(models.source, ".registerField(\"b\", &::model::B::b, anch::json::required)");
}

void
testConstraints() {
  std::string document = spec(R"({"Id":{"type":"string","minLength":1},"A":{"type":"object","properties":{
    "s":{"type":"string","minLength":1,"maxLength":64,"pattern":"^a+$"},"id":{"$ref":"#/components/schemas/Id"},
    "i":{"type":"integer","format":"int32","minimum":0,"maximum":100,"exclusiveMaximum":true},
    "u":{"type":"integer","maximum":18446744073709551615,"minimum":-9223372036854775808},"d":{"type":"number","multipleOf":0.01,"minimum":1e3},
    "l":{"type":"array","minItems":1,"maxItems":5,"uniqueItems":true,"items":{"$ref":"#/components/schemas/Id"}},
    "m":{"type":"object","minProperties":1,"maxProperties":3,"additionalProperties":{"type":"integer","minimum":0}},
    "e":{"type":"integer","enum":[1,2,9007199254740993]},"b":{"type":"boolean","enum":[true,null]},"uid":{"type":"string","format":"uuid","maxLength":36},
    "mail":{"type":"string","format":"email"},"esc":{"type":"string","pattern":"^\\d+\"$"}}}})");
  GeneratedModels models = generate(document);
  anch::ut::assertThat(models.warnings.size() == 1, "1 warning expected (u maximum outside int64_t range) ; found {}", models.warnings.size());
  contains(models.warnings[0], "A.u: maximum 18446744073709551615 is outside int64_t range");
  contains(models.source, ".registerField<std::string>(\"s\", &::model::A::s, anch::json::minLength(1), anch::json::maxLength(64), "
	   "anch::json::pattern(\"^a+$\"))\n");
  contains(models.header, "Constraints: minLength 1, maxLength 64, pattern ^a+$");
  contains(models.source, ".registerField<std::string>(\"mail\", &::model::A::mail, anch::json::format(\"email\"))\n");
  contains(models.source, ".registerField<std::string>(\"esc\", &::model::A::esc, anch::json::pattern(\"^\\\\d+\\\"$\"))\n");
  contains(models.source, ".registerField<std::string>(\"id\", &::model::A::id, anch::json::minLength(1))\n");
  contains(models.source, ".registerField<int32_t>(\"i\", &::model::A::i, anch::json::minimum(0), anch::json::exclusiveMaximum(100))\n");
  contains(models.source, ".registerField<int64_t>(\"u\", &::model::A::u, anch::json::minimum((-9223372036854775807 - 1)), anch::json::maximum(18446744073709551615u))\n");
  contains(models.source, ".registerField<double>(\"d\", &::model::A::d, anch::json::minimum(1e3), anch::json::multipleOf(0.01))\n");
  contains(models.source, ".registerField<std::string>(\"l\", &::model::A::l, anch::json::minItems(1), anch::json::maxItems(5), "
	   "anch::json::uniqueItems, anch::json::items(anch::json::minLength(1)))\n");
  contains(models.header, "Constraints: minItems 1, maxItems 5, uniqueItems, items: minLength 1");
  contains(models.source, ".registerField<int64_t>(\"m\", &::model::A::m, anch::json::minProperties(1), anch::json::maxProperties(3), "
	   "anch::json::items(anch::json::minimum(0)))\n");
  contains(models.source, ".registerField<int64_t>(\"e\", &::model::A::e, anch::json::allowed<int64_t>({1, 2, 9007199254740993}))\n");
  contains(models.header, "Allowed values: 1, 2, 9007199254740993");
  contains(models.source, ".registerField<bool>(\"b\", &::model::A::b, anch::json::allowed<bool>({true}))\n");
  contains(models.source, ".registerField<anch::UUID>(\"uid\", &::model::A::uid)\n");
  contains(models.header, "bool operator==(const A&) const = default;");
  // No constraints option
  ModelOptions options;
  options.constraints = false;
  models = generate(document, options);
  anch::ut::assertThat(models.source.find("anch::json::m") == std::string::npos && models.source.find("anch::json::items") == std::string::npos
		       && models.source.find("anch::json::allowed") == std::string::npos, "No constraint expected:\n{}", models.source);
  anch::ut::assertThat(models.header.find("Constraints:") == std::string::npos, "No constraint documentation expected");
}

void
testConstraintsWarnings() {
  std::string document = spec(R"({"A":{"type":"object","properties":{
    "i":{"type":"integer","minLength":1},"s":{"type":"string","minimum":1,"multipleOf":2},"d":{"type":"number","multipleOf":0},
    "e":{"type":"integer","format":"int32","enum":[1,2.5]},"f":{"type":"integer","format":"int32","enum":[1,3000000000]},
    "b":{"type":"array","uniqueItems":true,"items":{"type":"boolean"}},"x":{"type":"number","maximum":"10"},
    "p":{"type":"string","pattern":"([a-z]"}}}})");
  GeneratedModels models = generate(document);
  for(const char* warning : {"A.i: minLength is ignored for int64_t value", "A.s: minimum is ignored for std::string value",
			     "A.s: multipleOf is ignored for std::string value", "A.d: multipleOf has to be a strictly positive number: ignored",
			     "A.e: enum value 2.5 is not a int32_t value: enum is not checked",
			     "A.f: enum value 3000000000 is not a int32_t value: enum is not checked",
			     "A.b: uniqueItems is not supported for boolean arrays: ignored", "A.x: maximum is not a number: ignored"}) {
    anch::ut::assertThat(std::find(models.warnings.begin(), models.warnings.end(), warning) != models.warnings.end(), "Warning '{}' not found", warning);
  }
  for(const char* warning : {"A.p: pattern ([a-z] is not a valid ECMAScript regular expression ("}) {
    anch::ut::assertThat(std::find_if(models.warnings.begin(), models.warnings.end(), [&warning](const std::string& found) -> bool {
	  return found.starts_with(warning);
	}) != models.warnings.end(), "Warning '{}' not found", warning);
  }
  anch::ut::assertThat(models.warnings.size() == 9, "9 warnings expected ; found {}", models.warnings.size());
  contains(models.source, ".registerField<std::string>(\"p\", &::model::A::p)\n");
  contains(models.source, ".registerField<int64_t>(\"i\", &::model::A::i)\n");
  contains(models.source, ".registerField<int32_t>(\"e\", &::model::A::e)\n");
  ModelOptions options;
  options.strict = true;
  checkError<GeneratorError>([&]() { generate(document, options); }, "A.b: uniqueItems is not supported for boolean arrays");
}

/*!
 * OpenAPI document with paths and schemas
 */
std::string
pathsSpec(const std::string& paths, const std::string& schemas = "{}") {
  return R"({"openapi":"3.0.3","info":{"title":"My pets","version":"1"},"paths":)" + paths + R"(,"components":{"schemas":)" + schemas + "}}";
}

void
testEndpoints() {
  std::string document = pathsSpec(R"({
    "/pets/{id}":{"parameters":[{"name":"id","in":"path","required":true,"schema":{"type":"integer","format":"int64"}}],
      "get":{"operationId":"get-pet","summary":"Get pet","parameters":[{"name":"full","in":"query","schema":{"type":"boolean"}}],
        "responses":{"200":{"description":"ok","content":{"application/json":{"schema":{"$ref":"#/components/schemas/Pet"}}}},"404":{"description":"no"}}},
      "post":{"operationId":"class","requestBody":{"content":{"application/json":{"schema":{"$ref":"#/components/schemas/Pet"}},"text/plain":{"schema":{"type":"string"}}}},
        "responses":{"202":{"description":"accepted"}}},
      "delete":{"responses":{"204":{"description":"deleted"}}}},
    "/pets":{"get":{"operationId":"listPets","parameters":[{"name":"q","in":"query","required":true,"schema":{"type":"string"}},
        {"name":"page","in":"query","schema":{"type":"integer","format":"int32"}}],
        "responses":{"200":{"description":"ok","content":{"application/json":{"schema":{"type":"array","items":{"$ref":"#/components/schemas/Pet"}}}}},
          "206":{"description":"partial","content":{"application/json":{"schema":{"type":"array","items":{"$ref":"#/components/schemas/Pet"}}}}}}},
      "put":{"operationId":"replace","requestBody":{"content":{"application/json":{"schema":{"type":"object","properties":{"a":{"type":"string"}}}}}},
        "responses":{"201":{"description":"ok","content":{"application/json":{"schema":{"type":"object","properties":{"b":{"type":"string"}}}}}}}}}})",
    R"({"Pet":{"type":"object","properties":{"name":{"type":"string"}}}})");
  ModelOptions options;
  options.server = true;
  options.client = true;
  GeneratedModels models = generate(document, options);
  // Service
  contains(models.serverHeader, "#include \"models.hpp\"");
  contains(models.serverHeader, "class MyPetsService {");
  contains(models.serverHeader, "virtual std::optional<Pet> getPet(int64_t id, const std::optional<bool>& full) = 0;");
  contains(models.serverHeader, "virtual void class_(int64_t id, const Pet& body) = 0;");
  contains(models.serverHeader, "virtual void deletePetsById(int64_t id) = 0;");
  contains(models.serverHeader, "virtual anch::rest::Response listPets(const std::string& q, const std::optional<int32_t>& page) = 0;");
  contains(models.serverHeader, "virtual ReplaceResponse replace(const ReplaceRequest& body) = 0;");
  contains(models.serverHeader, "void registerEndpoints(anch::rest::Api& api, MyPetsService& service);");
  // Registration
  contains(models.serverSource, "api.get(\"/pets/{id}\", [&service](const anch::rest::Request& request, int64_t id) -> std::optional<Pet> {\n"
	   "      return service.getPet(id, anch::rest::queryParameter<bool>(request, \"full\"));\n"
	   "    }).produces<anch::rest::Json>();");
  contains(models.serverSource, "api.post(\"/pets/{id}\", [&service](int64_t id, anch::rest::Body<Pet> body) -> void {\n"
	   "      service.class_(id, *body);\n"
	   "    }).consumes<anch::rest::Json>().status(anch::rest::Status::ACCEPTED);");
  contains(models.serverSource, "api.del(\"/pets/{id}\", [&service](int64_t id) -> void {");
  contains(models.serverSource, "return service.listPets(anch::rest::requiredQueryParameter<std::string>(request, \"q\"), "
	   "anch::rest::queryParameter<int32_t>(request, \"page\"));\n    });");
  contains(models.serverSource, ".consumes<anch::rest::Json>().produces<anch::rest::Json>().status(anch::rest::Status::CREATED);");
  // Inline models
  contains(models.header, "struct ReplaceRequest {");
  contains(models.header, "struct ReplaceResponse {");
  // Client
  contains(models.clientHeader, "class MyPetsClient {");
  contains(models.clientHeader, "std::optional<Pet> getPet(int64_t id, const std::optional<bool>& full = std::nullopt);");
  contains(models.clientHeader, "anch::rest::Response listPets(const std::string& q, const std::optional<int32_t>& page = std::nullopt);");
  contains(models.clientSource, "    anch::rest::Call<> call = _client.get(\"/pets/{id}\", id);\n"
	   "    if(full) {\n"
	   "      call.query(\"full\", *full);\n"
	   "    }\n"
	   "    try {\n"
	   "      return std::move(call).accept<anch::rest::Json>().as<Pet>();\n"
	   "    } catch(const anch::rest::HttpError& error) {\n"
	   "      if(error.getStatus() == anch::rest::Status::NOT_FOUND) {");
  contains(models.clientSource, "    call.body<anch::rest::Json>(body);\n    std::move(call).execute();");
  contains(models.clientSource, "    call.query(\"q\", q);\n");
  contains(models.clientSource, "    return std::move(call).send();");
  anch::ut::assertThat(models.warnings.size() == 1, "1 warning expected ; found {}", models.warnings.size());
  contains(models.warnings[0], "paths./pets/{id}.delete: operationId is missing, method is named deletePetsById");
  // Endpoints are generated only when asked
  models = generate(document);
  anch::ut::assertThat(models.serverHeader.empty() && models.clientHeader.empty(), "No endpoint expected");
  anch::ut::assertThat(models.header.find("ReplaceRequest") == std::string::npos, "Operations inline models are generated with endpoints only");
}

void
testEndpointsWarnings() {
  std::string document = pathsSpec(R"({
    "/a/{x}.json":{"get":{"operationId":"a","parameters":[{"name":"x","in":"path","required":true,"schema":{"type":"string"}}],"responses":{}}},
    "/b/{y}":{"get":{"operationId":"b","responses":{}}},
    "/c":{"get":{"operationId":"c","parameters":[{"name":"t","in":"query","schema":{"type":"array","items":{"type":"string"}}}],"responses":{}},
      "post":{"operationId":"d","requestBody":{"content":{"application/xml":{"schema":{"type":"string"}}}},"responses":{}},
      "put":{"operationId":"e","parameters":[{"name":"X-Id","in":"header","schema":{"type":"string"}}],"responses":{}},
      "options":{"responses":{}}},
    "/f/{s}":{"get":{"operationId":"f","parameters":[{"name":"s","in":"path","required":true,"schema":{"$ref":"#/components/schemas/S"}}],"responses":{}}}})",
    R"({"S":{"type":"string","enum":["a","b"]}})");
  ModelOptions options;
  options.server = true;
  options.api = "Test";
  GeneratedModels models = generate(document, options);
  for(const char* warning : {"paths./a/{x}.json.get: path segment '{x}.json' (anch-rest parameters are whole segments): not generated",
			     "paths./b/{y}.get: path parameter 'y' is not declared: not generated",
			     "paths./c.get: query parameter 't' of type std::vector<std::string> (anch-rest parameters are strings, numbers, booleans and UUID): not generated",
			     "paths./c.post: request body media types (only application/json and text/plain with schema are supported): not generated",
			     "paths./c.put: header parameter 'X-Id' is not generated",
			     "paths./c.options: HEAD, OPTIONS and TRACE methods (HEAD is served by GET routes): not generated",
			     "paths./f/{s}.get: path parameter 's' of type S (anch-rest parameters are strings, numbers, booleans and UUID): not generated"}) {
    anch::ut::assertThat(std::find(models.warnings.begin(), models.warnings.end(), warning) != models.warnings.end(), "Warning '{}' not found", warning);
  }
  anch::ut::assertThat(models.warnings.size() == 7, "7 warnings expected ; found {}", models.warnings.size());
  contains(models.serverHeader, "class TestService {");
  contains(models.serverHeader, "virtual void e() = 0;");
  anch::ut::assertThat(models.serverHeader.find(" a(") == std::string::npos && models.serverHeader.find(" c(") == std::string::npos,
		       "Unsupported operations should not be generated");
  // Strict mode
  options.strict = true;
  checkError<GeneratorError>([&]() { generate(document, options); }, "paths./a/{x}.json.get: path segment '{x}.json'");
  // Invalid API name
  ModelOptions invalid;
  invalid.server = true;
  invalid.api = "my-api";
  checkError<GeneratorError>([&]() { generate(document, invalid); }, "Invalid API name 'my-api'");
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
    .add("gen-required", testRequired)
    .add("gen-constraints", testConstraints)
    .add("gen-constraints-warnings", testConstraintsWarnings)
    .add("gen-endpoints", testEndpoints)
    .add("gen-endpoints-warnings", testEndpointsWarnings)
    ;
}
