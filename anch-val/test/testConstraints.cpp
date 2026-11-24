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
#include "val/constraints.hpp"
#include "val/validation.hpp"

#include <cstdint>
#include <limits>
#include <chrono>
#include <map>
#include <sstream>
#include <regex>
#include <utility>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::val::ValidationContext;
using anch::val::Violation;

namespace {

  // Check one constraint: message when value is invalid, empty otherwise
  template<typename C, typename V>
  std::string
  violation(const C& constraint, const V& value) {
    std::string message;
    return constraint.check(value, message) ? std::string() : message;
  }

  // Validate value with constraints: collected violations
  template<typename V, typename... C>
  std::vector<Violation>
  validate(const V& value, const C&... constraints) {
    ValidationContext context;
    anch::val::detail::checkConstraints(value, context, constraints...);
    return context.violations();
  }

  // Context of a format: violations are errors with their path, constraints can be disabled
  struct FormatContext {
    bool check = true;
    std::string path;
  };

  struct FormatError: std::runtime_error {
    using std::runtime_error::runtime_error;
  };

}

template<>
struct anch::val::CheckContext<FormatContext> {
  static bool
  enabled(const FormatContext& context) {
    return context.check;
  }

  static void
  violation(FormatContext& context, const std::string& message) {
    throw FormatError(context.path + ": " + message);
  }

  template<typename F>
  static void
  item(FormatContext& context, std::string_view key, F&& check) {
    const std::size_t size = context.path.size();
    context.path += "/" + std::string(key);
    std::forward<F>(check)();
    context.path.resize(size);
  }

  template<typename F>
  static void
  item(FormatContext& context, std::size_t index, F&& check) {
    const std::size_t size = context.path.size();
    context.path += "[" + std::to_string(index) + "]";
    std::forward<F>(check)();
    context.path.resize(size);
  }
};

// Constraints kinds and applicability +
static_assert(anch::val::FieldConstraint<anch::val::Required, int>);
static_assert(anch::val::ValueConstraint<anch::val::MinLength, std::string>);
static_assert(!anch::val::ValueConstraint<anch::val::MinLength, int>);
static_assert(anch::val::ValueConstraint<anch::val::Minimum<int>, double>);
static_assert(anch::val::ValueConstraint<decltype(anch::val::items(anch::val::minLength(1))), std::vector<std::string>>);
static_assert(!anch::val::ValueConstraint<decltype(anch::val::items(anch::val::minLength(1))), std::vector<int>>);
static_assert(anch::val::isRequired<anch::val::Required, anch::val::MinLength>());
static_assert(anch::val::isNullable<std::optional<int>>() && anch::val::isNullable<std::shared_ptr<int>>() && !anch::val::isNullable<int>());
// Constraints are literal values: schema generators read them
static_assert(anch::val::minLength(3).value == 3 && anch::val::maxItems(4).value == 4);
static_assert(anch::val::exclusiveMinimum(0.5).value == 0.5 && anch::val::exclusiveMinimum(0.5).exclusive);
// Constraints kinds and applicability -

void
testStrings() {
  // Length in Unicode code points (UTF-8)
  anch::ut::assertThat(violation(anch::val::minLength(3), std::string("été")).empty(), "3 code points");
  anch::ut::assertThat(violation(anch::val::minLength(4), std::string_view("été")) == "Length 3 is lower than minLength 4",
		       "Unexpected message '{}'", violation(anch::val::minLength(4), std::string_view("été")));
  anch::ut::assertThat(violation(anch::val::maxLength(2), "abc") == "Length 3 is greater than maxLength 2", "maxLength");
  anch::val::Pattern digits = anch::val::pattern("^[0-9]+$");
  anch::ut::assertThat(violation(digits, std::string("123")).empty() && violation(digits, std::string("12a")) == "Value does not match pattern ^[0-9]+$",
		       "pattern");
  anch::val::Pattern search = anch::val::pattern("b+");
  anch::ut::assertThat(violation(search, std::string("abbc")).empty(), "ECMAScript patterns search the value (JSON Schema)");
}

