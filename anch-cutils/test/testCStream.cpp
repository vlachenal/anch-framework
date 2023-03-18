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
    COStream cos({.data = NULL, .size = size, .handle = &printBuffer});
    anch::json::serialize(plop, cos);
  }
  std::cout << std::endl;
}

std::string json("{\"toto\":\"toto\",\"titi\":42}"); // 25 characters
std::size_t offset = 0;

std::size_t
readBuffer(char* data, std::size_t size) {
  if(size >= json.length()) {
    ::strncpy(data, json.data(), json.length());
    return json.length();
  } else if(offset + size == json.length() + 1) {
    return 0;
  } else if(offset + size > json.length() + 1) {
    ::strncpy(data, json.substr(offset).data(), json.length() - offset);
    return json.length() - size - offset;
  } else {
    ::strncpy(data, json.substr(offset, offset + size).data(), size);
    offset += size;
    return size;
  }
}

void
deserialize(std::size_t size) {
  std::cout << "deserialize" << size << std::endl;
  offset = 0;
  CIStream cis({.data = NULL, .size = size, .handle = &readBuffer});
  Test plop;
  anch::json::deserialize(plop, cis);
  std::cout << "plop.toto=" << plop.toto << ", plop.titi=" << plop.titi << std::endl;
}

int
main(int argc, char* argv[]) {
  std::map<std::string, std::function<void(void)>> tests = {
    {"serialize", std::bind(&serialize, 4)},
    {"deserialize", std::bind(&deserialize, 4)},
    {"serialize1", std::bind(&serialize, 1)},
    {"deserialize1", std::bind(&deserialize, 1)},
    {"serialize1500", std::bind(&serialize, 1500)},
    {"deserialize1500", std::bind(&deserialize, 1500)},
    {"serialize25", std::bind(&serialize, 25)},
    {"deserialize25", std::bind(&deserialize, 25)}
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