#include <iostream>

#include "conf/configuration.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"


using anch::conf::Configuration;


const Configuration* conf;

void
startup() {
  std::cout << "Parse file" << std::endl;
  conf = &Configuration::loader()
    .name("test")
    .profiles({"UT"})
    .load();
  std::cout << "File has been parsed" << std::endl;
}

void
getToto() {
  std::cout << "Execute getToto" << std::endl;
  std::optional<std::string> val = conf->value("toto");
  anch::ut::assertTrue(val.has_value(), "toto not found");
  std::cout << "toto=" << val.value() << std::endl;
  anch::ut::assert(val.value() == "toto", "Found {} instead of {}", val.value(), "toto");
}

void
getTOTOToto() {
  std::cout << "Execute getTOTOToto" << std::endl;
  std::optional<std::string> val = conf->value("TOTO.toto");
  anch::ut::assertTrue(val.has_value(), "TOTO/toto not found");
  std::cout << "TOTO.toto=" << val.value() << std::endl;
  anch::ut::assert(val.value() == "tata", "Found {} instead of {}", val.value(), "tata");
}

void
getTata() {
  std::cout << "Execute getTata" << std::endl;
  std::optional<std::string> val = conf->value("tata");
  anch::ut::assertTrue(val.has_value(), "tata not found");
  std::cout << "tata=" << val.value() << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH conf unit tests")
    .description("Test AnCH conf library")
    .initialize(startup)
    .add("get-toto", getToto)
    .add("get-TOTO-toto", getTOTOToto)
    .add("get-tata", getTata)
    ;
}