void
testNumbers() {
  anch::ut::assertThat(violation(anch::val::minimum(0), -1) == "Value -1 is lower than minimum 0", "minimum");
  anch::ut::assertThat(violation(anch::val::minimum(0), 0).empty(), "minimum is inclusive");
  anch::ut::assertThat(violation(anch::val::exclusiveMinimum(0), 0) == "Value 0 is lower than or equal to exclusiveMinimum 0", "exclusiveMinimum");
  anch::ut::assertThat(violation(anch::val::maximum(10), 10.5) == "Value 10.5 is greater than maximum 10", "maximum on double");
  anch::ut::assertThat(violation(anch::val::exclusiveMaximum(10), 10).size() > 0, "exclusiveMaximum");
  // Integers are compared without conversion
  anch::ut::assertThat(violation(anch::val::minimum(-1), UINT64_MAX).empty() && violation(anch::val::maximum(UINT64_MAX), -1).empty(),
		       "Mixed signedness comparisons");
  anch::ut::assertThat(violation(anch::val::multipleOf(3), 9).empty() && violation(anch::val::multipleOf(3), 10) == "Value 10 is not a multiple of 3",
		       "multipleOf");
  anch::ut::assertThat(violation(anch::val::multipleOf(0.5), 1.5).empty(), "multipleOf on double");
}

void
testCollections() {
  const std::vector<int> values{1, 2, 2};
  anch::ut::assertThat(violation(anch::val::minItems(4), values) == "Item count 3 is lower than minItems 4", "minItems");
  anch::ut::assertThat(violation(anch::val::maxItems(2), values) == "Item count 3 is greater than maxItems 2", "maxItems");
  anch::ut::assertThat(violation(anch::val::uniqueItems, values) == "Items 1 and 2 are equal (uniqueItems)", "uniqueItems '{}'", violation(anch::val::uniqueItems, values));
  anch::ut::assertThat(violation(anch::val::uniqueItems, std::set<int>{1, 2}).empty(), "unique items");
  const std::map<std::string, int> map{{"a", 1}};
  anch::ut::assertThat(violation(anch::val::minProperties(2), map) == "Property count 1 is lower than minProperties 2", "minProperties");
  anch::ut::assertThat(violation(anch::val::maxProperties(0), map).size() > 0, "maxProperties");
}

void
testOthers() {
  auto colors = anch::val::allowed<std::string>({"red", "green"});
  anch::ut::assertThat(violation(colors, std::string("red")).empty() && violation(colors, std::string("blue")) == "Value 'blue' is not allowed (allowed: 'red', 'green')",
		       "allowed '{}'", violation(colors, std::string("blue")));
  auto even = anch::val::custom<int>([](const int& value) { return value % 2 == 0; }, "Value has to be even");
  anch::ut::assertThat(violation(even, 4).empty() && violation(even, 3) == "Value has to be even", "custom");
}

void
testFormats() {
  auto check = [](const std::string& name, std::string_view value) {
    return violation(anch::val::format(name), value).empty();
  };
  anch::ut::assertThat(check("email", "john.doe@example.com") && !check("email", "john..doe@example.com") && !check("email", "@example.com"), "email");
  anch::ut::assertThat(check("hostname", "www.example.com") && !check("hostname", "-www.example.com"), "hostname");
  anch::ut::assertThat(check("ipv4", "192.168.0.1") && !check("ipv4", "192.168.0.256"), "ipv4");
  anch::ut::assertThat(check("ipv6", "::1") && !check("ipv6", "::g"), "ipv6");
  anch::ut::assertThat(check("uri", "https://example.com/a%20b") && !check("uri", "no scheme"), "uri");
  anch::ut::assertThat(check("byte", "aGVsbG8=") && !check("byte", "a"), "byte (base64)");
#ifdef ANCH_TEST_UUID
  anch::ut::assertThat(check("uuid", "4a1b5c2d-3e4f-4a5b-8c6d-7e8f9a0b1c2d") && !check("uuid", "4a1b5c2d"), "uuid");
#endif
#ifdef ANCH_TEST_DATE
  anch::ut::assertThat(check("date-time", "2026-11-21T12:00:00Z") && !check("date-time", "2026-11-21"), "date-time");
  anch::ut::assertThat(check("date", "2026-11-21") && !check("date", "2026-13-01"), "date");
#endif
  anch::ut::assertThat(check("unknown-format", "anything"), "Unknown formats are not checked");
  anch::ut::assertThat(violation(anch::val::format("email"), std::string("x")) == "Value is not a valid email (format)", "Format message");
  anch::val::registerFormat("even-length", [](std::string_view value) { return value.size() % 2 == 0; });
  anch::ut::assertThat(check("even-length", "ab") && !check("even-length", "abc"), "Registered format");
}

