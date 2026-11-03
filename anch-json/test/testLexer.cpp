#include <iostream>
#include <sstream>
#include <chrono>
#include <limits>

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
  anch::ut::assertThat(anch::ut::equals("lex: ", false, value),
		       "Result should be false");
}

void
testParseBooleanTrue() {
  std::istringstream sis("true");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  bool value = false;
  anch::json::lexBoolean(value, context);
  std::cout << "Result: " << std::boolalpha << value << std::endl;
  anch::ut::assertThat(anch::ut::equals("lex: ", true, value),
		       "Result should be true");
}

void
testParseBooleanNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  bool value = true;
  anch::json::lexBoolean(value, context);
  std::cout << "Result: " << std::boolalpha << value << std::endl;
  anch::ut::assertThat(anch::ut::equals("lex: ", true, value),
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
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", std::string("test string"), value),
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
  anch::ut::assertThat(!found, "Result should not have value");
}

void
checkParseStr(const std::string& json, const std::string& expected, std::size_t bufferSize) {
  std::istringstream sis(json);
  anch::json::ReaderContext context(sis, {.buffer_size = bufferSize});
  std::string value;
  bool found = anch::json::lexString(value, context);
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(value == expected, "Parsing {} with buffer_size={}: expected '{}' ; found '{}'", json, bufferSize, expected, value);
}

void
checkParseStrKO(const std::string& json) {
  std::cout << "Parse " << json << std::endl;
  std::istringstream sis(json);
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::string value;
  try {
    anch::json::lexString(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::INVALID_FORMAT) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}

void
testParseStrEscapes() {
  checkParseStr("\"a\\\"b\\\\c\\/d\\b\\f\\n\\r\\t\"", "a\"b\\c/d\b\f\n\r\t", 1024);
}

void
testParseStrUnicode() {
  checkParseStr("\"caf\\u00e9 \\u20AC \\u0041\"", "caf\xC3\xA9 \xE2\x82\xAC A", 1024);
}

void
testParseStrSurrogate() {
  checkParseStr("\"\\uD83D\\uDE00\"", "\xF0\x9F\x98\x80", 1024);
}

void
testParseStrEscapeKO() {
  for(const char* json : {"\"\\x\"", "\"\\u12G4\"", "\"\\uD83D\"", "\"\\uD83Dx\"", "\"\\uD83D\\u0041\"", "\"\\uDE00\"", "\"abc\\"}) {
    checkParseStrKO(json);
  }
}

void
testParseStrUnterminatedKO() {
  checkParseStrKO("\"abc");
}

void
testParseStrBuffer() {
  // Strings, escape and unicode sequences across buffer boundaries
  for(std::size_t bufferSize = 1 ; bufferSize <= 12 ; ++bufferSize) {
    checkParseStr("\"0123456789\\n\\u00e9abc\\uD83D\\uDE00defgh\"", "0123456789\n\xC3\xA9" "abc\xF0\x9F\x98\x80" "defgh", bufferSize);
  }
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
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<anch::json::juint>(123456789), value),
		       "Result should parse '123456789'");
}

void
testParseUintNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::juint value;
  bool found = anch::json::lexUInteger(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(!found, "Result should not have value");
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
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<anch::json::jint>(-123456789), value),
		       "Result should parse '-123456789'");
}

void
testParseIntNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::jint value;
  bool found = anch::json::lexInteger(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(!found, "Result should not have value");
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
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(123456789), value),
		       "Result should parse '123456789'");
}

void
testParseUDoubleDot() {
  std::istringstream sis("123456789.123456");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(123456789.123456), value),
		       "Result should parse '123456789.123456'");
}

void
testParseUDoubleZeroDot() {
  std::istringstream sis("0.123456");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(0.123456), value),
		       "Result should parse '0.123456'");
}

void
testParseSDouble() {
  std::istringstream sis("-123456789");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(-123456789), value),
		       "Result should parse '-123456789'");
}

