#include <iostream>
#include <sstream>
#include <chrono>

#include "json/lexer.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::json::MappingError;
using anch::json::ErrorCode;

// Boolean +
void
testParseBooleanFalse() {
  std::istringstream sis("false");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  bool value = true;
  anch::json::lexBoolean(value, context);
  std::cout << "Result: " << std::boolalpha << value << std::endl;
  anch::ut::assert(anch::ut::equals("lex: ", false, value),
		   "Result should be false");
}

void
testParseBooleanTrue() {
  std::istringstream sis("true");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  bool value = false;
  anch::json::lexBoolean(value, context);
  std::cout << "Result: " << std::boolalpha << value << std::endl;
  anch::ut::assert(anch::ut::equals("lex: ", true, value),
		   "Result should be true");
}

void
testParseBooleanNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  bool value = true;
  anch::json::lexBoolean(value, context);
  std::cout << "Result: " << std::boolalpha << value << std::endl;
  anch::ut::assert(anch::ut::equals("lex: ", true, value),
		   "Result should remain true");
}

void
testParseBooleanKO() {
  std::istringstream sis("trasle");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  bool value = true;
  try {
    anch::json::lexBoolean(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() == ErrorCode::INVALID_FORMAT) {
      std::cout << e.what() << std::endl;
    } else {
      throw;
    }
  }
}
// Boolean -

// String +
void
testParseStr() {
  std::istringstream sis("\"test string\"");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::string value;
  bool found = anch::json::lexString(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", std::string("test string"), value),
		   "Result should parse 'test string'");
  std::cout << "Result: " << value << std::endl;
}

void
testParseStrNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::string value;
  bool found = anch::json::lexString(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(!found, "Result should not have value");
}
// String -

// Unsigned integer +
void
testParseUint() {
  std::istringstream sis("123456789");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::juint value;
  bool found = anch::json::lexUInteger(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", static_cast<anch::json::juint>(123456789), value),
		   "Result should parse '123456789'");
}

void
testParseUintNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::juint value;
  bool found = anch::json::lexUInteger(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(!found, "Result should not have value");
}

void
testParseUintSKO() {
  std::istringstream sis("-123456789");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::juint value;
  try {
    anch::json::lexUInteger(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() == ErrorCode::INVALID_FORMAT) {
      std::cout << e.what() << std::endl;
    } else {
      throw;
    }
  }
}

void
testParseUint0KO() {
  std::istringstream sis("0123456789");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::juint value;
  try {
    anch::json::lexUInteger(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() == ErrorCode::INVALID_FORMAT) {
      std::cout << e.what() << std::endl;
    } else {
      throw;
    }
  }
}
// Unsigned integer -

// Signed integer +
void
testParseInt() {
  std::istringstream sis("-123456789");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::jint value;
  bool found = anch::json::lexInteger(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", static_cast<anch::json::jint>(-123456789), value),
		   "Result should parse '-123456789'");
}

void
testParseIntNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::jint value;
  bool found = anch::json::lexInteger(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(!found, "Result should not have value");
}

void
testParseInt0KO() {
  std::istringstream sis("0123456789");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::jint value;
  try {
    anch::json::lexInteger(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() == ErrorCode::INVALID_FORMAT) {
      std::cout << e.what() << std::endl;
    } else {
      throw;
    }
  }
}
// Signed integer -

// Double +
void
testParseUDouble() {
  std::istringstream sis("123456789");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(123456789), value),
		   "Result should parse '123456789'");
}

void
testParseUDoubleDot() {
  std::istringstream sis("123456789.123456");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(123456789.123456), value),
		   "Result should parse '123456789.123456'");
}

void
testParseUDoubleZeroDot() {
  std::istringstream sis("0.123456");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(0.123456), value),
		   "Result should parse '0.123456'");
}

void
testParseSDouble() {
  std::istringstream sis("-123456789");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(-123456789), value),
		   "Result should parse '-123456789'");
}

void
testParseSDoubleDot() {
  std::istringstream sis("-123456789.123456");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(-123456789.123456), value),
		   "Result should parse '-123456789.123456'");
}

void
testParseSDoubleZeroDot() {
  std::istringstream sis("-0.123456");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(found, "Result should have value");
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(-0.123456), value),
		   "Result should parse '-0.123456'");
}

void
testParseDoubleNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(!found, "Result should not have value");
}

void
testParseUDoubleDotExp() {
  std::istringstream sis("1.2e+123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(1.2e+123), value),
		   "Result should parse '1.2e+123'");
}

void
testParseSDoubleDotExp() {
  std::istringstream sis("-1.2e+123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(-1.2e+123), value),
		   "Result should parse '-1.2e+123'");
}

void
testParseUDoubleExp() {
  std::istringstream sis("5e+123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(5e+123), value),
		   "Result should parse '5e+123'");
}

void
testParseSDoubleExp() {
  std::istringstream sis("-5e+123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assert(anch::ut::equals("lex", static_cast<double>(-5e+123), value),
		   "Result should parse '-5e+123'");
}
// Double -

