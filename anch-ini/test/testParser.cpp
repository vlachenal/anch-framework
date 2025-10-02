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
getValFromRoot() {
  std::cout << "Execute getValFromRoot" << std::endl;
  auto val = section.getValue("toto");
  anch::ut::assertTrue(val.has_value(), "toto not found");
  std::cout << "toto=" << val.value() << std::endl;
}

void
getValInSub() {
  std::cout << "Execute getValInSub" << std::endl;
  anch::ut::assertTrue(section.getSections().contains("TOTO"));
  auto val = section.getValue("TOTO.toto");
  anch::ut::assertTrue(val.has_value(), "toto not found");
  std::cout << "TOTO/toto=" << val.value() << std::endl;
}

void
getValSubInSub() {
  std::cout << "Execute getValSubInSub" << std::endl;
  anch::ut::assertTrue(section.getSections().contains("TOTO"));
  auto sec = section.getSections().find("TOTO")->second;
  anch::ut::assertTrue(sec.getSections().contains("tata"));
  auto val = section.getValue("TOTO.tata.tyty");
  anch::ut::assertTrue(val.has_value(), "tyty not found");
  std::cout << "TOTO/tata/tyty=" << val.value() << std::endl;
}

void
getValInSubSub() {
  std::cout << "Execute getPlopPlipPlup" << std::endl;
  anch::ut::assertTrue(section.getSections().contains("plop"));
  auto sec = section.getSections().find("plop")->second;
  anch::ut::assertTrue(sec.getSections().contains("plip"));
  auto val = section.getValue("plop.plip.plup");
  anch::ut::assertTrue(val.has_value(), "plup not found");
  std::cout << "plop/plip/plup=" << val.value() << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH INI unit tests")
    .description("Test AnCH INI library")
    .initialize(startup)
    .add("parser-root-val", getValFromRoot)
    .add("parser-root-sub-val", getValInSub)
    .add("parser-root-sub-keysub-val", getValSubInSub)
    .add("parser-root-subsub-val", getValInSubSub)
    ;
}
