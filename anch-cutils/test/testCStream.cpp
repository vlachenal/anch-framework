#include "cutils/iostream.hpp"

#include <iostream>

#include "json/json.hpp"

using anch::cutils::CStreambuf;
using anch::cutils::CIStream;
using anch::cutils::COStream;

struct Test {
  std::string toto;
  int titi;
};

template<>
void
anch::json::registerObject<Test>(anch::json::ObjectMapper<Test>& mapper) {
  mapper.registerField("toto", &Test::toto).registerField("titi", &Test::titi);
}

std::size_t
printBuffer(char* data, std::size_t bufferSize) {
  std::cout << std::string(data, bufferSize);
  return bufferSize;
}

void
serialize() {
  Test plop = {
    .toto = "toto",
    .titi = 42
  };
  { // in block for flush before std::endl;
    COStream cos({.data = NULL, .size = 4, .handle = &printBuffer});
    anch::json::serialize(plop, cos);
  }
  std::cout << std::endl;
}

std::size_t
readBuffer(char* data, std::size_t size) {
  static std::string json("{\"toto\":\"toto\",\"titi\":42}");
  static std::size_t offset = 0;
  if(offset + size > json.length()) {
    ::strncpy(data, json.substr(offset).data(), json.length() - offset);
    return json.length() - size - offset;
  } else {
    ::strncpy(data, json.substr(offset, offset + size).data(), size);
    offset += size;
    return size;
  }
}

void
deserialize() {
  CIStream cis({.data = NULL, .size = 4, .handle = &readBuffer});
  Test plop;
  anch::json::deserialize(plop, cis);
  std::cout << "plop.toto=" << plop.toto << ", plop.titi=" << plop.titi << std::endl;
}

int
main(int argc, char* argv[]) {
  std::map<std::string, std::function<void(void)>> tests = {
    {"serialize", std::function<void(void)>(&serialize)},
    {"deserialize", std::function<void(void)>(&deserialize)}
  };
  if(argc > 1) {
    auto iter = tests.find(argv[1]);
    if(iter == tests.end()) {
      std::cerr << "Test " << argv[1] << " not found" << std::endl;
      return 1;
    }
    iter->second();
  } else {
    for(auto iter = tests.cbegin() ; iter != tests.cend() ; ++iter) {
      iter->second();
    }
  }
  return 0;
}