void
testPatternCompiler() {
  anch::val::setPatternCompiler([](std::string_view source) {
    const std::string prefix(source);
    return anch::val::PatternMatcher([prefix](std::string_view value) { return value.starts_with(prefix); });
  });
  anch::val::Pattern prefix = anch::val::pattern("ab");
  anch::val::setPatternCompiler(anch::val::PatternCompiler());
  anch::ut::assertThat(violation(prefix, std::string("abc")).empty() && !violation(prefix, std::string("cab")).empty(), "Custom pattern compiler");
  anch::val::Pattern regex = anch::val::pattern("ab");
  anch::ut::assertThat(violation(regex, std::string("cab")).empty(), "Default compiler restored");
  // A pattern is compiled once (each format registers the description of a type), until its compiler is changed
  anch::ut::assertThat(anch::val::pattern("ab").matcher == regex.matcher && anch::val::xsdPattern("ab").matcher != regex.matcher,
		       "Compiled pattern has to be shared by syntax");
  anch::val::setPatternCompiler(anch::val::PatternCompiler());
  anch::ut::assertThat(anch::val::pattern("ab").matcher != regex.matcher, "Pattern has to be compiled by the new compiler");
}

void
testValidationContext() {
  // Every violation is collected with its path ; nullable values are checked when they are set
  std::vector<Violation> violations = validate(std::vector<std::string>{"ok", "", "a/b"}, anch::val::maxItems(2),
					       anch::val::items(anch::val::minLength(1), anch::val::maxLength(2)));
  anch::ut::assertThat(violations.size() == 3, "3 violations expected, found {}", violations.size());
  anch::ut::assertThat(violations[0].path.empty() && violations[1].path.pointer() == "/1" && violations[2].path.pointer() == "/2", "Unexpected paths");
  std::map<std::string, std::optional<int>> map{{"a/b~c", -1}, {"z", std::nullopt}};
  violations = validate(map, anch::val::items(anch::val::minimum(0)));
  anch::ut::assertThat(violations.size() == 1 && violations[0].path.pointer() == "/a~1b~0c", "Map key escaped in path (JSON Pointer): '{}'",
		       violations.empty() ? std::string() : violations[0].path.pointer());
  // Segments are kept as they are (key and index), formats format them
  anch::ut::assertThat(violations[0].path.segments() == std::vector<anch::val::Path::Segment>{std::string("a/b~c")},
		       "Map key segment is not escaped");
  std::vector<Violation> items = validate(std::vector<std::string>{"", "a"}, anch::val::items(anch::val::minLength(1)));
  anch::ut::assertThat(items.size() == 1 && items[0].path == anch::val::Path({std::size_t{0}}) && !items[0].path.empty(), "Index segment");
  std::ostringstream out;
  out << anch::val::Path({std::string("pets"), std::size_t{1}, std::string("name")}) << '|' << anch::val::Path();
  anch::ut::assertThat(out.str() == "/pets/1/name|", "Path written as a JSON Pointer: '{}'", out.str());
  anch::ut::assertThat(validate(std::optional<std::string>(), anch::val::minLength(1)).empty(), "Absent value is not checked");
  anch::ut::assertThat(validate(std::make_shared<std::string>(""), anch::val::minLength(1)).size() == 1, "Pointed value is checked");
  anch::ut::assertThat(validate(std::string(""), anch::val::required).empty(), "required is checked by formats");
}

void
testCheckContext() {
  // Format context: first violation raises its error with the path of the value
  FormatContext context;
  bool raised = false;
  try {
    anch::val::detail::checkConstraints(std::vector<std::string>{"a", ""}, context, anch::val::items(anch::val::minLength(1)));
  } catch(const FormatError& error) {
    raised = std::string(error.what()) == "[1]: Length 0 is lower than minLength 1";
  }
  anch::ut::assertThat(raised, "Format error with path expected");
  FormatContext disabled{false, ""};
  anch::val::detail::checkConstraints(std::string(""), disabled, anch::val::minLength(1));
}

