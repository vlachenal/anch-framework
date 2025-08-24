#include "cutils/iostream.hpp"
#include "ut/unit.hpp"

#include <iostream>

#include "json/json.hpp"
#include "ut/assert.hpp"

using anch::cutils::CStreambuf;
using anch::cutils::CIStream;
using anch::cutils::COStream;
using anch::cutils::CIOStream;

struct Test {
  std::string toto;
  int titi;
};

template<>
void
anch::json::registerObject<Test>(anch::json::ObjectMapper<Test>& mapper) {
  mapper
    .registerField("toto", &Test::toto)
    .registerField("titi", &Test::titi);
}

std::size_t
printBuffer(char* data, std::size_t size) {
  std::cout << std::string(data, size);
  return size;
}

void
serialize(std::size_t size) {
  std::cout << "serialize" << size << std::endl;
  Test plop = {
    .toto = "toto",
    .titi = 42
  };
  { // in block for flush before std::endl;
    COStream cos({.data = NULL, .size = size, .write = &printBuffer});
    anch::json::serialize(plop, cos);
  }
  std::cout << std::endl;
}

std::string json("{\"toto\":\"toto\",\"titi\":42}"); // 25 characters
std::size_t offset = 0;

std::size_t
readBuffer(char* data, std::size_t size) {
  if(offset > json.length()) {
    return 0;
  }
  if(size >= json.length()) {
    ::strncpy(data, json.data(), json.length());
    return json.length();
  }
  ::strncpy(data, json.substr(offset, offset + size).data(), size);
  offset += size;
  return size;
}

void
deserialize(std::size_t size) {
  std::cout << "deserialize" << size << std::endl;
  offset = 0;
  CIStream cis({.data = NULL, .size = size, .read = &readBuffer});
  Test plop;
  anch::json::deserialize(plop, cis);
  std::cout << "plop.toto=" << plop.toto << ", plop.titi=" << plop.titi << std::endl;
}

void
serdser(std::size_t size) {
  std::cout << "serdser" << size << std::endl;
  Test plop = {
    .toto = "toto",
    .titi = 42
  };
  { // in block for flush before std::endl;
    CIOStream cios({.data = NULL, .size = size, .read = &readBuffer, .write = &printBuffer});
    anch::json::serialize(plop, cios);
    cios.flush();
    std::cout << std::endl;
    Test plip;
    anch::json::deserialize(plip, cios);
    std::cout << "plip.toto=" << plip.toto << ", plip.titi=" << plip.titi;
  }
  std::cout << std::endl;
}


void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH CStream unit tests")
    .description("Test AnCH CStream unit tests library")
    .add("cstream-output-jsonser", std::bind(&serialize, 4))
    .add("cstream-output-jsondeser", std::bind(&deserialize, 4))
    .add("cstream-output-jsonser-1char", std::bind(&serialize, 1))
    .add("cstream-output-jsondeser-1char", std::bind(&deserialize, 1))
    .add("cstream-output-jsonser-1500char", std::bind(&serialize, 1500))
    .add("cstream-output-jsondeser-1500char", std::bind(&deserialize, 1500))
    .add("cstream-output-jsonser-25char", std::bind(&serialize, 25))
    .add("cstream-output-jsondeser-25char", std::bind(&deserialize, 25))
    .add("cstream-inout-json-1500char", std::bind(&serdser, 1500))
    .add("cstream-inout-json-1char", std::bind(&serdser, 1))
    .add("cstream-inout-json-4char", std::bind(&serdser, 4))
    ;
}
