#include <iostream>

#include "conf/configuration.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"


using anch::conf::Configuration;


const Configuration* conf;

void
startup() {
  std::cout << "Parse file" << std::endl << std::endl;
  conf = &Configuration::loader()
    .name("test-placeholders")
    .load();
  std::cout << std::endl << "File has been parsed" << std::endl;
}

void
parseSingle() {
  std::cout << "Enter in parseSingle" << std::endl;
  std::optional<std::string> val = conf->value("TOTO.toto");
  anch::ut::assertTrue(val.has_value(), "TOTO/toto not found");
  std::cout << "TOTO.toto=" << val.value() << std::endl;
  // std::string user = std::getenv("USER");
  // anch::ut::assert(val.value() == user", "Found {} instead of {}", val.value(), user);
  anch::ut::assert(val.value() == "${env=USER}", "Found {} instead of {}", val.value(), "${env=USER}");
  std::cout << "Exit parseSingle" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH conf placeholders unit tests")
    .description("Test AnCH conf placeholders")
    .initialize(startup)
    .add("single", parseSingle)
    ;
}
