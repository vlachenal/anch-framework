/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2013 Vincent Lachenal

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
#include "crypto/b2t/base64.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <typeinfo>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::Base64;

void
testEncEmpty() {
  std::cout << "Encode empty string" << std::endl;
  std::string res = Base64::encode(std::string(""));
  anch::ut::assertTrue(res == "");
  std::cout << "Found " << res << std::endl;
}

void
testEncF() {
  std::cout << "Encode f" << std::endl;
  std::string res = Base64::encode(std::string("f"));
  anch::ut::assertTrue(res == "Zg==");
  std::cout << "Found " << res << std::endl;
}

void
testEncFo() {
  std::cout << "Encode fo" << std::endl;
  std::string res = Base64::encode(std::string("fo"));
  anch::ut::assertTrue(res == "Zm8=");
  std::cout << "Found " << res << std::endl;
}

void
testEncFoo() {
  std::cout << "Encode foo" << std::endl;
  std::string res = Base64::encode(std::string("foo"));
  anch::ut::assertTrue(res == "Zm9v");
  std::cout << "Found " << res << std::endl;
}

void
testEncFoob() {
  std::cout << "Encode foob" << std::endl;
  std::string res = Base64::encode(std::string("foob"));
  anch::ut::assertTrue(res == "Zm9vYg==");
  std::cout << "Found " << res << std::endl;
}

void
testEncFooba() {
  std::cout << "Encode fooba" << std::endl;
  std::string res = Base64::encode(std::string("fooba"));
  anch::ut::assertTrue(res == "Zm9vYmE=");
  std::cout << "Found " << res << std::endl;
}

void
testEncFoobar() {
  std::cout << "Encode foobar" << std::endl;
  std::string res = Base64::encode(std::string("foobar"));
  anch::ut::assertTrue(res == "Zm9vYmFy");
  std::cout << "Found " << res << std::endl;
}

void
testDecEmpty() {
  std::cout << "Decode empty string" << std::endl;
  std::string res = Base64::decode(std::string(""));
  anch::ut::assertTrue(res == "");
  std::cout << "Found " << res << std::endl;
}

void
testDecF() {
  std::cout << "Decode Zg==" << std::endl;
  std::string res = Base64::decode(std::string("Zg=="));
  anch::ut::assertTrue(res == "f");
  std::cout << "Found " << res << std::endl;
}

void
testDecFo() {
  std::cout << "Decode Zm8=" << std::endl;
  std::string res = Base64::decode(std::string("Zm8="));
  anch::ut::assertTrue(res == "fo");
  std::cout << "Found " << res << std::endl;
}

void
testDecFoo() {
  std::cout << "Decode Zm9v" << std::endl;
  std::string res = Base64::decode(std::string("Zm9v"));
  anch::ut::assertTrue(res == "foo");
  std::cout << "Found " << res << std::endl;
}

void
testDecFoob() {
  std::cout << "Decode Zm9vYg==" << std::endl;
  std::string res = Base64::decode(std::string("Zm9vYg=="));
  anch::ut::assertTrue(res == "foob");
  std::cout << "Found " << res << std::endl;
}

void
testDecFooba() {
  std::cout << "Decode Zm9vYmE=" << std::endl;
  std::string res = Base64::decode(std::string("Zm9vYmE="));
  anch::ut::assertTrue(res == "fooba");
  std::cout << "Found " << res << std::endl;
}

void
testDecFoobar() {
  std::cout << "Decode Zm9vYmFy" << std::endl;
  std::string res = Base64::decode(std::string("Zm9vYmFy"));
  anch::ut::assertTrue(res == "foobar");
  std::cout << "Found " << res << std::endl;
}

void
testEncMakefile() {
  std::cout << "Encode Makefile" << std::endl;
  std::ifstream file("Makefile");
  Base64::encode(file, std::cout);
  std::cout << std::endl;
  std::cout << "You can check result manually using base64 command" << std::endl;
  file.close();
}

