#include "crypto/b2t/base64.hpp"

#include <fstream>

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
    ;
}