void
testLength() {
  anch::ut::assertThat(violation(anch::val::length(3), std::string("été")).empty(), "3 code points");
  anch::ut::assertThat(violation(anch::val::length(2), std::string("été")) == "Length 3 is not length 2", "length on string");
  anch::ut::assertThat(violation(anch::val::length(2), std::vector<int>{1, 2}).empty()
		       && violation(anch::val::length(3), std::vector<int>{1, 2}) == "Item count 2 is not length 3", "length on list");
  static_assert(!anch::val::ValueConstraint<anch::val::Length, int>);
}

void
testDigits() {
  // Leading zeros of the integer part and trailing zeros of the fraction are not counted
  anch::ut::assertThat(anch::val::detail::decimalDigits("0012.500") == std::make_pair(std::size_t{3}, std::size_t{1}), "0012.500");
  anch::ut::assertThat(anch::val::detail::decimalDigits("-0.001") == std::make_pair(std::size_t{3}, std::size_t{3}), "-0.001");
  anch::ut::assertThat(anch::val::detail::decimalDigits("100") == std::make_pair(std::size_t{3}, std::size_t{0}), "100");
  anch::ut::assertThat(anch::val::detail::decimalDigits("0") == std::make_pair(std::size_t{1}, std::size_t{0})
		       && anch::val::detail::decimalDigits(".5") == std::make_pair(std::size_t{1}, std::size_t{1})
		       && anch::val::detail::decimalDigits("+5.") == std::make_pair(std::size_t{1}, std::size_t{0}), "0, .5, +5.");
  for(std::string_view invalid : {"", "-", ".", "1e5", "1.2.3", " 1", "0x10"}) {
    anch::ut::assertThat(!anch::val::detail::decimalDigits(invalid).has_value(), "'{}' is not a decimal", invalid);
  }
  anch::ut::assertThat(violation(anch::val::totalDigits(3), 999).empty() && violation(anch::val::totalDigits(3), -1000) == "Value -1000 has 4 digits, more than totalDigits 3",
		       "totalDigits on integer '{}'", violation(anch::val::totalDigits(3), -1000));
  anch::ut::assertThat(violation(anch::val::totalDigits(3), 12.5).empty() && !violation(anch::val::totalDigits(2), 12.5).empty(), "totalDigits on double");
  anch::ut::assertThat(violation(anch::val::fractionDigits(2), 0.25).empty()
		       && violation(anch::val::fractionDigits(2), 0.125) == "Value 0.125 has 3 fraction digits, more than fractionDigits 2",
		       "fractionDigits on double '{}'", violation(anch::val::fractionDigits(2), 0.125));
  anch::ut::assertThat(violation(anch::val::fractionDigits(0), UINT64_MAX).empty(), "Integers have no fraction digit");
  anch::ut::assertThat(violation(anch::val::fractionDigits(2), std::string("19.990")).empty()
		       && violation(anch::val::totalDigits(30), std::string("123456789012345678901234567890.1")).size() > 0, "Decimal strings");
  anch::ut::assertThat(violation(anch::val::totalDigits(5), std::string("12a")) == "Value '12a' is not a decimal", "Not a decimal");
  anch::ut::assertThat(violation(anch::val::totalDigits(5), std::numeric_limits<double>::infinity()) == "Value inf is not a decimal",
		       "Infinity is not a decimal '{}'", violation(anch::val::totalDigits(5), std::numeric_limits<double>::infinity()));
}

