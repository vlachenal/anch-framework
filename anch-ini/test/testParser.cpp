#include <iostream>

#include "ini/parser.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::ini::ParserError;


anch::ini::Section section;

void
startup() {
  std::cout << "Parse file" << std::endl;
  std::string file = "test.ini";
  section = anch::ini::parse(file);
  std::cout << section.debug() << std::endl;
  std::cout << "File has been parsed" << std::endl;
}

void
getValFromRoot() {
  std::cout << "Execute getValFromRoot" << std::endl;
  auto val = section.getValue<std::string>("toto");
  anch::ut::assertTrue(val.has_value(), "toto not found");
  std::cout << "toto=" << val.value() << std::endl;
}

void
getValInSub() {
  std::cout << "Execute getValInSub" << std::endl;
  anch::ut::assertTrue(section.getSections().contains("TOTO"));
  auto val = section.getValue<std::string>("TOTO.toto");
  anch::ut::assertTrue(val.has_value(), "toto not found");
  std::cout << "TOTO/toto=" << val.value() << std::endl;
}

void
getValSubInSub() {
  std::cout << "Execute getValSubInSub" << std::endl;
  anch::ut::assertTrue(section.getSections().contains("TOTO"));
  auto sec = section.getSections().find("TOTO")->second;
  anch::ut::assertTrue(sec.getSections().contains("tata"));
  auto val = section.getValue<std::string>("TOTO.tata.tyty");
  anch::ut::assertTrue(val.has_value(), "tyty not found");
  std::cout << "TOTO/tata/tyty=" << val.value() << std::endl;
}

void
getValInSubSub() {
  std::cout << "Execute getPlopPlipPlup" << std::endl;
  anch::ut::assertTrue(section.getSections().contains("plop"));
  auto sec = section.getSections().find("plop")->second;
  anch::ut::assertTrue(sec.getSections().contains("plip"));
  auto val = section.getValue<std::string>("plop.plip.plup");
  anch::ut::assertTrue(val.has_value(), "plup not found");
  std::cout << "plop/plip/plup=" << val.value() << std::endl;
}

void
getInt16() {
  std::cout << "Execute getInt16" << std::endl;
  auto val = section.getValue<int16_t>("conv.int16");
  anch::ut::assertTrue(val.has_value(), "conv/int16 not found");
  std::cout << "conv/int16=" << val.value() << std::endl;
}

void
getInt32() {
  std::cout << "Execute getInt32" << std::endl;
  auto val = section.getValue<int32_t>("conv.int32");
  anch::ut::assertTrue(val.has_value(), "conv/int32 not found");
  std::cout << "conv/int32=" << val.value() << std::endl;
}

void
getInt64() {
  std::cout << "Execute getInt64" << std::endl;
  auto val = section.getValue<int64_t>("conv.int64");
  anch::ut::assertTrue(val.has_value(), "conv/int64 not found");
  std::cout << "conv/int64=" << val.value() << std::endl;
}

void
getUInt16() {
  std::cout << "Execute getUInt16" << std::endl;
  auto val = section.getValue<int16_t>("conv.uint16");
  anch::ut::assertTrue(val.has_value(), "conv/uint16 not found");
  std::cout << "conv/uint16=" << val.value() << std::endl;
}

void
getUInt32() {
  std::cout << "Execute getUInt32" << std::endl;
  auto val = section.getValue<int32_t>("conv.uint32");
  anch::ut::assertTrue(val.has_value(), "conv/uint32 not found");
  std::cout << "conv/uint32=" << val.value() << std::endl;
}

void
getUInt64() {
  std::cout << "Execute getUInt64" << std::endl;
  auto val = section.getValue<int64_t>("conv.uint64");
  anch::ut::assertTrue(val.has_value(), "conv/uint64 not found");
  std::cout << "conv/uint64=" << val.value() << std::endl;
}

void
getFloat() {
  std::cout << "Execute getFloat" << std::endl;
  auto val = section.getValue<float>("conv.float");
  anch::ut::assertTrue(val.has_value(), "conv/float not found");
  anch::ut::assert(anch::ut::equals("val", (float)123.456, val.value()), "Value {} should be equals to 123.456", val.value());
  std::cout << "conv/float=" << val.value() << std::endl;
}

void
getDouble() {
  std::cout << "Execute getDouble" << std::endl;
  auto val = section.getValue<double>("conv.double");
  anch::ut::assertTrue(val.has_value(), "conv/double not found");
  anch::ut::assert(anch::ut::equals("val", 123.4567890123, val.value()), "Value {} should be equals to 123.4567890123", val.value());
  std::cout << "conv/double=" << val.value() << std::endl;
}

void
getBool() {
  std::cout << "Execute getBool" << std::endl;
  auto val = section.getValue<bool>("conv.bool");
  anch::ut::assertTrue(val.has_value(), "conv/bool not found");
  std::cout << "conv/bool=" << val.value() << std::endl;
}


void
getNumFailRange() {
  std::cout << "Execute getNumFailRange" << std::endl;
  try {
    section.getValue<int16_t>("conv.int64");
    anch::ut::fail("This test should have failed");
  } catch(const ParserError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assert(e.getErrorCode() == ParserError::ErrorCode::BAD_CAST, "Error code should be BAD_CAST");
  }
}

void
getNumFailInvalid() {
  std::cout << "Execute getNumFailInvalid" << std::endl;
  try {
    section.getValue<uint16_t>("conv.int16");
    anch::ut::fail("This test should have failed");
  } catch(const ParserError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assert(e.getErrorCode() == ParserError::ErrorCode::BAD_CAST, "Error code should be BAD_CAST");
  }
}

void
getNumFailIncomplete() {
  std::cout << "Execute getNumFailIncomplete" << std::endl;
  try {
    section.getValue<uint16_t>("conv.double");
    anch::ut::fail("This test should have failed");
  } catch(const ParserError& e) {
    std::cout << e.what() << std::endl;
    anch::ut::assert(e.getErrorCode() == ParserError::ErrorCode::BAD_CAST, "Error code should be BAD_CAST");
  }
}


void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH INI unit tests")
    .description("Test AnCH INI library")
    .initialize(startup)
    .add("parser-root-val", getValFromRoot)
    .add("parser-root-sub-val", getValInSub)
    .add("parser-root-sub-keysub-val", getValSubInSub)
    .add("parser-root-subsub-val", getValInSubSub)
    .add("parser-int16", getInt16)
    .add("parser-int32", getInt32)
    .add("parser-int64", getInt64)
    .add("parser-uint16", getUInt16)
    .add("parser-uint32", getUInt32)
    .add("parser-uint64", getUInt64)
    .add("parser-float", getFloat)
    .add("parser-double", getDouble)
    .add("parser-bool", getBool)
    .add("parser-num-range", getNumFailRange)
    .add("parser-num-invalid", getNumFailInvalid)
    .add("parser-num-incomplete", getNumFailIncomplete)
    ;
}
