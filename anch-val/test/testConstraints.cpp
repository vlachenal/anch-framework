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

#include <cstdint>
#include <map>
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
}

void
testValidationContext() {
  // Every violation is collected with its path ; nullable values are checked when they are set
  std::vector<Violation> violations = validate(std::vector<std::string>{"ok", "", "a/b"}, anch::val::maxItems(2),
					       anch::val::items(anch::val::minLength(1), anch::val::maxLength(2)));
  anch::ut::assertThat(violations.size() == 3, "3 violations expected, found {}", violations.size());
  anch::ut::assertThat(violations[0].path.empty() && violations[1].path == "/1" && violations[2].path == "/2", "Unexpected paths");
  std::map<std::string, std::optional<int>> map{{"a/b~c", -1}, {"z", std::nullopt}};
  violations = validate(map, anch::val::items(anch::val::minimum(0)));
  anch::ut::assertThat(violations.size() == 1 && violations[0].path == "/a~1b~0c", "Map key escaped in path (JSON Pointer): '{}'",
		       violations.empty() ? std::string() : violations[0].path);
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
    ;
}