void
testXsdPattern() {
  // Whole value, ^ and $ are ordinary characters
  anch::val::Pattern zip = anch::val::xsdPattern("\\d{5}");
  anch::ut::assertThat(zip.syntax == anch::val::PatternSyntax::XSD && violation(zip, std::string("75001")).empty()
		       && !violation(zip, std::string("750012")).empty() && !violation(zip, std::string("a75001")).empty(), "Whole value");
  anch::ut::assertThat(violation(anch::val::xsdPattern("$[0-9]+\\.[0-9]{2}"), std::string("$10.50")).empty()
		       && violation(anch::val::xsdPattern("^a$"), std::string("^a$")).empty(), "^ and $ are ordinary characters");
  anch::ut::assertThat(violation(anch::val::xsdPattern("[^a-c]+(-[A-Z]{2})?"), std::string("xyz-FR")).empty()
		       && !violation(anch::val::xsdPattern("[^a-c]+"), std::string("xay")).empty(), "Classes, groups, quantifiers");
  anch::ut::assertThat(violation(anch::val::xsdPattern("a\\sb[\\s\\d]c"), std::string("a\tb7c")).empty()
		       && !violation(anch::val::xsdPattern("a\\sb"), std::string("a\vb")).empty(), "\\s is space, tab, CR or LF");
  anch::ut::assertThat(violation(anch::val::xsdPattern("a|bc"), std::string("bc")).empty()
		       && !violation(anch::val::xsdPattern("a|bc"), std::string("abc")).empty(), "Branches match the whole value");
  for(std::string_view unsupported : {"\\p{L}+", "\\i\\c*", "\\w+", "[a-z-[aeiou]]", "[\\S]"}) {
    bool raised = false;
    try {
      anch::val::xsdPattern(unsupported);
    } catch(const std::invalid_argument& error) {
      raised = std::string_view(error.what()).find("not supported by the default compiler") != std::string_view::npos;
    }
    anch::ut::assertThat(raised, "'{}' is not supported by the default compiler", unsupported);
  }
  for(std::string_view invalid : {"(?:a)", "a*?", "\\b", "\\1", "[a-z", "a\\"}) {
    bool raised = false;
    try {
      anch::val::xsdPattern(invalid);
    } catch(const std::invalid_argument&) {
      raised = true;
    } catch(const std::regex_error&) {
      raised = true;
    }
    anch::ut::assertThat(raised, "'{}' is not a valid XSD pattern", invalid);
  }
  // Compiler by syntax: XSD compiler does not change ECMAScript patterns
  anch::val::setPatternCompiler(anch::val::PatternSyntax::XSD, [](std::string_view source) {
    const std::string expected(source);
    return anch::val::PatternMatcher([expected](std::string_view value) { return value == expected; });
  });
  anch::val::Pattern literal = anch::val::xsdPattern("\\p{L}");
  anch::val::Pattern ecma = anch::val::pattern("b");
  anch::val::setPatternCompiler(anch::val::PatternSyntax::XSD, anch::val::PatternCompiler());
  anch::ut::assertThat(violation(literal, std::string("\\p{L}")).empty() && violation(ecma, std::string("abc")).empty(), "Compiler by syntax");
}

// Described types (anch::Mapping, without format) +
namespace {

  struct Tag {
    std::string name;
  };

  struct Pet {
    std::string name;
    int age = 0;
    std::vector<Tag> tags;
    std::optional<Tag> main;
    std::map<int, Tag> byRank;
    Pet* parent = nullptr;
    int computed() const { return -age; }
  };

}

template<>
struct anch::Mapping<Tag> {
  static void
  registerObject(anch::ObjectMapper<Tag> auto& mapper) {
    mapper.registerField("name", &Tag::name, anch::val::required, anch::val::minLength(1));
  }
};

template<>
struct anch::Mapping<Pet> {
  static void
  registerObject(anch::ObjectMapper<Pet> auto& mapper) {
    mapper
      .registerField("name", &Pet::name, anch::val::required, anch::val::minLength(2))
      .registerField("age", &Pet::age, anch::val::minimum(0))
      .registerField("tags", &Pet::tags, anch::val::maxItems(2))
      .registerField("main", &Pet::main)
      .registerField("byRank", &Pet::byRank)
      .registerField("parent", &Pet::parent)
      .registerField("computed", std::function<int(const Pet&)>(&Pet::computed), anch::val::maximum(0))
      .registerField("written", std::function<void(Pet&, const int&)>([](Pet& pet, const int& value) { pet.age = value; }),
		     anch::val::minimum(100))
      ;
  }
};
static_assert(anch::ObjectMapper<anch::val::ValidationMapper<Pet>, Pet>);

// A class which is neither described nor a scalar is refused (its description is probably not visible)
namespace {

  struct Undescribed {
    int value;
  };

  // Value type mapped as a string by formats
  struct Money {
    long cents;
  };

  struct Wallet {
    Money balance;
  };

}