void
testParseSDoubleDot() {
  std::istringstream sis("-123456789.123456");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(-123456789.123456), value),
		       "Result should parse '-123456789.123456'");
}

void
testParseSDoubleZeroDot() {
  std::istringstream sis("-0.123456");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(-0.123456), value),
		       "Result should parse '-0.123456'");
}

void
testParseDoubleNull() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(!found, "Result should not have value");
}

void
testParseUDoubleDotExp() {
  std::istringstream sis("1.2e+123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(1.2e+123), value),
		       "Result should parse '1.2e+123'");
}

void
testParseSDoubleDotExp() {
  std::istringstream sis("-1.2e+123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(-1.2e+123), value),
		       "Result should parse '-1.2e+123'");
}

void
testParseUDoubleExp() {
  std::istringstream sis("5e+123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(5e+123), value),
		       "Result should parse '5e+123'");
}

void
testParseSDoubleExp() {
  std::istringstream sis("-5e+123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  bool found = anch::json::lexNumber(value, context);
  std::cout << "Result has value ? " << std::boolalpha << found << std::endl;
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<double>(-5e+123), value),
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
  anch::ut::assertThat(value.empty(), "Result should not have value");
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
  anch::ut::assertThat(value.empty(), "Result should not have value");
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
  anch::ut::assertThat(anch::ut::equals("lex", expected, value), "Result should parse '{{1,2,3,4}}'");
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
  anch::ut::assertThat(anch::ut::equals("lex", expected, value), "Result should parse '{{1,2,3,4}}'");
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
  anch::ut::assertThat(anch::ut::equals("lex", expected, value), "Result should parse '{{1,2,3,4}}'");
}
// Array -

// Map +
#include <map>

void
testNullMap() {
  std::istringstream sis("null");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::map<std::string,anch::json::jint> value;
  anch::json::PushItem push = [&](const std::string& key, anch::json::ReaderContext& ctxt) {
    anch::json::jint val;
    if(anch::json::lexInteger(val, ctxt)) {
      value[key] = val;
    }
  };
  anch::json::lexMap(push, context);
  std::map<std::string,anch::json::jint> expected = {};
  anch::ut::assertThat(value.empty(), "Result should not have value");
}

void
testEmptyMap() {
  std::istringstream sis("{}");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::map<std::string,anch::json::jint> value;
  anch::json::PushItem push = [&](const std::string& key, anch::json::ReaderContext& ctxt) {
    anch::json::jint val;
    if(anch::json::lexInteger(val, ctxt)) {
      value[key] = val;
    }
  };
  anch::json::lexMap(push, context);
  std::map<std::string,anch::json::jint> expected = {};
  anch::ut::assertThat(value.empty(), "Result should not have value");
}

void
testMap() {
  std::istringstream sis("{\"plop\":1,\"plip\":2,\"plap\":3}");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  std::map<std::string,anch::json::jint> value;
  anch::json::PushItem push = [&value](const std::string& key, anch::json::ReaderContext& ctxt) {
    anch::json::jint val;
    if(anch::json::lexInteger(val, ctxt)) {
      value[key] = val;
    }
  };
  anch::json::lexMap(push, context);
  for(auto iter = value.cbegin() ; iter != value.cend() ; ++iter) {
    std::cout << iter->first << '=' << iter->second << std::endl;
  }
  std::map<std::string,anch::json::jint> expected = {
    {"plop", 1},
    {"plip", 2},
    {"plap", 3}
  };
  anch::ut::assertThat(expected.size() == value.size(), "Number of items should be {} ; found {}", expected.size(), value.size());
  for(auto iter = expected.cbegin() ; iter != expected.cend() ; ++iter) {
    anch::ut::assertThat(iter->second == value[iter->first], "{}={} differs from {}={}", iter->first, iter->second, iter->first, value[iter->first]);
  }
}
// Map -

