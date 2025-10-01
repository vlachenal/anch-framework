#include <iostream>

#include "ini/parser.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"


void
addIndent(uint32_t indent) {
  for(uint32_t i = 0 ; i < indent ; ++i) {
    std::cout << ' ';
  }
}

void
printSection(const anch::ini::Section& section, const std::string& name = "root", uint32_t indent = 0) {
  addIndent(indent);
  std::cout << '[' << name << ']' << std::endl;
  for(auto iter = section.getValues().begin() ; iter != section.getValues().end() ; ++iter) {
    addIndent(indent + 1);
    std::cout << iter->first << '=' << iter->second << std::endl;
  }
  for(auto iter = section.getSections().begin() ; iter != section.getSections().end() ; ++iter) {
    printSection(iter->second, iter->first, indent + 2);
  }
}


anch::ini::Section section;

void
startup() {
  std::cout << "Parse file" << std::endl;
  std::string file = "test.ini";
  section = anch::ini::parse(file);
  printSection(section);
  std::cout << "File has been parsed" << std::endl;
}

void
getToto() {
  std::cout << "Execute getToto" << std::endl;
  auto val = section.getValue("toto");
  anch::ut::assertTrue(val.has_value(), "toto not found");
  std::cout << "toto=" << val.value() << std::endl;
}

void
getTOTOToto() {
  std::cout << "Execute getTOTOToto" << std::endl;
  auto val = section.getValue("TOTO.toto");
  anch::ut::assertTrue(val.has_value(), "toto not found");
  std::cout << "TOTO/toto=" << val.value() << std::endl;
}

void
getTata() {
  std::cout << "Execute getTata" << std::endl;
  auto val = section.getValue("tata");
  anch::ut::assertTrue(val.has_value(), "tata not found");
  std::cout << "tata=" << val.value() << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH INI unit tests")
    .description("Test AnCH INI library")
    .initialize(startup)
    .add("parser-toto", getToto)
    .add("parser-TOTO-toto", getTOTOToto)
    .add("parser-tata", getTata)
    ;
}
