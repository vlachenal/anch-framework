/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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
#include <cstdint>
#include <iostream>
#include <string>

#include "ini/parser.hpp"
#include "ini/section.hpp"

//! [example]
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

int
main() {
  std::cout << "Parse file" << std::endl;
  anch::ini::Section section = anch::ini::parse(std::string("test.ini"));
  printSection(section);
  std::cout << "File has been parsed" << std::endl;
  std::cout << "TOTO.toto = " << section.getValue<std::string>("TOTO.toto").value_or("<none>") << std::endl;
  return 0;
}
//! [example]