// Zero +
void
testParseUint0() {
  std::istringstream sis("0,");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::juint value = 42;
  bool found = anch::json::lexUInteger(value, context);
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<anch::json::juint>(0), value),
		       "Result should parse '0'");
}

void
testParseInt0() {
  std::istringstream sis("0}");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::jint value = 42;
  bool found = anch::json::lexInteger(value, context);
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<anch::json::jint>(0), value),
		       "Result should parse '0'");
}

void
testParseIntMinus0() {
  std::istringstream sis("-0");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::jint value = 42;
  bool found = anch::json::lexInteger(value, context);
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", static_cast<anch::json::jint>(0), value),
		       "Result should parse '-0'");
}

void
testParseIntMinus0KO() {
  std::istringstream sis("-0123");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::jint value;
  try {
    anch::json::lexInteger(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::INVALID_FORMAT) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}

void
testParseDouble0() {
  std::istringstream sis("0,");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value = 42.0;
  bool found = anch::json::lexNumber(value, context);
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", 0.0, value), "Result should parse '0'");
}

void
testParseDouble0Exp() {
  std::istringstream sis("0e2");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value = 42.0;
  bool found = anch::json::lexNumber(value, context);
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", 0.0, value), "Result should parse '0e2'");
}

void
testParseDouble0KO() {
  std::istringstream sis("0123.4");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  double value;
  try {
    anch::json::lexNumber(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::INVALID_FORMAT) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}
// Zero -

// Overflow +
template<typename T>
void
checkOverflow(const std::string& input, bool (*lex)(T&, anch::json::ReaderContext&)) {
  std::istringstream sis(input);
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  T value;
  try {
    lex(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::POTENTIAL_OVERFLOW) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}

void
testParseUintOverflow() {
  checkOverflow<anch::json::juint>("18446744073709551616", anch::json::lexUInteger);
}

void
testParseIntOverflow() {
  checkOverflow<anch::json::jint>("9223372036854775808", anch::json::lexInteger);
}

void
testParseIntUnderflow() {
  checkOverflow<anch::json::jint>("-9223372036854775809", anch::json::lexInteger);
}

void
testParseDoubleOverflow() {
  checkOverflow<double>("1e999", anch::json::lexNumber);
}

void
testParseUintMax() {
  std::istringstream sis("18446744073709551615");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::juint value = 0;
  bool found = anch::json::lexUInteger(value, context);
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", std::numeric_limits<anch::json::juint>::max(), value),
		       "Result should parse '18446744073709551615'");
}

void
testParseIntMin() {
  std::istringstream sis("-9223372036854775808");
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  anch::json::jint value = 0;
  bool found = anch::json::lexInteger(value, context);
  anch::ut::assertThat(found, "Result should have value");
  anch::ut::assertThat(anch::ut::equals("lex", std::numeric_limits<anch::json::jint>::min(), value),
		       "Result should parse '-9223372036854775808'");
}
// Overflow -

// Number scanning +
template<typename T, typename F>
void
checkParseNumberBuffer(const std::string& number, T expected, F lex) {
  for(std::size_t bufferSize = 1 ; bufferSize <= 12 ; ++bufferSize) {
    for(std::size_t pad = 0 ; pad < 4 ; ++pad) {
      std::string json = std::string(pad, ' ') + number + ",";
      std::istringstream sis(json);
      anch::json::ReaderContext context(sis, {.deserialize_max_discard_char = -1, .buffer_size = bufferSize});
      T value{};
      bool found = lex(value, context);
      anch::ut::assertThat(found, "'{}' should have value", json);
      anch::ut::assertThat(value == expected, "Parsing '{}' with buffer_size={}: expected {} ; found {}", json, bufferSize, expected, value);
      char next = context.discard();
      anch::ut::assertThat(next == ',', "Parsing '{}' with buffer_size={}: next character should be ',' ; found '{}'", json, bufferSize, next);
    }
  }
}

void
testParseNumberBuffer() {
  checkParseNumberBuffer<anch::json::jint>("-1234567890123", -1234567890123, [](anch::json::jint& v, anch::json::ReaderContext& c) { return anch::json::lexInteger(v, c); });
  checkParseNumberBuffer<anch::json::juint>("18446744073709551615", std::numeric_limits<uint64_t>::max(), [](anch::json::juint& v, anch::json::ReaderContext& c) { return anch::json::lexUInteger(v, c); });
  checkParseNumberBuffer<double>("-12.5e-3", -12.5e-3, [](double& v, anch::json::ReaderContext& c) { return anch::json::lexNumber(v, c); });
  checkParseNumberBuffer<double>("0.5E+2", 0.5E+2, [](double& v, anch::json::ReaderContext& c) { return anch::json::lexNumber(v, c); });
}

template<typename T, typename F>
void
checkParseNumberKO(const char* json, F lex) {
  std::cout << "Parse " << json << std::endl;
  std::istringstream sis(json);
  anch::json::ReaderContext context(sis, anch::json::DEFAULT_MAPPING_OPTIONS);
  T value{};
  try {
    lex(value, context);
    anch::ut::fail("This test should have failed");
  } catch(const MappingError& e) {
    if(e.getErrorCode() != ErrorCode::INVALID_FORMAT) {
      throw;
    }
    std::cout << e.what() << std::endl;
  }
}

void
testParseDoubleFormatKO() {
  for(const char* json : {".5", "-.5", "1.", "1.e2", "1e", "1e+", "-", "1.5.3", "+1", "1-2", "01.5", "1e2.5"}) {
    checkParseNumberKO<double>(json, [](double& v, anch::json::ReaderContext& c) { return anch::json::lexNumber(v, c); });
  }
}

void
testParseIntFormatKO() {
  for(const char* json : {"-", "--1", "1-2", "-01"}) {
    checkParseNumberKO<anch::json::jint>(json, [](anch::json::jint& v, anch::json::ReaderContext& c) { return anch::json::lexInteger(v, c); });
  }
  for(const char* json : {"-1", "01"}) {
    checkParseNumberKO<anch::json::juint>(json, [](anch::json::juint& v, anch::json::ReaderContext& c) { return anch::json::lexUInteger(v, c); });
  }
}
// Number scanning -

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
    .add("lexer-str-escapes", testParseStrEscapes)
    .add("lexer-str-unicode", testParseStrUnicode)
    .add("lexer-str-surrogate", testParseStrSurrogate)
    .add("lexer-str-escape-ko", testParseStrEscapeKO)
    .add("lexer-str-unterminated-ko", testParseStrUnterminatedKO)
    .add("lexer-str-buffer", testParseStrBuffer)
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
    .add("lexer-map-null", testNullMap)
    .add("lexer-map-empty", testEmptyMap)
    .add("lexer-map", testMap)
    // Map -
    // Zero +
    .add("lexer-uint-0", testParseUint0)
    .add("lexer-int-0", testParseInt0)
    .add("lexer-int-minus-0", testParseIntMinus0)
    .add("lexer-int-minus-0-ko", testParseIntMinus0KO)
    .add("lexer-double-0", testParseDouble0)
    .add("lexer-double-0-exp", testParseDouble0Exp)
    .add("lexer-double-0-ko", testParseDouble0KO)
    // Zero -
    // Overflow +
    .add("lexer-uint-overflow", testParseUintOverflow)
    .add("lexer-int-overflow", testParseIntOverflow)
    .add("lexer-int-underflow", testParseIntUnderflow)
    .add("lexer-double-overflow", testParseDoubleOverflow)
    .add("lexer-uint-max", testParseUintMax)
    .add("lexer-int-min", testParseIntMin)
    // Overflow -
    // Number scanning +
    .add("lexer-number-buffer", testParseNumberBuffer)
    .add("lexer-double-format-ko", testParseDoubleFormatKO)
    .add("lexer-int-format-ko", testParseIntFormatKO)
    // Number scanning -
    // Object +
    // Object -
    ;
}