// Array +
#include <list>
#include <set>
#include <vector>

void
testArrayNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::list<anch::json::jint> value;
  anch::json::AddItem add = [&](anch::json::ReaderContext& ctxt) {
    anch::json::jint val;
    if(anch::json::lexInteger(val, ctxt)) {
      value.push_back(val);
      return true;
    }
    return false;
  };
  anch::json::lexArray(add, context);
  anch::ut::assert(value.empty(), "Result should not have value");
}

void
testArrayEmpty() {
  std::istringstream sis("[]");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::list<anch::json::jint> value;
  anch::json::AddItem add = [&](anch::json::ReaderContext& ctxt) {
    anch::json::jint val;
    if(anch::json::lexInteger(val, ctxt)) {
      value.push_back(val);
      return true;
    }
    return false;
  };
  anch::json::lexArray(add, context);
  anch::ut::assert(value.empty(), "Result should not have value");
}

void
testArrayList() {
  std::istringstream sis("[1,2,3,4]");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::list<anch::json::jint> value;
  anch::json::AddItem add = [&](anch::json::ReaderContext& ctxt) {
    anch::json::jint val;
    if(anch::json::lexInteger(val, ctxt)) {
      value.push_back(val);
      return true;
    }
    return false;
  };
  anch::json::lexArray(add, context);
  std::list<anch::json::jint> expected = {1,2,3,4};
  anch::ut::assert(anch::ut::equals("lex", expected, value), "Result should parse '{1,2,3,4}'");
}

void
testArrayVector() {
  std::istringstream sis("[1,2,3,4]");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::vector<anch::json::jint> value;
  anch::json::AddItem add = [&](anch::json::ReaderContext& ctxt) {
    anch::json::jint val;
    if(anch::json::lexInteger(val, ctxt)) {
      value.push_back(val);
      return true;
    }
    return false;
  };
  anch::json::lexArray(add, context);
  std::vector<anch::json::jint> expected = {1,2,3,4};
  anch::ut::assert(anch::ut::equals("lex", expected, value), "Result should parse '{1,2,3,4}'");
}

void
testArraySet() {
  std::istringstream sis("[1,2,3,4]");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::set<anch::json::jint> value;
  anch::json::AddItem add = [&](anch::json::ReaderContext& ctxt) {
    anch::json::jint val;
    if(anch::json::lexInteger(val, ctxt)) {
      value.insert(val);
      return true;
    }
    return false;
  };
  anch::json::lexArray(add, context);
  std::set<anch::json::jint> expected = {1,2,3,4};
  anch::ut::assert(anch::ut::equals("lex", expected, value), "Result should parse '{1,2,3,4}'");
}
// Array -

// Map +
// Map -

// Object +
// Object -

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH JSON mapper unit tests")
    .description("Test AnCH JSON mapper library")
    // .initialize(beforeAll)
    // .uninitialize(afterAll)
    // Boolean +
    .add("lexer-bool-false", testParseBooleanFalse)
    .add("lexer-bool-true", testParseBooleanTrue)
    .add("lexer-bool-null", testParseBooleanNull)
    .add("lexer-bool-ko", testParseBooleanKO)
    // Boolean -
    // String +
    .add("lexer-str", testParseStr)
    .add("lexer-str-null", testParseStrNull)
    // String -
    // Unsigned integer +
    .add("lexer-uint", testParseUint)
    .add("lexer-uint-null", testParseUintNull)
    .add("lexer-uint-s-ko", testParseUintSKO)
    .add("lexer-uint-0-ko", testParseUint0KO)
    // Unsigned integer -
    // Signed integer +
    .add("lexer-int", testParseInt)
    .add("lexer-int-null", testParseIntNull)
    .add("lexer-int-0-ko", testParseInt0KO)
    // Signed integer -
    // Double +
    .add("lexer-double-u", testParseUDouble)
    .add("lexer-double-u-dot", testParseUDoubleDot)
    .add("lexer-double-u-0-dot", testParseUDoubleDot)
    .add("lexer-double-s", testParseSDouble)
    .add("lexer-double-s-dot", testParseSDoubleDot)
    .add("lexer-double-s-0-dot", testParseSDoubleDot)
    .add("lexer-double-null", testParseDoubleNull)
    .add("lexer-double-u-exp", testParseUDoubleExp)
    .add("lexer-double-s-exp", testParseSDoubleExp)
    .add("lexer-double-u-dot-exp", testParseUDoubleDotExp)
    .add("lexer-double-s-dot-exp", testParseSDoubleDotExp)
    // Double -
    // Array +
    .add("lexer-array-null", testArrayNull)
    .add("lexer-array-empty", testArrayEmpty)
    .add("lexer-array-list", testArrayList)
    .add("lexer-array-vector", testArrayVector)
    .add("lexer-array-set", testArraySet)
    // Array -
    // Map +
    // Map -
    // Object +
    // Object -
    ;
}
