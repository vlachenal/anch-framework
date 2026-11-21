/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2020 Vincent Lachenal

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
#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <typeinfo>

#include "convert.hpp"

#include "ut/unit.hpp"
#include "ut/assert.hpp"

// Check that str is converted into expected value
template<typename N>
void
checkValue(std::string_view str, N expected) {
  try {
    N val = anch::convert<N>(str);
    anch::ut::assertThat(val == expected, "'{}' has been converted into {} instead of {}", str, +val, +expected);
  } catch(const std::bad_cast&) {
    anch::ut::fail("Unable to convert '" + std::string(str) + "'");
  }
}

// Check that str conversion fails
template<typename N>
void
checkInvalid(std::string_view str) {
  try {
    N val = anch::convert<N>(str);
    anch::ut::fail("'" + std::string(str) + "' has been converted into " + std::to_string(+val));
  } catch(const std::bad_cast&) {
    // expected
  }
}

void
testUnsigned() {
  checkValue<uint16_t>("32", 32);
  checkValue<uint16_t>("0", 0);
  checkValue<uint16_t>("65535", 65535);
  checkValue<uint64_t>("18446744073709551615", std::numeric_limits<uint64_t>::max());
}

void
testUnsignedNegative() {
  checkInvalid<uint16_t>("-32");
  checkInvalid<uint32_t>("-1");
  checkInvalid<uint64_t>("-1");
}

void
testSigned() {
  checkValue<int16_t>("32", 32);
  checkValue<int16_t>("-32", -32);
  checkValue<int64_t>("9223372036854775807", std::numeric_limits<int64_t>::max());
  checkValue<int64_t>("-9223372036854775808", std::numeric_limits<int64_t>::min());
}

void
testOutOfRange() {
  checkInvalid<uint16_t>("65536");
  checkInvalid<int16_t>("-32769");
  checkInvalid<int64_t>("9223372036854775808");
  checkInvalid<uint64_t>("18446744073709551616");
  checkInvalid<double>("1e400");
}

void
testInt8() {
  checkValue<uint8_t>("7", 7);
  checkValue<uint8_t>("255", 255);
  checkValue<int8_t>("-7", -7);
  checkInvalid<uint8_t>("256");
}

void
testDecimalAsInteger() {
  checkInvalid<uint16_t>("32.2");
  checkInvalid<uint16_t>("32e-1");
  checkInvalid<int16_t>("32.2");
}

void
testFloat() {
  checkValue<float>("32.2", 32.2f);
  checkValue<float>("32e-1", 3.2f);
  checkValue<double>("-0.5", -0.5);
  checkValue<double>("inf", std::numeric_limits<double>::infinity());
  try {
    anch::ut::assertTrue(std::isnan(anch::convert<double>("nan")), "'nan' has not been converted into NaN");
  } catch(const std::bad_cast&) {
    anch::ut::fail("Unable to convert 'nan'");
  }
}

void
testMalformed() {
  checkInvalid<int32_t>("");
  checkInvalid<int32_t>(" 1");
  checkInvalid<int32_t>("1 ");
  checkInvalid<int32_t>("+5");
  checkInvalid<int32_t>("1a");
  checkInvalid<int32_t>("0x10");
  checkInvalid<int32_t>("-");
  checkInvalid<double>("1,5");
}

void
testStringTypes() {
  std::string str("42");
  checkValue<int32_t>(str, 42);
  checkValue<int32_t>("42", 42);
  // Only the view has to be converted, not the whole buffer
  std::string_view view = std::string_view("1234").substr(0, 2);
  checkValue<int32_t>(view, 12);
}

void
testTryConvert() {
  std::expected<uint16_t, std::errc> ok = anch::tryConvert<uint16_t>("65535");
  anch::ut::assertThat(ok.has_value() && *ok == 65535, "65535 should be converted");
  std::expected<double, std::errc> dbl = anch::tryConvert<double>("-1.5e3");
  anch::ut::assertThat(dbl.has_value() && *dbl == -1500.0, "-1.5e3 should be converted");
  for(const char* str : {"", " 1", "1 ", "+1", "-1", "1a", "0x10", "1.5", "abc"}) {
    std::expected<uint16_t, std::errc> res = anch::tryConvert<uint16_t>(str);
    anch::ut::assertThat(!res.has_value() && res.error() == std::errc::invalid_argument, "'{}' should be an invalid argument", str);
  }
  for(const char* str : {"65536", "99999999999999999999999"}) {
    std::expected<uint16_t, std::errc> res = anch::tryConvert<uint16_t>(str);
    anch::ut::assertThat(!res.has_value() && res.error() == std::errc::result_out_of_range, "'{}' should be out of range", str);
  }
  std::expected<int8_t, std::errc> neg = anch::tryConvert<int8_t>("-129");
  anch::ut::assertThat(!neg.has_value() && neg.error() == std::errc::result_out_of_range, "-129 should be out of range");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH convert unit tests")
    .description("Test AnCH convert utiliy class")
    .add("conv-unsigned", testUnsigned)
    .add("conv-unsigned-negative", testUnsignedNegative)
    .add("conv-signed", testSigned)
    .add("conv-out-of-range", testOutOfRange)
    .add("conv-int8", testInt8)
    .add("conv-decimal-as-integer", testDecimalAsInteger)
    .add("conv-float", testFloat)
    .add("conv-malformed", testMalformed)
    .add("conv-string-types", testStringTypes)
    .add("conv-try-convert", testTryConvert)
    ;
}