template<>
struct anch::val::IsScalar<Money>: std::true_type {};

template<>
struct anch::Mapping<Wallet> {
  static void
  registerObject(anch::ObjectMapper<Wallet> auto& mapper) {
    mapper.registerField("balance", &Wallet::balance, anch::val::custom<Money>([](const Money& money) { return money.cents >= 0; },
									       "Balance can not be negative"));
  }
};

static_assert(anch::val::Validatable<Pet> && anch::val::Validatable<std::vector<std::optional<Tag>>>
	      && anch::val::Validatable<std::map<std::string, Pet*>>);
static_assert(anch::val::Validatable<int> && anch::val::Validatable<std::string> && anch::val::Validatable<anch::UUID>
	      && anch::val::Validatable<anch::date::Date> && anch::val::Validatable<std::chrono::system_clock::time_point>);
static_assert(!anch::val::Validatable<Undescribed> && !anch::val::Validatable<std::vector<Undescribed>>
	      && !anch::val::Validatable<std::optional<Undescribed>> && !anch::val::Validatable<std::map<std::string, Undescribed>>);
static_assert(anch::val::Validatable<Money> && anch::val::Validatable<Wallet>, "Declared scalars are validated as a whole");
// Described types -

void
testValidate() {
  Pet parent{"P", 3, {}, std::nullopt, {}, nullptr};
  Pet pet{"Rex", 2, {{"dog"}, {""}, {"a"}}, Tag{""}, {{7, Tag{""}}}, &parent};
  const std::vector<Violation> violations = anch::val::validate(pet);
  const std::vector<std::pair<std::string, std::string>> expected{
    {"/tags", "Item count 3 is greater than maxItems 2"},
    {"/tags/1/name", "Length 0 is lower than minLength 1"},
    {"/main/name", "Length 0 is lower than minLength 1"},
    {"/byRank/7/name", "Length 0 is lower than minLength 1"},
    {"/parent/name", "Length 1 is lower than minLength 2"}};
  anch::ut::assertThat(violations.size() == expected.size(), "{} violations expected, found {}", expected.size(), violations.size());
  for(std::size_t i = 0; i < expected.size() && i < violations.size(); ++i) {
    anch::ut::assertThat(violations[i].path.pointer() == expected[i].first && violations[i].message == expected[i].second,
			 "Violation {}: '{}' {}", i, violations[i].path.pointer(), violations[i].message);
  }
  // Getter value is checked, setter only field is not, required is not checked
  Pet invalid{"Médor", -1, {}, std::nullopt, {}, nullptr};
  const std::vector<Violation> getter = anch::val::validate(invalid);
  anch::ut::assertThat(getter.size() == 2 && getter[0].path.pointer() == "/age" && getter[1].path.pointer() == "/computed", "age and computed violations expected");
  anch::ut::assertThat(anch::val::isValid(Pet{"Médor", 1, {}, std::nullopt, {}, nullptr}), "Valid pet");
  // Collections of described objects and primitives
  anch::ut::assertThat(anch::val::validate(std::vector<Tag>{{"a"}, {""}}).size() == 1
		       && anch::val::validate(std::vector<Tag>{{"a"}, {""}})[0].path.pointer() == "/1/name", "Collection of objects");
  anch::ut::assertThat(anch::val::isValid(42) && anch::val::isValid(std::string()), "Primitive values are valid");
  // Declared scalar: constraints of the field are checked, the value has no field
  const std::vector<Violation> wallet = anch::val::validate(Wallet{Money{-1}});
  anch::ut::assertThat(wallet.size() == 1 && wallet[0].path.pointer() == "/balance" && wallet[0].message == "Balance can not be negative",
		       "Declared scalar constraint");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH validation unit tests")
    .description("Test AnCH validation constraints")
    .add("strings", testStrings)
    .add("numbers", testNumbers)
    .add("collections", testCollections)
    .add("others", testOthers)
    .add("formats", testFormats)
    .add("pattern-compiler", testPatternCompiler)
    .add("validation-context", testValidationContext)
    .add("check-context", testCheckContext)
    .add("length", testLength)
    .add("digits", testDigits)
    .add("xsd-pattern", testXsdPattern)
    .add("validate", testValidate)
    ;
}