void
testDecBinary() {
  std::cout << "Decode binary data with null bytes (C7)" << std::endl;
  const std::string data("\x00\x01\x02\x41\x00\x42", 6);
  std::string enc = Base64::encode(data);
  anch::ut::assertThat(enc == "AAECQQBC", "Unexpected encoding: {}", enc);
  std::string dec = Base64::decode(enc);
  anch::ut::assertThat(dec == data, "Decoded {} bytes instead of 6", dec.size());
  std::string all;
  for(int i = 0 ; i < 256 ; ++i) {
    all.push_back(static_cast<char>(i));
  }
  anch::ut::assertTrue(Base64::decode(Base64::encode(all)) == all, "256 bytes round trip failed");
}

void
testNoPadding() {
  std::cout << "Round trip without padding (C8)" << std::endl;
  std::string data;
  for(std::size_t len = 0 ; len < 32 ; ++len) {
    for(uint8_t opts : {static_cast<uint8_t>(Base64::URL | Base64::NOPADDING), static_cast<uint8_t>(Base64::STD | Base64::NOPADDING)}) {
      std::string enc = Base64::encode(data, opts);
      anch::ut::assertThat(enc.find('=') == std::string::npos, "Padding found in {}", enc);
      // exact size buffer so that ASan detects any read past the data
      std::unique_ptr<char[]> buffer(new char[enc.size()]);
      std::copy(enc.begin(), enc.end(), buffer.get());
      std::string dec = Base64::decode(buffer.get(), enc.size(), opts);
      anch::ut::assertThat(dec == data, "Round trip failed for length {}: {}", len, enc);
    }
    data.push_back(static_cast<char>(0xF0 + len % 16));
  }
  anch::ut::assertThat(Base64::decode(std::string("QUI"), Base64::URL | Base64::NOPADDING) == "AB", "QUI is not decoded as AB");
}

void
testDecInvalid() {
  std::cout << "Decode invalid data" << std::endl;
  for(const char* invalid : {"Q", "QUJDR", "QUJD====", "QU*D"}) {
    bool thrown = false;
    try {
      Base64::decode(std::string(invalid));
    } catch(const std::bad_cast&) {
      thrown = true;
    }
    anch::ut::assertThat(thrown, "{} has been decoded", invalid);
  }
}

void
testStream() {
  std::cout << "Encode and decode streams larger than read buffer" << std::endl;
  std::string data;
  for(std::size_t i = 0 ; i < 5000 ; ++i) {
    data.push_back(static_cast<char>((i * 7) % 256));
  }
  std::istringstream in(data);
  std::string enc = Base64::encode(in);
  anch::ut::assertThat(enc == Base64::encode(data), "Stream encoding differs from string encoding ({} / {} characters)",
		       enc.size(), Base64::encode(data).size());
  std::istringstream encIn(enc);
  anch::ut::assertTrue(Base64::decode(encIn) == data, "Stream round trip failed");
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH base64 unit tests")
    .description("Test AnCH base64 library")
    .add("base64-enc-empty", testEncEmpty)
    .add("base64-enc-f", testEncF)
    .add("base64-enc-fo", testEncFo)
    .add("base64-enc-foo", testEncFoo)
    .add("base64-enc-foob", testEncFoob)
    .add("base64-enc-fooba", testEncFooba)
    .add("base64-enc-foobar", testEncFoobar)
    .add("base64-dec-empty", testDecEmpty)
    .add("base64-dec-f", testDecF)
    .add("base64-dec-fo", testDecFo)
    .add("base64-dec-foo", testDecFoo)
    .add("base64-dec-foob", testDecFoob)
    .add("base64-dec-fooba", testDecFooba)
    .add("base64-dec-foobar", testDecFoobar)
    .add("base64-enc-makefile", testEncMakefile)
    .add("base64-dec-binary", testDecBinary)
    .add("base64-nopadding", testNoPadding)
    .add("base64-dec-invalid", testDecInvalid)
    .add("base64-stream", testStream)
    ;
}
