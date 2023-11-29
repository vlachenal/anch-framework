#include <iostream>

#include "resource/resource.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"


using anch::resource::Resource;


const Resource* res;

void
startup() {
  std::cout << "Parse file" << std::endl;
  res = &Resource::getResource("test.ini");
  std::cout << "File has been parsed" << std::endl;
}

void
getToto() {
  std::cout << "Execute getToto" << std::endl;
  std::string value;
  bool found = res->getParameter(value,"toto");
  anch::ut::assertTrue(found, "toto not found");
  std::cout << "toto=" << value << std::endl;
}

void
getTOTOToto() {
  std::cout << "Execute getTOTOToto" << std::endl;
  std::string value;
  bool found = res->getParameter(value,"toto","TOTO");
  anch::ut::assertTrue(found, "toto not found");
  std::cout << "TOTO/toto=" << value << std::endl;
}

void
getTata() {
  std::cout << "Execute getTata" << std::endl;
  std::string value;
  bool found = res->getParameter(value,"tata");
  anch::ut::assertTrue(found, "tata not found");
  std::cout << "tata=" << value << std::endl;
}

void
getOptToto() {
  std::cout << "Execute getOptToto" << std::endl;
  std::optional<std::string> value = res->parameter("toto");
  anch::ut::assertTrue(value.has_value(), "toto not found");
  std::cout << "toto=" << *value << std::endl;
}

void
getOptTOTOToto() {
  std::cout << "Execute getOptTOTOToto" << std::endl;
  std::optional<std::string> value = res->section("TOTO")->parameter("toto");
  anch::ut::assertTrue(value.has_value(), "TOTO/toto has not been found");
  std::cout << "TOTO/toto=" << *value << std::endl;
}

void
getOptTata() {
  std::cout << "Execute getOptTata" << std::endl;
  std::optional<std::string> value = res->parameter("tata");
  anch::ut::assertTrue(value.has_value(), "tata not found");
  std::cout << "tata=" << *value << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH resource unit tests")
    .description("Test AnCH resource library")
    .initialize(startup)
    .add("get-toto", getToto)
    .add("get-TOTO-toto", getTOTOToto)
    .add("get-tata", getTata)
    .add("get-opt-toto", getOptToto)
    .add("get-opt-TOTO-toto", getOptTOTOToto)
    .add("get-opt-tata", getOptTata)
    ;
}
